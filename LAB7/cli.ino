//shell pe card sd: cd, mkdir, rmdir, ls, cat, echo, find, cp, crc
//cat fisier.bmp afiseaza imaginea ca text art (bmp 24 sau 8 biti, necomprimat)
//cat fisier.bmp 1 inverseaza luminozitatea (pentru fundal alb)

#include <SPI.h>
#include <SD.h>

#define TW 60 //latimea maxima a imaginii text
#define TH 60 //inaltimea maxima a imaginii text

//pin chip select pt sd
const int chipSelect = 4;

String currentDir = "/";
bool ok = false; //true cand cardul e initializat

//date bmp
unsigned long bo; //inceputul datelor de imagine
unsigned long bs; //lungimea unui rand in fisier
unsigned long bp; //inceputul paletei
long bw, bh;      //latime si inaltime
byte bt;          //biti pe pixel
bool bd;          //true daca imaginea e salvata de sus in jos
char ramp[] = " .:-=+*#%@"; //de la intunecat la luminos

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("Initializare card SD..."));
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Eroare: Cardul SD nu a putut fi initializat sau lipseste!"));
    return;
  }
  ok = true;
  Serial.println(F("Card SD initializat cu succes."));
  Serial.println(F("Introduceti comenzi (cd, mkdir, rmdir, ls, cat, echo, find, cp, crc):"));
  printPrompt();
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      Serial.println(input); //confirma comanda primita
      executeCommand(input);
      printPrompt();
    }
  }
}

void printPrompt() {
  Serial.print(currentDir);
  Serial.print(F("> "));
}

//lipeste un nume la o cale, fara dublu slash
String jn(String a, String b) {
  if (a == "/") return "/" + b;
  return a + "/" + b;
}

//functie pentru a determina calea absoluta sau relativa
String resolvePath(String path) {
  String r;
  if (path.startsWith("./")) {
    path = path.substring(2);
  }
  if (path.startsWith("/")) {
    r = path; //cale absoluta
  } else {
    r = jn(currentDir, path);
  }
  //scoate slash-ul de la final
  while (r.length() > 1 && r.endsWith("/")) {
    r.remove(r.length() - 1);
  }
  return r;
}

void executeCommand(String cmdLine) {
  if (!ok) {
    Serial.println(F("Eroare: Cardul SD nu este initializat."));
    return;
  }
  //ia prima comanda
  int spaceIndex = cmdLine.indexOf(' ');
  String cmd = (spaceIndex == -1) ? cmdLine : cmdLine.substring(0, spaceIndex);
  String args = (spaceIndex == -1) ? "" : cmdLine.substring(spaceIndex + 1);
  args.trim();
  cmd.toLowerCase();

  if (cmd == "cd") {
    cmdCd(args);
  } else if (cmd == "mkdir") {
    cmdMkdir(args);
  } else if (cmd == "rmdir") {
    cmdRmdir(args);
  } else if (cmd == "ls") {
    cmdLs();
  } else if (cmd == "cat") {
    cmdCat(args);
  } else if (cmd == "echo") {
    cmdEcho(args);
  } else if (cmd == "find") {
    cmdFind(args);
  } else if (cmd == "cp") {
    cmdCp(args);
  } else if (cmd == "crc" || cmd == "[crc]") {
    cmdCrc(args);
  } else {
    Serial.println(F("Comanda necunoscuta. Comenzi valabile: cd, mkdir, rmdir, ls, cat, echo, find, cp, crc"));
  }
}

//comanda CD
void cmdCd(String path) {
  if (path == "" || path == ".") {
    //doar afiseaza directorul curent
    Serial.println(currentDir);
    return;
  }

  if (path == "..") {
    if (currentDir != "/") {
      int lastSlash = currentDir.lastIndexOf('/');
      if (lastSlash == 0) {
        currentDir = "/";
      } else {
        currentDir = currentDir.substring(0, lastSlash);
      }
    }
    Serial.println(currentDir);
    return;
  }

  String newPath = resolvePath(path);
  if (newPath == "/") {
    currentDir = "/";
    Serial.println(currentDir);
    return;
  }
  File dir = SD.open(newPath.c_str());
  if (!dir) {
    Serial.println(F("Eroare: Directorul nu exista."));
  } else if (!dir.isDirectory()) {
    Serial.println(F("Eroare: Calea specificata nu este un director."));
    dir.close();
  } else {
    currentDir = newPath;
    dir.close();
  }
  Serial.println(currentDir);
}

