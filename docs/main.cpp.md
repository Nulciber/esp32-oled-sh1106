## Bibliothèques

```cpp
#include <Arduino.h> // On appelle la bibliothèque Arduino
#include <Wire.h>    // On appelle la bibliothèque qui inclut les programmes de communication avec le hardware I2C
```
## Constante I2C_ADDR

```cpp
#define I2C_ADDR   0x3D // Adresse I2C du SH1106
```
## Code pour trouver l'adresse I2C
```cpp
#include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);
}

void loop() {
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t err = Wire.endTransmission();
        if (err == 0) {
            Serial.print("Appareil trouve a 0x");
            Serial.println(addr, HEX);
        }
    }
    delay(3000);
}
```


## Autres constantes

#define SDA_PIN    8    // Port SDA
#define SCL_PIN    9    // Port SCL
#define WIDTH      128  // Largeur de l'écran en pixels 
#define HEIGHT     64   // Hauteur de l'écran en pixels
#define PAGES      8    // Nombre de pages. Chaque page = une bande de 8 lignes sur toute la largeur de l'écran

Rappel: en I2C, le port SDA permet la circultion sur les deux sens (MOSI et MISO)
Il n'y a pas de CS car le composant est choisi par son adresse, dans notre cas 0x3D. Donc on peut dialoguer avec 127 composants avec un seul fil. Ce protocole est cependant plus lent que SPI.

## Variables globales
Variable qui sera utilisée pour envoyer l'ensemble des pixels à l'écran.
C'est une copie en RAM de l'ESP32 de ce qui doit être affiché.
On dessine dedans, puis on l'envoie à l'écran avec display().
Taille : 8 pages × 128 colonnes = 1024 octets.
```cpp
uint8_t framebuffer[PAGES][WIDTH];
// Pour correspondre aux besoins du SH1106, on donne d'abord la ligne (PAGES) puis la colonne (WIDTH)
```
## fonction sendCommand()
Reçoit en paramètre une variable de type uint8_t (octet) et lui donne le nom de cmd pour l'utilisation dans son code interne

```cpp
void send_command(uint8_t cmd) {
    Wire.beginTransmission(I2C_ADDR); // On indique à l'écran qu'on va lui transmettre de l'information
    Wire.write(0x00);    // 0x00 = Ce qui suit est une commande
    Wire.write(cmd);     // La commande elle-même, qui a été passée en paramètre
    Wire.endTransmission(); // La transmission est terminée
}
```
## fonction sh1106_init()
Initialisation de l'écran. Le détail des valeurs est donné dans le fichier help.md
```cpp
void sh1106_init() {
    send_command(0xAE); //Eteindre l'écran
    send_command(0xD5); send_command(0x80); // Configuration de l'horloge interne; 0xD5=avec la valeur du datasheet
    send_command(0xA8); send_command(0x3F); // Configuration du multiplexage;0xA8=64 lignes (63 en comptant depuis 0)
    send_command(0xD3); send_command(0x00); // Configuration du décalage vertical;0xD3=pas de décalage
    send_command(0x40); // Ligne de départ. 0x40 porte en elle-même que la ligne de départ vaut 0
    send_command(0x8D); send_command(0x14); // Configure la charge pump ON; 0x14=ON. Sans cette commande l'écran ne s'allume pas!
    send_command(0xA1); // Orientation normale: on scanne de gauche à droite
    send_command(0xC8); // Orientation normale: on scanne de haut en bas
    send_command(0xDA); send_command(0x12); // Configuration des broches COM;0xDA=Configuration pour un écran 128x64 
    send_command(0x81); send_command(0xFF); // Configurer le contraste; 0xFF=contraste maximal
    send_command(0xD9); send_command(0x22); // Configuration de la pré-charge;0x22=valeur recommandée par le datasheet
    send_command(0xDB); send_command(0x35); // Configuration du niveau VCOMH;0xDB=valeur recommandée par le datasheet
    send_command(0xA4); // Afficher le contenu de la RAM
    send_command(0xA6); // Mode normal (1 = pixel allumé, 0 = pixel éteint)
    send_command(0xAF); //Allumer l'écran — toujours en dernier

}
```
## Fonction clear()
Efface le framebuffer en mettant tous les pixels à 0 (écran noir).

`memset` est une fonction C qui remplit un bloc de mémoire avec une valeur donnée. Elle prend 3 paramètres :
1. **`framebuffer`** — l'adresse du tableau qu'on remplit, dans notre cas le framebuffer de 1024 octets
2. **`0x00`** — la valeur à mettre partout (0 = pixel éteint)
3. **`sizeof(framebuffer)`** — le nombre d'octets à remplir (1024 octets dans notre cas)

En une seule ligne elle met tous les 1024 octets du framebuffer à zéro — ce qui correspond à un écran entièrement noir.

C'est l'équivalent de faire :
```cpp
for (int i = 0; i < 1024; i++) {
    framebuffer[i] = 0x00;
}
```
mais en beaucoup plus rapide.

