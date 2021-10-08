void Tost() {
  static bool readyTost = false;
  if ( tost ) {
    if (!readyTost && PAUSEtimer.isReady()) {
      if (volume != 0 && tracks != -1 ) {
        myMP3.setVolume(volume);
        lcd.clear();
        lcd.setCursor(4, 0);
        print_lcd(2);//  ТРЕК
        lcd.setCursor(11, 0);
        lcd.print((TostList[num] + 1), DEC);
        lcd.setCursor(2, 1);
        print_lcd(3);//  !!! ТОСТ !!!
        myMP3.playFolderTrack(folder, (TostList[num] + 1));
        if (++num >= tracks) num = 0;
      }
      readyTost = true;
      PAUSEtimer.setInterval(500);
      PAUSEtimer.reset();
    }
      if ((readyTost && ((PAUSEtimer.isReady() && digitalRead(BUSY_PIN)) || noTost )) || volume == 0 || tracks == -1) {
      tost = false;
      if (noTost) {
        noTost = false;
        myMP3.stop();
      }
      SAVEtimer.reset();
      readyTost = false;
      if (MenuFlag == 31) {
        menu_vol(0);
      } else {
        if (playOn) {
          player = true;
          PLAYtimer.reset();
          pause = false;
        }
        returnMenu = true;
      }
    }
  }
}
