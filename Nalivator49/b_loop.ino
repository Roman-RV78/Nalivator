void loop() {
#ifdef BAT_MONITOR_ON
  if (!lowBat) {
#endif
    encTick();
    flowTick();
    CvetoMuzik();
    play_mushket();
    energy_saving();
    Tost();
    ret_menu();
    play_next();
#ifdef BUTTON_TOWER
    button_tower ();
#endif
#ifdef BAT_MONITOR_ON
  }
  bat_tery();
#endif
}
