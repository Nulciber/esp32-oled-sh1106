# ESP32-S3 × OLED SH1106 — Programmation bas niveau

Projet d'apprentissage : piloter un écran OLED Adafruit #938 (SH1106, 128×64px)
depuis un ESP32-S3 **sans bibliothèques externes**, en I2C.

---

## Matériel requis

| Composant | Référence |
|-----------|-----------|
| Microcontrôleur | ESP32-S3-WROOM-1 (DevKitC-1) |
| Écran OLED | Adafruit #938 (SH1106, 1.3", STEMMA QT) |

---

## Câblage

```
ESP32-S3       Adafruit #938
────────       ─────────────
3.3V    →      VIN
GND     →      GND
GPIO 8  →      DATA (SDA)
GPIO 9  →      CLK  (SCL)
```

> **Note** : l'adresse I2C de l'écran est **0x3D** (découverte par scan I2C).
> Le contrôleur est un **SH1106** (et non SSD1306 comme indiqué par Adafruit).

---

## Structure du projet

```
esp32-oled-sh1106/
├── platformio.ini        ← Configuration PlatformIO pour ESP32-S3
└── src/
    ├── main.cpp          ← Programme principal (setup et loop)
    ├── sh1106.h          ← Déclarations des fonctions et constantes
    ├── sh1106.cpp        ← Code des fonctions d'affichage
    ├── caractere.h       ← Définition du type Caractere
    ├── police_standard.h ← Police ASCII standard (8×8 pixels)
    ├── police_accents.h  ← Police des caractères accentués
    ├── txt_aveMaria.h    ← Texte : Ave Maria
    └── txt_eneide.h      ← Texte : Énéide
```

> **Convention** : les fichiers de texte à afficher portent le préfixe `txt_`.
> La variable dans le fichier a le même nom que le fichier (ex. `txt_aveMaria`).

---

## Fonctions disponibles

| Fonction | Description |
|----------|-------------|
| `sh1106_init()` | Initialise le contrôleur SH1106 |
| `clear()` | Efface le framebuffer (écran noir) |
| `display()` | Envoie le framebuffer à l'écran via I2C |
| `set_pixel(x, y, on)` | Allume ou éteint un pixel |
| `draw_hline(x0, x1, y)` | Dessine une ligne horizontale |
| `draw_vline(x, y0, y1)` | Dessine une ligne verticale |
| `draw_rectangle(x0, x1, y0, y1)` | Dessine un rectangle vide |
| `draw_line(x0, y0, x1, y1)` | Dessine une ligne oblique (Bresenham) |
| `draw_circle(cx, cy, r)` | Dessine un cercle (Bresenham) |
| `draw_glyph(x, y, car)` | Dessine un glyphe (fonction interne) |
| `draw_char(x, y, c)` | Affiche un caractère ASCII |
| `draw_string(x, y, str)` | Affiche une chaîne de caractères |

---

## Ajouter un texte à afficher

1. Créer un fichier `txt_monTexte.h` dans `src/` :

```cpp
#ifndef TXT_MONTEXTE_H
#define TXT_MONTEXTE_H

const char *txt_monTexte =
    "Première ligne du texte  "
    "Deuxième ligne du texte  ";

#endif
```

2. Dans `main.cpp`, inclure le fichier et définir `TEXTEQUIDEFILE` :

```cpp
#include "txt_monTexte.h"
#define TEXTEQUIDEFILE txt_monTexte
```

---

## Caractères accentués

Les accents sont définis dans `police_accents.h` avec des codes spéciaux :

| Constante | Caractère | Code |
|-----------|-----------|------|
| `CAR_E_AIGU` | é | 0x80 |
| `CAR_E_GRAVE` | è | 0x81 |
| `CAR_E_TREMA` | ë | 0x82 |
| `CAR_E_MACRO` | ē | 0x83 |
| `CAR_E_CIRC` | ê | 0x84 |

Dans une chaîne, utiliser `\x80` pour é, `\x81` pour è, etc.

---

## Concepts clés

### Le bus I2C
Deux fils partagés entre plusieurs composants :
- **DATA** (SDA) : les données
- **CLK** (SCL) : l'horloge

### L'octet de contrôle
- `0x00` → ce qui suit est une **commande**
- `0x40` → ce qui suit sont des **données** (pixels)

### La RAM du SH1106
L'écran est organisé en **8 pages** de 8 pixels de haut × 128 pixels de large.
Le SH1106 a une RAM de 132 colonnes — les pixels visibles commencent à la colonne 0.

### Le framebuffer
Copie en RAM de l'ESP32 de ce qui doit être affiché (1024 octets).
Flux de travail :
```
1. clear()         ← effacer le framebuffer
2. set_pixel(x, y) ← dessiner dans le framebuffer
3. display()       ← envoyer à l'écran
```

---

## Unicode et caractères spéciaux