//comanda MKDIR
void cmdMkdir(String path) {
  if (path.length() == 0) {
    Serial.println(F("Eroare: Nume director lipsa."));
    return;
  }
  String fullPath = resolvePath(path);
  if (SD.mkdir(fullPath.c_str())) {
    Serial.println(F("Director creat cu succes."));
  } else {
    Serial.println(F("Eroare la crearea directorului."));
  }
}

//comanda RMDIR
void cmdRmdir(String path) {
  if (path.length() == 0) {
    Serial.println(F("Eroare: Nume director lipsa."));
    return;
  }
  String fullPath = resolvePath(path);
  if (SD.rmdir(fullPath.c_str())) {
    Serial.println(F("Director sters cu succes."));
  } else {
    Serial.println(F("Eroare la stergerea directorului (probabil nu e gol)."));
  }
}

//comanda LS
void cmdLs() {
  File dir = SD.open(currentDir.c_str());
  if (!dir || !dir.isDirectory()) {
    Serial.println(F("Eroare la deschiderea directorului curent."));
    if (dir) dir.close();
    return;
  }

  Serial.print(F("Continutul directorului "));
  Serial.print(currentDir);
  Serial.println(F(":"));
  File entry = dir.openNextFile();
  while (entry) {
    Serial.print(F("  "));
    if (entry.isDirectory()) {
      Serial.print(F("[DIR]  "));
      Serial.print(entry.name());
    } else {
      Serial.print(F("[FILE] "));
      Serial.print(entry.name());
      Serial.print(F("\t\tDimensiune: "));
      Serial.print(entry.size());
      Serial.print(F(" octeti"));
    }
    Serial.println();
    entry.close();
    entry = dir.openNextFile();
  }
  dir.close();
}

//citeste 4 octeti little endian
unsigned long g4(byte *b) {
  return (unsigned long)b[0] | ((unsigned long)b[1] << 8) |
         ((unsigned long)b[2] << 16) | ((unsigned long)b[3] << 24);
}

//luminozitatea pixelului x,y (y de sus in jos), 0..255
byte px(File &f, long x, long y) {
  byte b[3];
  long r = bd ? y : bh - 1 - y;
  f.seek(bo + (unsigned long)r * bs + (unsigned long)x * (bt / 8));
  if (bt == 8) {
    f.read(b, 1);
    f.seek(bp + (unsigned long)b[0] * 4);
  }
  f.read(b, 3);
  return (b[2] * 30 + b[1] * 59 + b[0] * 11) / 100;
}

//afiseaza un bmp deschis ca text art, inv = inversare luminozitate
void art(File &f, bool inv) {
  char ln[TW + 2];
  byte hd[54];
  f.seek(0);
  if (f.read(hd, 54) < 54 || hd[0] != 'B' || hd[1] != 'M') {
    Serial.println(F("Eroare: Fisier BMP invalid."));
    return;
  }
  bo = g4(hd + 10);
  bp = 14 + g4(hd + 14);
  bw = (long)g4(hd + 18);
  bh = (long)g4(hd + 22);
  bt = hd[28];
  if (hd[29] != 0 || g4(hd + 30) != 0 || (bt != 24 && bt != 8) || bw <= 0 || bh == 0) {
    Serial.println(F("Eroare: Format BMP nesuportat (doar 24/8 biti necomprimat)."));
    return;
  }
  bd = bh < 0;
  if (bd) bh = -bh;
  bs = ((unsigned long)bw * bt + 31) / 32 * 4;
  //nr coloane si randuri, caracterul e de 2 ori mai inalt decat lat
  long cl = bw < TW ? bw : TW;
  long rw = bh * cl / (2 * bw);
  if (rw < 1) rw = 1;
  if (rw > TH) rw = TH;
  for (long j = 0; j < rw; j++) {
    long y0 = j * bh / rw;
    long y1 = (j + 1) * bh / rw;
    for (long i = 0; i < cl; i++) {
      long x0 = i * bw / cl;
      long x1 = (i + 1) * bw / cl;
      unsigned int s = 0;
      //media a 4 puncte din celula
      for (byte a = 0; a < 2; a++)
        for (byte b = 0; b < 2; b++)
          s += px(f, x0 + (x1 - x0) * (2 * a + 1) / 4, y0 + (y1 - y0) * (2 * b + 1) / 4);
      byte g = s / 4;
      byte k = g * 10 / 256;
      if (inv) k = 9 - k;
      ln[i] = ramp[k];
    }
    ln[cl] = 0;
    Serial.println(ln);
  }
}

