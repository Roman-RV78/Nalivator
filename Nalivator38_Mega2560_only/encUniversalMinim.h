// мини-класс для работы с энкодером, версия 1.0 мод

class encMinim
{
  public:
    encMinim(uint8_t clk, uint8_t dt, uint8_t sw, bool dir);
    void tick();
    boolean isSingle();
    boolean isTurn();
    boolean isRight();
    boolean isLeft();
    boolean isHolded();
    boolean isDouble();
    boolean isHold();
    void rst();

  private:
    uint8_t _clk, _dt, _sw;
    bool _turnFlag, _swState, _swFlag, _clickFlag, _flagsTurn ;
    uint8_t _state, _lastState, _encState, _countClick;
    uint32_t _debTimer;
    bool _resetFlag = false;
    // 0 - ничего, 1 - лево, 2 - право, 5 - одиночный клик , 6 - двойной клик, 7 - удержание
};

encMinim::encMinim(uint8_t clk, uint8_t dt, uint8_t sw, boolean dir) {
  if (dir) {
    _clk = clk;
    _dt = dt;
  } else {
    _clk = dt;
    _dt = clk;
  }
  _sw = sw;
  pinMode (_clk, INPUT_PULLUP);
  pinMode (_dt, INPUT_PULLUP);
  pinMode (_sw, INPUT_PULLUP);
}

void encMinim::tick() {

  _state = digitalRead(_clk) | digitalRead(_dt) << 1;
  if (_resetFlag && _state == 0b11) {
    if (_lastState == 0b10) _encState =  1;
    if (_lastState == 0b01) _encState =  2;
    _resetFlag = false;
  }
  if (_state == 0b00) _resetFlag = true;
  _lastState = _state;


  _swState = digitalRead(_sw);
  if (!_swState && !_swFlag && millis() - _debTimer > 50 ) {
    _debTimer = millis();
    _swFlag = true;
    _clickFlag  = true;
    _countClick++;
  }
  if (_swState && _swFlag && millis() - _debTimer > 50 ) {
    _debTimer = millis();
    _swFlag = false;
  }

  if (_clickFlag && !_swFlag && millis() -  _debTimer > 200  ) { // время двойного и последующих кликов
    _clickFlag  = false;
    if (_countClick < 2) _encState = 5;
    else _encState = 7;
    _countClick = 0;
  }

  if (_clickFlag  &&  _swFlag && millis() - _debTimer > 1000 ) {  // время удержания
    _clickFlag  = false;
    _encState = 6;
    _countClick = 0;
  }
  if (_encState > 0 && _encState < 5) _flagsTurn = true;
  
}

void encMinim::rst() {
  _encState = 0;
}

boolean encMinim::isTurn() {
  if (_flagsTurn) {
    _flagsTurn = false;
    return true;
  } else return false;
}
boolean encMinim::isRight() {
  if (_encState == 1) {
    _encState = 0;
    return true;
  } else return false;
}
boolean encMinim::isLeft() {
  if (_encState == 2) {
    _encState = 0;
    return true;
  } else return false;
}

boolean encMinim::isSingle() {
  if (_encState == 5) {
    _encState = 0;
    return true;
  } else return false;
}

boolean encMinim::isHolded() {
  if (_encState == 6) {
    _encState = 0;
    return true;
  } else return false;
}

boolean encMinim::isHold() {
  return (!_swState);
}


boolean encMinim::isDouble() {
  if (_encState == 7) {
    _encState = 0;
    return true;
  } else return false;
}
