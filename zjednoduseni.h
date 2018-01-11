

// Slovnicek :-)

#define kladne unsigned
#define nezmenitelne const
#define nezmenitelny const
#define promenlive volatile
#define promenlivy volatile
#define cislo int
#define cas unsigned long
#define tvrzeni boolean
#define pravda true
#define nepravda false
#define procedura void

#define ZAPNUTO HIGH
#define VYPNUTO LOW

#define VSTUP INPUT
#define VYSTUP OUTPUT
#define VSTUP_S_VNITRNIM_ODPOREM INPUT_PULLUP

#define SESTUPNA_HRANA FALLING
#define VZESTUPNA_HRANA RISING

#define konstanta nezmenitelne kladne cislo

#define cekej delay
#define kdyz if
#define jinak else
#define napis Serial.println
#define kusNapisu Serial.print
#define nastavRychlostPsaniNapisu Serial.begin
#define zakazVyrusovani noInterrupts
#define povolVyrusovani interrupts
#define prectiPin digitalRead
#define nastavTo setup
#define delejTo loop
#define nastavPin pinMode


procedura zapni(kladne cislo pin) {
  digitalWrite(pin, ZAPNUTO);
}

procedura vypni(kladne cislo pin) {
  digitalWrite(pin, VYPNUTO);
}

procedura nastavObsluhuVyrusovaniProPin (kladne cislo pin, procedura *p()) {
  attachInterrupt (digitalPinToInterrupt(pin), p, SESTUPNA_HRANA);
};