//comanda CAT
void cmdCat(String args) {
  if (args.length() == 0) {
    Serial.println(F("Eroare: Nume fisier lipsa."));
    return;
  }
  String name = args;
  bool inv = false;
  int sp = args.indexOf(' ');
  if (sp != -1) {
    name = args.substring(0, sp);
    String o = args.substring(sp + 1);
    o.trim();
    inv = (o == "1");
  }
  File file = SD.open(resolvePath(name).c_str(), FILE_READ);
  if (!file) {
    Serial.println(F("Eroare: Nu s-a putut deschide fisierul."));
    return;
  }
  if (file.isDirectory()) {
    Serial.println(F("Eroare: Calea specificata este un director."));
    file.close();
    return;
  }

  //verificare pt BMP
  String l = name;
  l.toLowerCase();
  if (l.endsWith(".bmp")) {
    art(file, inv);
  } else {
    uint8_t b[32];
    int k;
    while ((k = file.read(b, 32)) > 0) {
      Serial.write(b, k);
    }
    Serial.println();
  }
  file.close();
}

//comanda ECHO (ex: echo "text" > file.txt)
void cmdEcho(String args) {
  int firstQuote = args.indexOf('"');
  int lastQuote = args.lastIndexOf('"');
  int gtIndex = (lastQuote == -1) ? -1 : args.indexOf('>', lastQuote);

  if (firstQuote == -1 || lastQuote <= firstQuote || gtIndex == -1) {
    Serial.println(F("Sintaxa incorecta. Utilizare: echo \"text\" > fisier.txt"));
    return;
  }

  String textToWrite = args.substring(firstQuote + 1, lastQuote);
  String fileName = args.substring(gtIndex + 1);
  fileName.trim();
  if (fileName.length() == 0) {
    Serial.println(F("Eroare: Nume fisier lipsa."));
    return;
  }

  String fullPath = resolvePath(fileName);

  //daca fisierul exista, il stergem pentru a-l suprascrie
  if (SD.exists(fullPath.c_str())) {
    SD.remove(fullPath.c_str());
  }

  File file = SD.open(fullPath.c_str(), FILE_WRITE);
  if (file) {
    file.print(textToWrite);
    file.close();
    Serial.println(F("Fisier creat/actualizat cu succes."));
  } else {
    Serial.println(F("Eroare la scrierea in fisier."));
  }
}

//comanda FIND (cautare recursiva)
void recursiveFind(File &dir, String &targetName, String currentPathStr) {
  File entry = dir.openNextFile();
  while (entry) {
    String entryName = String(entry.name());
    //uneori entry.name() returneaza calea completa, depinde de biblioteca
    int lastSlash = entryName.lastIndexOf('/');
    if (lastSlash != -1) {
      entryName = entryName.substring(lastSlash + 1);
    }

    if (entry.isDirectory()) {
      recursiveFind(entry, targetName, jn(currentPathStr, entryName));
    } else {
      if (entryName.equalsIgnoreCase(targetName)) {
        Serial.println(jn(currentPathStr, entryName));
      }
    }
    entry.close();
    entry = dir.openNextFile();
  }
}