L'encodage **UTF-8** représente les caractères sur 1, 2 ou 3 octets selon la **valeur numérique** du code Unicode — pas selon la nature du caractère :

- **1 octet** : U+0000 à U+007F — ASCII standard — ex. 'A' = 0x41
- **2 octets** : U+0080 à U+07FF — accents, grec, cyrillique... — ex. 'α' = U+03B1, 'é' = U+00E9
- **3 octets** : U+0800 à U+FFFF — caractères asiatiques, symboles, zone privée — ex. '中', '€', U+E000

C'est donc uniquement la valeur du code Unicode qui détermine le nombre d'octets, pas le type de caractère. La zone privée Unicode comporte trois zones :
- **U+E000 à U+F8FF** — zone privée de base (6400 caractères)
- **U+F0000 à U+FFFFF** — zone privée supplémentaire A
- **U+100000 à U+10FFFF** — zone privée supplémentaire B

Pour notre projet, on utilise uniquement la zone de base (U+E000 à U+F8FF), encodée sur 3 octets car ses valeurs dépassent U+07FF.

### Exemple avec α (alpha grec, code Unicode U+03B1)

En UTF-8, U+03B1 s'encode sur 2 octets :
- Octet 1 : `0xCE` = `1100 1110` → masque `110xxxxx`, les 5 bits de poids fort du code
- Octet 2 : `0xB1` = `1011 0001` → masque `10xxxxxx`, les 6 bits de poids faible du code

La fonction `draw_string()` reconstruit le code Unicode :

```cpp
code = ((0xCE & 0x1F) << 6) | (0xB1 & 0x3F)
     = (0x0E << 6) | 0x31
     = 0x380 | 0x31
     = 0x3B1  ← c'est bien U+03B1 !
```

Pour afficher α, il faut l'ajouter dans `police_accents.h` avec le code `0x3B1` et son bitmap 8×8.

### Exemple avec un glyphe de la zone privée (U+E000)

U+E000 s'encode sur 3 octets : `\xEE\x80\x80`

La fonction `draw_string()` reconstruit le code Unicode :

```cpp
code = ((0xEE & 0x0F) << 12) | ((0x80 & 0x3F) << 6) | (0x80 & 0x3F)
     = (0x0E << 12) | (0x00 << 6) | 0x00
     = 0xE000  ← c'est bien U+E000 !
```

Le glyphe U+E000 représente une diagonale descendant de gauche à droite, puis une ligne verticale :

```
█ . . . . . . .
. █ . . . . . .
. . █ . . . . .
. . . █ . . . .
. . . . █ . . .
. . . . . █ . .
. . . . . █ . .
. . . . . █ . .
```

### Fichier exemple avec 3 glyphes : 'a', α et U+E000

```cpp
#ifndef POLICE_EXEMPLE_H
#define POLICE_EXEMPLE_H
#include "caractere.h"

const Caractere police_exemple[] = {
    {0x61,   {0x00, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x3E, 0x00}}, // a  (ASCII, 1 octet)
    {0x3B1,  {0x00, 0x1C, 0x22, 0x3E, 0x22, 0x22, 0x1C, 0x00}}, // α  (U+03B1, 2 octets : \xCE\xB1)
    {0xE000, {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x04, 0x04}}, // ↘  (U+E000, zone privée, 3 octets : \xEE\x80\x80)
};

#endif
```

### Afficher les 3 glyphes dans main.cpp

Si `α` (0x3B1) et le glyphe U+E000 sont dans `police_accents.h`, pas besoin de fichier supplémentaire. `draw_char()` les trouve automatiquement.

**Option 1 : via `draw_char()` avec le code Unicode directement**

On passe le code Unicode en hexadécimal — pas de `\x` nécessaire :

```cpp
#include <Arduino.h>
#include <Wire.h>
#include "sh1106.h"

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();

    draw_char(10, 10, 0x61);    // a     — code Unicode direct, pas de \x
    draw_char(20, 10, 0x3B1);   // α     — code Unicode direct, pas de \x
    draw_char(30, 10, 0xE000);  // ↘     — code Unicode direct, pas de \x
    display();
}

void loop() {}
```

**Option 2 : via `draw_string()` avec les octets UTF-8**

Dans une chaîne, chaque caractère est encodé en octets UTF-8 avec `\x` :

```cpp
// a  = ASCII 1 octet  → \x61
// α  = UTF-8 2 octets → \xCE\xB1
// ↘  = UTF-8 3 octets → \xEE\x80\x80

draw_string(10, 10, "\x61\xCE\xB1\xEE\x80\x80");
display();
```

---

## Scanner l'adresse I2C

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

---

## Pour aller plus loin
- Ajouter des boutons pour changer de texte
- Afficher des images bitmap
- Créer des animations
- Optimiser le rafraîchissement (ne rafraîchir que les pages modifiées)

---

| <img src="pictures/1.jpg" width="400"> | <img src="pictures/2.jpg" width="400"> |
|---|---|