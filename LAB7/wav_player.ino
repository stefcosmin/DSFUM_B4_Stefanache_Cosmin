//TMRpcm library is needed
//comenzi serial (9600 baud, terminat cu newline): MAKE, PLAY 0, PLAY 1, STOP, NEXT, PREV, LIST
//PLAY 0 = linear, PLAY 1 = random

#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"

#define PF "PLAYLIST.TXT"  //fisierul playlist
#define PW 24              //lungime maxima cale
#define RL 32              //lungime fixa a unei linii din playlist

TMRpcm tmrpcm;
File pl;

char pb[PW + 2];  //buffer cale pentru scanare
char rp[PW + 1];  //calea citita din playlist
unsigned long dd; //durata citita din playlist
char cb[16];      //buffer comanda
byte ci = 0;
int n = 0;        //nr melodii
int c = -1;       //melodia curenta
int pv = 0;       //melodia anterioara (pentru random)
bool on = false;  //true cand se canta
bool rnd = false; //true cand e random
bool ok = false;  //true cand cardul e initializat

//numarul de melodii din playlist
int cnt()
{
  File f = SD.open(PF);
  if (!f) return 0;
  unsigned long s = f.size();
  f.close();
  return s / RL;
}

//citeste linia i din playlist in rp si dd
bool rd(int i)
{
  char r[RL + 1];
  File f = SD.open(PF);
  if (!f) return false;
  f.seek((unsigned long)i * RL);
  int k = f.read(r, RL);
  f.close();
  if (k < RL) return false;
  r[RL] = 0;
  byte j = PW;
  while (j > 0 && r[j - 1] == ' ') j--;
  memcpy(rp, r, j);
  rp[j] = 0;
  dd = atol(r + PW + 1);
  return true;
}

//durata in secunde dintr-un wav, calculata din header
unsigned long wd(char *p)
{
  File f = SD.open(p);
  if (!f) return 0;
  unsigned long s = f.size();
  unsigned long b = 0;
  f.seek(28);
  f.read(&b, 4);
  f.close();
  if (b == 0 || s < 44) return 0;
  return (s - 44) / b;
}

//adauga o linie in playlist
void add(char *p)
{
  char b[8];
  unsigned long d = wd(p);
  pl.print(p);
  for (byte i = strlen(p); i < PW; i++) pl.print(' ');
  sprintf(b, " %05lu", d);
  pl.print(b);
  pl.print("\r\n");
  n++;
}

//scanare recursiva, l = lungimea caii curente din pb
void scan(File &d, byte l)
{
  File e;
  while ((e = d.openNextFile()))
  {
    const char *nm = e.name();
    byte m = strlen(nm);
    if (e.isDirectory())
    {
      if (l + m + 1 <= PW)
      {
        memcpy(pb + l, nm, m);
        pb[l + m] = '/';
        pb[l + m + 1] = 0;
        scan(e, l + m + 1);
      }
    }
    else if (m > 4 && l + m <= PW && strcasecmp(nm + m - 4, ".WAV") == 0)
    {
      memcpy(pb + l, nm, m);
      pb[l + m] = 0;
      add(pb);
    }
    e.close();
  }
  pb[l] = 0;
}

//comanda MAKE
void mk()
{
  tmrpcm.stopPlayback();
  on = false;
  c = -1;
  SD.remove(PF);
  pl = SD.open(PF, FILE_WRITE);
  if (!pl)
  {
    Serial.println(F("ERR playlist"));
    return;
  }
  n = 0;
  pb[0] = 0;
  File r = SD.open("/");
  scan(r, 0);
  r.close();
  pl.close();
  Serial.print(F("OK "));
  Serial.print(n);
  Serial.println(F(" melodii"));
}

//canta melodia i
void go(int i)
{
  if (!rd(i))
  {
    Serial.println(F("ERR citire"));
    on = false;
    return;
  }
  tmrpcm.stopPlayback();
  tmrpcm.play(rp);
  pv = c;
  c = i;
  on = true;
  Serial.print(F("PLAY "));
  Serial.println(rp);
}

//urmatoarea melodie
void nx()
{
  n = cnt();
  if (n == 0)
  {
    on = false;
    Serial.println(F("ERR playlist gol"));
    return;
  }
  if (rnd) go(random(n));
  else go((c + 1) % n);
}

//melodia precedenta
void pr()
{
  n = cnt();
  if (n == 0)
  {
    on = false;
    Serial.println(F("ERR playlist gol"));
    return;
  }
  if (rnd)
  {
    if (pv < 0 || pv >= n) pv = 0;
    go(pv);
  }
  else if (c <= 0) go(n - 1);
  else go(c - 1);
}

//comanda LIST
void ls()
{
  n = cnt();
  for (int i = 0; i < n; i++)
  {
    if (!rd(i)) break;
    Serial.print(i + 1);
    Serial.print(F(". "));
    Serial.print(rp);
    Serial.print(F(" "));
    Serial.print(dd / 60);
    Serial.print(F(":"));
    if (dd % 60 < 10) Serial.print(F("0"));
    Serial.println(dd % 60);
  }
  Serial.println(F("OK"));
}

//executa comanda din cb
void cmd()
{
  if (!ok)
  {
    Serial.println(F("ERR SD"));
    return;
  }
  if (strcmp(cb, "MAKE") == 0) mk();
  else if (strncmp(cb, "PLAY", 4) == 0)
  {
    rnd = (cb[5] == '1');
    c = -1;
    nx();
  }
  else if (strcmp(cb, "STOP") == 0)
  {
    tmrpcm.stopPlayback();
    on = false;
    Serial.println(F("OK stop"));
  }
  else if (strcmp(cb, "NEXT") == 0) nx();
  else if (strcmp(cb, "PREV") == 0) pr();
  else if (strcmp(cb, "LIST") == 0) ls();
  else Serial.println(F("ERR comanda"));
}

void setup()
{
  tmrpcm.speakerPin = 9;
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD fail");
    return;
  }
  ok = true;
  tmrpcm.setVolume(6);
  randomSeed(analogRead(A0));
  n = cnt();
  Serial.println(F("Gata"));
}

void loop()
{
  //citire comenzi de la serial
  while (Serial.available())
  {
    char x = Serial.read();
    if (x == '\n' || x == '\r')
    {
      if (ci)
      {
        cb[ci] = 0;
        ci = 0;
        cmd();
      }
    }
    else if (ci < 15) cb[ci++] = toupper(x);
  }
  //trecere automata la urmatoarea melodie
  if (ok && on && !tmrpcm.isPlaying()) nx();
}