void cmdFind(String targetName) {
  if (targetName.length() == 0) {
    Serial.println(F("Eroare: Nume fisier cautat lipsa."));
    return;
  }
  File dir = SD.open(currentDir.c_str());
  if (!dir) {
    Serial.println(F("Eroare la deschiderea directorului curent."));
    return;
  }
  Serial.print(F("Rezultate cautare pentru '"));
  Serial.print(targetName);
  Serial.println(F("':"));
  recursiveFind(dir, targetName, currentDir);
  dir.close();
}

//comanda CP (cp cale1/fisier cale2)
void cmdCp(String args) {
  int spaceIdx = args.indexOf(' ');
  if (spaceIdx == -1) {
    Serial.println(F("Sintaxa incorecta. Utilizare: cp cale1/fisier cale2"));
    return;
  }

  String src = args.substring(0, spaceIdx);
  String dest = args.substring(spaceIdx + 1);
  src.trim();
  dest.trim();

  String srcPath = resolvePath(src);
  String destPath = resolvePath(dest);

  File srcFile = SD.open(srcPath.c_str(), FILE_READ);
  if (!srcFile) {
    Serial.println(F("Eroare: Fisierul sursa nu exista."));
    return;
  }
  if (srcFile.isDirectory()) {
    Serial.println(F("Eroare: Sursa este un director."));
    srcFile.close();
    return;
  }

  //daca destinatia este un director existent, adaugam numele fisierului sursa
  File destDir = SD.open(destPath.c_str());
  if (destDir) {
    if (destDir.isDirectory()) {
      int lastSlash = srcPath.lastIndexOf('/');
      String fileName = (lastSlash != -1) ? srcPath.substring(lastSlash + 1) : srcPath;
      destPath = jn(destPath, fileName);
    }
    destDir.close();
  }

  //fara asta, copierea unui fisier peste el insusi l-ar sterge
  if (srcPath.equalsIgnoreCase(destPath)) {
    Serial.println(F("Eroare: Sursa si destinatia sunt identice."));
    srcFile.close();
    return;
  }

  if (SD.exists(destPath.c_str())) {
    SD.remove(destPath.c_str());
  }

  File destFile = SD.open(destPath.c_str(), FILE_WRITE);
  if (!destFile) {
    Serial.println(F("Eroare: Nu s-a putut crea fisierul destinatie."));
    srcFile.close();
    return;
  }

  uint8_t b[32];
  int k;
  while ((k = srcFile.read(b, 32)) > 0) {
    destFile.write(b, k);
  }

  srcFile.close();
  destFile.close();
  Serial.println(F("Fisier copiat cu succes."));
}

//comanda CRC (polinom X^4 + X^2 + X + 1 = 10111 binar = 0x17)
void cmdCrc(String filename) {
  if (filename.length() == 0) {
    Serial.println(F("Eroare: Nume fisier lipsa pentru CRC."));
    return;
  }
  String fullPath = resolvePath(filename);
  File file = SD.open(fullPath.c_str(), FILE_READ);
  if (!file) {
    Serial.println(F("Eroare: Nu s-a putut deschide fisierul pentru CRC."));
    return;
  }
  if (file.isDirectory()) {
    Serial.println(F("Eroare: Calea specificata este un director."));
    file.close();
    return;
  }

  uint8_t crc = 0x00; //valoare initiala
  //se proceseaza bit cu bit; fara bitul de grad 4, polinomul ramane x^2 + x + 1 = 0b0111 = 0x07
  while (file.available()) {
    uint8_t b = file.read();
    for (int i = 0; i < 8; i++) {
      bool bit = ((b >> (7 - i)) & 1);
      bool c4 = (crc >> 3) & 1;
      crc = ((crc << 1) | bit) & 0x0F;
      if (c4) {
        crc ^= 0x07;
      }
    }
  }
  file.close();

  Serial.print(F("CRC-4 pentru "));
  Serial.print(filename);
  Serial.print(F(": 0x"));
  Serial.println(crc, HEX);
}
