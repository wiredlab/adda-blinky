


#define ADC_DAC_REFERENCE INTERNAL4V3
#define ANALOG_IN_PIN PIN_PA5
#define ANALOG_OUT_PIN PIN_PA6
#define SAMPLE_PIN PIN_PB5
#define POT_CHECK_INTERVAL_MS 2000
#define POT_PIN PIN_PB4

void setup() {
  // LED output pins
  // PC3..0 --> LED3..0
  PORTC.DIR = (PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm);

  // PB3..0 --> LED7..4
  PORTB.DIR = (PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm);

  // PB5 is "Sample indicator" LED
  PORTB.DIRSET = PIN5_bm;


  // Configure the ADC and DAC references
  VREF.CTRLA = (0x03 << 4) | (0x03 << 0);
  VREF.CTRLB = VREF_ADC0REFEN_bm | VREF_DAC0REFEN_bm;

  // ADC setup
  /*
  ADC0.CTRLA = ADC_RESSEL_bm | ADC_FREERUN_bm | ADC_ENABLE_bm;
  ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_PRESC_DIV2_gc;
  ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
  ADC0.COMMAND = 1;
  */
  analogReference(INTERNAL4V3);

  // DAC setup
  DAC0.CTRLA = DAC_OUTEN_bm | DAC_ENABLE_bm;


  // Configure timer to set the sample rate
  // clock divider
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

  // periodic interrupt
  TCA0.SINGLE.CMP0 = 1000;
  TCA0.SINGLE.PER = 1000;

  sei();
}


void set_data_leds(int16_t data8b) {
  PORTC.OUT = (0xF0 & PORTC.OUT) | (0x0F & ~data8b);
  PORTB.OUT = (0xF0 & PORTB.OUT) | (0x0F & (~data8b >> 4));
}



void set_sample_rate(int16_t val) {
  if (val < 0x20) {
    // 1 Hz
  }
  else if (val < 0x40) {
    // 10 Hz
  }
  else if (val < 0x60) {
    // 100 Hz
  }
  else if (val < 0x80) {
    // 1 kHz
  }
  else if (val < 0xA0) {
    // 3 kHz
  }
  else if (val < 0xC0) {
    // 10 kHz
  }
  else if (val < 0xE0) {
    // 20 kHz
  }
  else {
    // max
  }
}

ISR(TCA0_OVF_vect) {
  int16_t signal_val;
  int16_t adc_raw;

  // Sample indicator
  digitalWrite(SAMPLE_PIN, 1);
  
  // sample
  
  //signal_val = 0xFF & millis()>>5;
  adc_raw = analogRead(ANALOG_IN_PIN);
  signal_val = adc_raw >> 2;

  //signal_val = analogRead(POT_PIN) >> 2;
  //signal_val = ADC0.RESL;
  
  // display
  set_data_leds(signal_val);

  // output
  //analogWrite(ANALOG_OUT_PIN, signal_val);
  DAC0.DATA = signal_val;


  // AVR 1-series needs to clear the interrupt flag
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
  ADC0.COMMAND = 1;

  // Sample indicator
  digitalWrite(SAMPLE_PIN, 0);
}



void loop() {
  unsigned long last_check = 0;
  unsigned long now;
  int16_t potval;

  now = millis();

  if ((now - last_check) > POT_CHECK_INTERVAL_MS) {
    cli();
    potval = analogRead(POT_PIN) >> 2;
    sei();
    set_sample_rate(potval);
    //set_data_leds(potval);

    //digitalWrite(PIN_PC0, ADC0.COMMAND);
    TCA0.SINGLE.PER = 20000;
    last_check = now;
  }
}
