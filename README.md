# ESP32 × OLED SSD1306 — Programmation bas niveau

Projet d'apprentissage : piloter un écran OLED Adafruit #938 (SSD1306, 128×64px)
depuis un ESP32 **sans aucune bibliothèque externe**, en I2C.

---

## Matériel requis

| Composant | Référence |
|-----------|-----------|
| Microcontrôleur | ESP32 (n'importe quelle carte) |
| Écran OLED | Adafruit #938 (SSD1306, 1.3", STEMMA QT) |
| Câble | 4 fils Dupont femelle-femelle |

---

## Câblage

```
ESP32          Adafruit #938
─────          ─────────────
3.3V    →      Vin  (rouge)
GND     →      GND  (noir)
GPIO 21 →      SDA  (bleu)
GPIO 22 →      SCL  (jaune)
```

> **Note STEMMA QT** : si tu utilises le câble STEMMA QT fourni,
> les couleurs correspondent directement. Sinon, utilise des fils Dupont
> sur les trous de soudure classiques de la carte.

---

## Structure du projet

```
esp32-oled-ssd1306/
├── platformio.ini      ← Configuration PlatformIO
└── src/
    ├── main.cpp        ← Exercices progressifs (UN seul actif à la fois)
    ├── i2c.h / .cpp    ← Couche bas niveau I2C
    └── ssd1306.h / .cpp← Registres et framebuffer SSD1306
```

### Rôle de chaque fichier

**`i2c.cpp`** — Gère le bus I2C hardware de l'ESP32.
Seule fonction utile : `i2c_write(adresse, données, longueur)`.
Tu peux l'ignorer au départ et faire confiance à son interface.

**`ssd1306.h`** — Contient TOUS les registres documentés du SSD1306.
Lis ce fichier comme un dictionnaire : chaque `#define` correspond
à une commande réelle du contrôleur, avec son explication.

**`ssd1306.cpp`** — Implémente les fonctions de dessin.
Le cœur du projet : initialisation, framebuffer, pixels, texte.

**`main.cpp`** — Tes exercices. Décommente-les un par un.

---

## Concepts clés à comprendre

### 1. Le bus I2C

Deux fils partagés entre plusieurs composants :
- **SDA** (Serial Data) : les données
- **SCL** (Serial Clock) : l'horloge

Chaque composant a une **adresse unique** (7 bits).
Le SSD1306 répond à l'adresse **0x3C** (ou 0x3D selon la config).

Une transaction I2C :
```
[START] [0x3C + W] [ACK] [octet1] [ACK] [octet2] [ACK] ... [STOP]
```

### 2. L'octet de contrôle

Après l'adresse, le SSD1306 attend **toujours** un octet de contrôle :
- `0x00` → ce qui suit est une **commande** (configure le contrôleur)
- `0x40` → ce qui suit sont des **données** (pixels à afficher)

### 3. La RAM interne du SSD1306

L'écran est organisé en **8 pages** de 8 pixels de haut :

```
Page 0 : lignes  0 à  7
Page 1 : lignes  8 à 15
Page 2 : lignes 16 à 23
...
Page 7 : lignes 56 à 63
```

Chaque page contient **128 octets** (un par colonne).
Chaque octet contrôle **8 pixels verticaux** (1 bit = 1 pixel).

```
Octet de la page 0, colonne 5 :
  bit 0 → pixel (5, 0)  ← le plus en haut
  bit 1 → pixel (5, 1)
  ...
  bit 7 → pixel (5, 7)  ← le plus en bas
```

### 4. Le framebuffer

Comme on ne peut pas **lire** la RAM du SSD1306 en I2C,
on garde une copie en RAM de l'ESP32 :

```
uint8_t framebuffer[8][128];  // 1024 octets
```

Flux de travail pour dessiner :
```
1. ssd1306_clear()         ← effacer le framebuffer (RAM ESP32)
2. ssd1306_set_pixel(x,y)  ← modifier des bits dans le framebuffer
3. ssd1306_display()       ← envoyer les 1024 octets à l'écran via I2C
```

---

## Exercices

| # | Fichier | Objectif |
|---|---------|----------|
| 1 | `#define EXERCICE_1` | Vérifier le câblage, allumer l'écran |
| 2 | `#define EXERCICE_2` | Afficher du texte, comprendre le framebuffer |
| 3 | `#define EXERCICE_3` | Manipuler des pixels individuels |
| 4 | `#define EXERCICE_4` | Tracer des lignes et rectangles |
| 5 | `#define EXERCICE_5` | Créer une animation |

**Pour changer d'exercice :** dans `main.cpp`, commente le `#define` actuel
et décommente le suivant, puis recompile (`Ctrl+Alt+U` dans PlatformIO).

---

## Débogage

Si l'écran ne s'allume pas à l'exercice 1 :

1. **Vérifier le câblage** : SDA sur GPIO 21, SCL sur GPIO 22, 3.3V sur Vin
2. **Ouvrir le moniteur série** (`Ctrl+Alt+S`) — tu dois voir les messages de setup
3. **Scanner l'I2C** : si le SSD1306 répond, tu dois trouver l'adresse 0x3C
4. **Vérifier la tension** : le SSD1306 attend 3.3V, pas 5V sur Vin

---

## Pour aller plus loin

Une fois les 5 exercices maîtrisés, tu peux explorer :
- Dessiner des cercles (algorithme de Bresenham)
- Afficher des images bitmap (convertir une image en tableau de bits)
- Utiliser le défilement matériel du SSD1306 (commandes 0x26/0x27)
- Mesurer la vitesse d'affichage et optimiser les transferts I2C