```cpp
void clear() {
    memset(framebuffer, 0x00, sizeof(framebuffer)); // Remplit les 1024 octets du framebuffer avec 0x00
}
```
## Fonction display()
Envoi du framebuffer à l'écran via I2C, page par page (rappel, il y a 8 pages de 128 octets, numérotées de haut en bas de 0 à 7)
Séquence pour chaque page:
On dit au SH1106 quelle page on va remplir
On envoie les 64 premiers octets de cette page
On envoie les 64 octets suivants
Pourquoi deux transmissions de 64 octets?
Parce que le buffer I2C de l'ESP32-S3 est limité à 128 octets par transmission. Or on doit envoyer 129 octets en une fois (1 octet de contrôle 0x40 + 128 octets de données) — c'est 1 octet de trop.
On coupe donc en deux transmissions de 65 octets chacune (1 octet 0x40 + 64 octets de données).
```cpp
void display() {
    for (uint8_t page = 0; page < PAGES; page++) {
        // On dit au SH1106 quelle page on va remplir
        Wire.beginTransmission(I2C_ADDR); // On indique à l'écran qu'on va transmettre une information
        Wire.write(0x00); // Ce qui suit est une commande (0x00= commande, 0x40=données)
        Wire.write(0xB0 + page); // sélectionner la page (0xB0=page 0, 0xB1=page 1, etc.). 0xB0 est défini dans le datasheet.
        Wire.write(0x00); // Partie basse de l'adresse de colonne. Préfixe 0x0 + valeur 0000 = colonne 0
        Wire.write(0x10); // Partie haute de l'adresse de colonne. Préfixe 0x1 + valeur 0000 = colonne 0
```
        Le préfixe c'est ce que le SH1106 impose dans son datasheet pour distinguer les deux commandes :
        0x0_ = commande "partie basse de l'adresse de colonne" — le _ est la valeur (4 bits)
        0x1_ = commande "partie haute de l'adresse de colonne" — le _ est la valeur (4 bits)
        Et les 0000 viennent de la valeur de la colonne qu'on choisit. Pour la colonne 0 :
        partie basse = 0x00 = préfixe 0x0 + valeur 0000
        partie haute = 0x10 = préfixe 0x1 + valeur 0000
        Pour la colonne 18 (= 0b00010010) :
        partie basse = 0x02 = préfixe 0x0 + valeur 0010 (les 4 bits de droite de 18)
        partie haute = 0x11 = préfixe 0x1 + valeur 0001 (les 4 bits de gauche de 18)

```cpp

        Wire.endTransmission();

        // On envoie les 64 premiers octets de cette page
        Wire.beginTransmission(I2C_ADDR); // On indique à l'écran qu'on va lui transmettre de l'information
        Wire.write(0x40);                 // 0x40=On envoie des données
        Wire.write(framebuffer[page], 64);// On envoie les 64 premiers octets de la page 
        Wire.endTransmission();           // La transmission est terminée

        // On envoie les 64 octets suivants
        Wire.beginTransmission(I2C_ADDR); // On indique à l'écran qu'on va lui transmettre de l'information
        Wire.write(0x40);                 // 0x40=On envoie des données
        Wire.write(framebuffer[page] + 64, 64);// On envoie les 64 octets de la page 
        Wire.endTransmission();           // La transmission est terminée

    }
}
```
## Fonction set_pixel()
```cpp
void set_pixel(uint8_t x, uint8_t y, uint8_t on) {
    if (x >= WIDTH || y >= HEIGHT) return; // si on est sorti de l'écran à droite (>=128) ou en bas (>=64), on s'arrête
    uint8_t page = y / 8; // définition d'une variable page qui vaut y / 8. On cherche dans quelle page on se trouve. Rappel: en C, la division d'un entier renvoie le nombre avant la virgule
    uint8_t bit  = y % 8; // définition d'une variable bit qui vaut le reste de y divisé par 8. Dans la page, on cherche le bit, qui correspond au reste de la division de y par 8. 
    if (on) // Si on veut allumer un pixel (ON= Allume le pixel)
        framebuffer[page][x] |=  (1 << bit); //Rappel: framebuffer[ligne][colonne]
        // Grâce à la fonction clear(), framebuffer[page][x] = 00000000
        // 0b00000000 |= 1 << 4 = 0b00010000
        // Donc on allume le pixel en position 4 depuis LSB de l'octet de la page 2 pour la ligne, et en position x pour la colonne
        // framebuffer[page][x] |=  (1 << bit) permet de passer le bit concerné à 1 sans modifier les autres bits de l'octet
    else
        framebuffer[page][x] &= ~(1 << bit); // Sinon on éteint le bit
        // framebuffer[page][x] &= ~(1 << bit)  permet de passer le bit concerné à 0 sans modifier les autres bits de l'octet
        // 0b00010000 &= ~(1 << 4)
        // ~(1 << 4) = ~0b00010000 = 0b11101111
        // 0b00010000 & 0b11101111 = 0b00000000
        // Donc on éteint le pixel sans modifier les autres bits
}
```
## Fonction setup()
```cpp
void setup() {
    Serial.begin(115200);        // Console interne pour monitoring
    Wire.begin(SDA_PIN, SCL_PIN);// Lancement du protocole I2C
    Wire.setClock(400000);       // Fréquence de l'horloge I2C = 400 kHz (Fast mode)
    sh1106_init();               // Exécute la fonction sh1106_init()
    clear();                     // Efface le framebuffer en mettant tous les pixels à 0 (écran noir).
    display();                   // Envoi du framebuffer à l'écran via I2C
    set_pixel(0, 0, 1);          // Allume le pixel en position (0,0)
    display();                   // Envoi du framebuffer à l'écran via I2C
}
```

## Fonction loop()
```cpp
void loop() {} // Rien — tout est fait dans setup()
```

