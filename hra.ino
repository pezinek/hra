
#include "zjednoduseni.h"

// konstanty
konstanta pinBlikaciDiody = 13;
konstanta pinTlacitka = 2;
konstanta pinNejnizsiDiodaScore = 3; // první dioda, která zobrazuje score
konstanta pinNejvyssiDiodaScore = 12; // poslední dioda, která zobrazuje score
konstanta krokZrychlovani = 50; // ms - dioda bliká s lineárním zrychlením, s každým bodem o krokZrychlovani dolu
konstanta nejmensiCas = 200; //ms - nejmenší čas, který dioda svítí. Pod toto nejde nezávisle na score
konstanta kladneBody = 1; // o kolik bodů se zvýší score při trefě do svitu diody
konstanta nasobitelZaporneBody = 3; // body budes ztracet 3x rychleji nez ziskavat
konstanta okamzik = 16000;  // kolik mikrosekund čekat po zmáčknutí tlačítka (ochrana proti zákmitům)
cas periodaKontrolyTlacitka = 100; // ms - jak casto kontrolovat jestli je zmacknuto tlacitko

// promenne
cislo score = 0; // score na chvilku může klesout pod 0 - divné, ale mám tak udělané spíše pro jednoduchost kódu
tvrzeni sviti = nepravda; // jestli dioda právě ted svítí
cislo dobaSvitu = 1000; // v ms - doba svitu diody, zároveň je doba mezery blikání
cas posledniKontrolaTlacitka = 0; // cas kdy se naposledy kontrolovalo zmacknuti tlacitka
cislo posledniStavTlacitka = VYPNUTO;  // posledni znamy stav tlacitka


procedura rozsvitDiodu() { 
  zapni(pinBlikaciDiody);
  sviti = pravda;
}

procedura zhasniDiodu() { 
  vypni(pinBlikaciDiody); 
  sviti = nepravda;
}

procedura prepniDiodu() {
  kdyz (sviti) {
    zhasniDiodu();
  } jinak {
    rozsvitDiodu();
  }
}

procedura zhasniScore () {
   for (cislo i=pinNejnizsiDiodaScore; i<=pinNejvyssiDiodaScore; i++) { vypni(i); };
}

procedura predstavSe () {
  for (score = 0; score <= 10; score++) {rozsvitScoreJednouPosouvajiciSeDiodou(); cekej(20);};
  for (score = 10; score >= 0; score--) {rozsvitScoreJednouPosouvajiciSeDiodou(); cekej(20);};
}

// jedna možnost jak ukázat score
procedura rozsvitScoreVeDvojkoveSoustave () {
  // umi max 8 diod, tj. score 0 az 255. Diody na pinech 11 a 12 jsou zbytecne
  zhasniScore();
  kdyz (score > 255) {score = 1;}; 
  kdyz (score & B00000001) { zapni(pinNejnizsiDiodaScore+0); };
  kdyz (score & B00000010) { zapni(pinNejnizsiDiodaScore+1); };
  kdyz (score & B00000100) { zapni(pinNejnizsiDiodaScore+2); };
  kdyz (score & B00001000) { zapni(pinNejnizsiDiodaScore+3); };
  kdyz (score & B00010000) { zapni(pinNejnizsiDiodaScore+4); };
  kdyz (score & B00100000) { zapni(pinNejnizsiDiodaScore+5); };
  kdyz (score & B01000000) { zapni(pinNejnizsiDiodaScore+6); };
  kdyz (score & B10000000) { zapni(pinNejnizsiDiodaScore+7); };
}

// druhá možnost jak ukázat score
procedura rozsvitScoreJednouPosouvajiciSeDiodou () {
  zhasniScore();
  konstanta maximalniScore = pinNejvyssiDiodaScore - pinNejnizsiDiodaScore;
  kdyz (score > maximalniScore) {score = 1;};
  zapni(pinNejnizsiDiodaScore + score);
}

procedura rozsvitScore () {
  kdyz (score < 0) {score = 0;};
  //rozsvitScoreJednouPosouvajiciSeDiodou ();
  rozsvitScoreVeDvojkoveSoustave ();
}

// složitější  funkce, které už jsou k věci

procedura zapoctiTrefu() {
  score = score + kladneBody;
  dobaSvitu = dobaSvitu - krokZrychlovani;
  kdyz (dobaSvitu <= nejmensiCas) {dobaSvitu = nejmensiCas;};
  
  rozsvitScore ();
  napis("trefa ");
  napis(score);
  
}

procedura zapoctiChybu() {
  score = score - (kladneBody * nasobitelZaporneBody);
  dobaSvitu = dobaSvitu + (krokZrychlovani * nasobitelZaporneBody);

  kdyz (score <= 0) { score = 0; }  // Score nemuze byt mene nez 0
  
  rozsvitScore ();
  napis("chyba ");
  napis(score);  
}


// zkontroluj jestli se prave zmacklo tlacitko a jestli jo
// zapocti chybu nebo trefu.

procedura obsluzTlacitko() {

  cas ted = millis();  // promennou "ted" nastav na aktualni cas

  kdyz (posledniKontrolaTlacitka + periodaKontrolyTlacitka > ted) {

    cislo soucasnyStavTlacitka = prectiPin(pinTlacitka);
    
    kdyz ((posledniStavTlacitka == VYPNUTO) && (soucasnyStavTlacitka == ZAPNUTO))  {  // nekdo to od posledni kontroly zmacknul    
        kdyz (sviti) {
          zapoctiTrefu();
        } jinak {
          zapoctiChybu();
        }
    }

    posledniStavTlacitka = soucasnyStavTlacitka;
    posledniKontrolaTlacitka = ted;  
  }
}

cas posledniBliknuti = 0;

procedura obsluzBlikani() {

  cas ted = millis();  // promennou "ted" nastav na aktualni cas

  kdyz (posledniBliknuti + dobaSvitu > ted) {
    prepniDiodu();
    posledniBliknuti = ted;
  }
  
}

// Zavolá se samo od sebe když se to zapne
procedura nastavTo() {
  
  nastavPin(pinTlacitka, VSTUP_S_VNITRNIM_ODPOREM);
  for (cislo i=pinNejnizsiDiodaScore; i<=pinNejvyssiDiodaScore; i++) {
    nastavPin(i, VYSTUP);
  };
  nastavPin(pinBlikaciDiody, VYSTUP);
  predstavSe ();
  score = 0;
  zhasniScore();
  
}

// Volá se samo od sebe pořád dokola (ale až potom co se to nastavi)
procedura delejTo() {
  
 obsluzTlacitko();
 obsluzBlikani();

}

