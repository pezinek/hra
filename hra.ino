
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

// tyto se nastavují z obsluhy přerušení
promenlive cislo score = 0; // score na chvilku může klesout pod 0 - divné, ale mám tak udělané spíše pro jednoduchost kódu
promenlive tvrzeni trefa = nepravda; // jestli se hráč trefil do svitu diody
promenlive tvrzeni sviti = nepravda; // jestli dioda právě ted svítí
promenlive cislo dobaSvitu = 1000; // v ms - doba svitu diody, zároveň je doba mezery blikání
promenlivy cas lastMicros = 0; // pro podporu řešení zákmitů tlačítka


procedura rozsvitDiodu() { zapni(pinBlikaciDiody); };
procedura zhasniDiodu() { vypni(pinBlikaciDiody); };

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
  konstanta maximalni_score = pinNejvyssiDiodaScore - pinNejnizsiDiodaScore;
  kdyz (score > maximalni_score) {score = 1;};
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
  score = score - (kladneBody*nasobitelZaporneBody);
  dobaSvitu = dobaSvitu + (krokZrychlovani*nasobitelZaporneBody);

  kdyz (score <= 0) { score = 0; }  // Score nemuze byt mene nez 0
  
  rozsvitScore ();
  napis("chyba ");
  napis(score);  
}


procedura stisk () {
 //ochrana proti zákmitům - je to legitimni stisk
 zakazVyrusovani ();
 cas tedMicros = micros(); //neresim moznost preteceni, v praxi nezpůsobuje potíže
 cislo stav = prectiPin(pinTlacitka);
 kdyz (stav == VYPNUTO) {
   kdyz ((tedMicros - lastMicros) > okamzik) { 
    lastMicros = tedMicros;
    kdyz (sviti) {
      zapoctiTrefu();
    } jinak {
      zapoctiChybu();
    }
   }
 }
}

// Zavolá se sama od sebe když se to zapne
procedura nastavTo() {
  nastavPin(pinTlacitka, VSTUP_S_VNITRNIM_ODPOREM);
  for (cislo i=pinNejnizsiDiodaScore; i<=pinNejvyssiDiodaScore; i++) {
    nastavPin(i, VYSTUP);
  };
  nastavPin(pinBlikaciDiody, VYSTUP);
  predstavSe ();
  score = 0;
  zhasniScore();
  zakazVyrusovani ();
  nastavObsluhuVyrusovaniProPin(pinTlacitka, stisk);  // Zavolej stisk kdyz te vyrusi pin pinTlacitka
}

// Volá se sama od sebe pořád dokola (ale až potom co zkončí nastavTo)
procedura delejTo() {
 rozsvitDiodu(); 
 sviti = pravda;
 povolVyrusovani();
 cekej(dobaSvitu);
 
 zhasniDiodu(); 
 sviti = nepravda;  
 povolVyrusovani();
 cekej(dobaSvitu);
}

