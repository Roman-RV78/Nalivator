/*-------------------------------------------------------------------------
  DFMiniMp3 library

  Written by Michael C. Miller.

  I invest time and resources providing this open source code,
  please support me by dontating (see https://github.com/Makuna/DFMiniMp3)

  -------------------------------------------------------------------------
  This file is part of the Makuna/DFMiniMp3 library.

  DFMiniMp3 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  DFMiniMp3 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with DFMiniMp3.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/
#pragma once


enum DfMp3_Eq
{
  DfMp3_Eq_Normal,
  DfMp3_Eq_Pop,
  DfMp3_Eq_Rock,
  DfMp3_Eq_Jazz,
  DfMp3_Eq_Classic,
  DfMp3_Eq_Bass
};



// 7E FF 06 0F 00 01 01 xx xx EF
// 0	->	7E is start code
// 1	->	FF is version
// 2	->	06 is length
// 3	->	0F is command
// 4	->	00 is no receive
// 5~6	->	01 01 is argument
// 7~8	->	checksum = 0 - ( FF+06+0F+00+01+01 )
// 9	->	EF is end code
struct DfMp3_Packet_WithCheckSum {
  uint8_t startCode;
  uint8_t version;
  uint8_t length;
  uint8_t command;
  uint8_t requestAck;
  uint8_t hiByteArgument;
  uint8_t lowByteArgument;
  uint8_t hiByteCheckSum;
  uint8_t lowByteCheckSum;
  uint8_t endCode;
};
struct DfMp3_Packet_WithoutCheckSum {
  uint8_t startCode;
  uint8_t version;
  uint8_t length;
  uint8_t command;
  uint8_t requestAck;
  uint8_t hiByteArgument;
  uint8_t lowByteArgument;
  uint8_t endCode;
};


uint16_t calcChecksum(const DfMp3_Packet_WithCheckSum& packet) {
  uint16_t sum = 0xFFFF;
  for (const uint8_t* packetByte = &(packet.version); packetByte != &(packet.hiByteCheckSum); packetByte++) {
    sum -= *packetByte;
  }
  return sum + 1;
}

void setChecksum(DfMp3_Packet_WithCheckSum* out) {
  uint16_t sum = calcChecksum(*out);
  out->hiByteCheckSum = (sum >> 8);
  out->lowByteCheckSum = (sum & 0xff);
}

bool validateChecksum(const DfMp3_Packet_WithCheckSum& in) {
  uint16_t sum = calcChecksum(in);
  return (sum == static_cast<uint16_t>((in.hiByteCheckSum << 8) | in.lowByteCheckSum));
}

class Mp3ChipMH2024K16SS {
  public:
    //static const bool SendCheckSum = false;

    typedef DfMp3_Packet_WithoutCheckSum SendPacket;
    typedef DfMp3_Packet_WithCheckSum ReceptionPacket;

    static const SendPacket generatePacket(uint8_t command, uint16_t arg) {
      return {
        0x7E,
        0xFF,
        6,
        command,
        0,
        static_cast<uint8_t>(arg >> 8),
        static_cast<uint8_t>(arg & 0x00ff),
        0xEF };
    }
};

class Mp3ChipOriginal {
  public:
    //static const bool SendCheckSum = true;

    typedef DfMp3_Packet_WithCheckSum SendPacket;
    typedef DfMp3_Packet_WithCheckSum ReceptionPacket;

    static const SendPacket generatePacket(uint8_t command, uint16_t arg) {
      SendPacket packet = {
        0x7E,
        0xFF,
        6,
        command,
        0,
        static_cast<uint8_t>(arg >> 8),
        static_cast<uint8_t>(arg & 0x00ff),
        0,
        0,
        0xEF
      };
      setChecksum(&packet);
      return packet;
    }
};


template <class T_SERIAL_METHOD, class T_CHIP_VARIANT = Mp3ChipOriginal> class DFMiniMp3 {
  public:
    explicit DFMiniMp3(T_SERIAL_METHOD& serial) :
      _serial(serial),
      _lastSendSpace(c_msSendSpace)
    {
    }

    void begin(uint16_t baud = 9600, uint16_t timeout = 10000) {
      _serial.begin(baud);
      _serial.setTimeout(timeout);
      _lastSend = millis();
    }

    void loop() {
      while (_serial.available() >= static_cast<int>(sizeof(typename T_CHIP_VARIANT::ReceptionPacket))) {
        listenForReply(0x00);
      }
    }



    // sd:/mp3/####track name
    void playMp3FolderTrack(uint8_t track) {
      sendPacket(0x12, track);
    }

    // older devices: sd:/###/###track name
    // newer devices: sd:/##/###track name
    // folder and track numbers are zero padded
    void playFolderTrack(uint8_t folder, uint8_t track) {
      uint16_t arg = (folder << 8) | track;
      sendPacket(0x0f, arg);
    }



    // 0- 30
    void setVolume(uint8_t volume) {
      sendPacket(0x06, volume);
    }


    void setEq(DfMp3_Eq eq) {
      sendPacket(0x07, eq);
    }



    void start() {
      sendPacket(0x0d);
    }

    void pause() {
      sendPacket(0x0e);
    }

    void stop() {
      sendPacket(0x16);
    }



    uint8_t getFolderTrackCount(uint8_t folder) {
      drainResponses();
#ifdef SENDING_MULTIPLE_COMMANDS
      return sendAndListenForReply(0x4e, folder);
#else
      sendPacket(0x4e, folder);
      return listenForReply(0x4e);
#endif
    }



  private:
#ifndef PLAYER_MH2024K_24SS
    static const uint16_t c_msSendSpace = 50;
#else
    static const uint16_t c_msSendSpace = 1500;
#endif

    T_SERIAL_METHOD& _serial;
    uint32_t _lastSend; // not initialized as agreed in issue #63
    uint16_t _lastSendSpace;

    void drainResponses() {
      while (_serial.available() > 0) {
        listenForReply(0x00);
      }
    }

    void sendPacket(uint8_t command, uint16_t arg = 0, uint16_t sendSpaceNeeded = c_msSendSpace) {
      typename T_CHIP_VARIANT::SendPacket packet = T_CHIP_VARIANT::generatePacket(command, arg);

      // wait for spacing since last send
      while (((millis() - _lastSend) < _lastSendSpace)) {
        // check for event messages from the device while
        // we wait
        loop();
        delay(1);
      }

      _lastSendSpace = sendSpaceNeeded;
      _serial.write(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
      _lastSend = millis();
    }

    bool readPacket(uint8_t* command, uint16_t* argument) {
      typename T_CHIP_VARIANT::ReceptionPacket in;
      uint8_t read;

      // init our out args always
      *command = 0;
      *argument = 0;

      // try to sync our reads to the packet start
      do {
        // we use readBytes as it gives us the standard timeout
        read = _serial.readBytes(&in.startCode, 1);

        if (read != 1) return false;

      } while (in.startCode != 0x7e);

      read += _serial.readBytes(&in.version, sizeof(in) - 1);
      if (read < sizeof(in)) return false;


      if (in.version != 0xFF || in.length != 0x06 || in.endCode != 0xef) return false;

      if (!validateChecksum(in)) return false;


      *command = in.command;
      *argument = ((in.hiByteArgument << 8) | in.lowByteArgument);
      return true;
    }

    int16_t listenForReply(uint8_t command) {
      uint8_t replyCommand = 0;
      uint16_t replyArg = 0;


      do {
        if (readPacket(&replyCommand, &replyArg)) {
          if (command != 0 && command == replyCommand) {
            return replyArg;
          } else {
            if (replyCommand == 0x40 ) return 0;
          }
        } else {
          if (replyArg != 0) {
            if (_serial.available() == 0) {
#ifdef SENDING_MULTIPLE_COMMANDS
              return -1;
#else
              return 0;
#endif
            }
          } else {
            return 0;
          }
        }
      } while (command != 0);

      return 0;
    }
#ifdef SENDING_MULTIPLE_COMMANDS
    int16_t sendAndListenForReply(uint8_t command, uint16_t arg = 0) {
      int16_t rep = -1;
      for (uint8_t i = 0; i < 3 && rep == -1; i++) {
        sendPacket(command, arg);
        rep = listenForReply(command);
      }
      if (rep != -1) return rep;
      else return 0;
    }
#endif
};
