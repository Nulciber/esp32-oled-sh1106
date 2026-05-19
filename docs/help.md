## I2C
C'est un bus de communication qui permet à l'ESP32 de parler à l'écran avec seulement 2 fils :

SDA (GPIO 8) — les données
SCL (GPIO 9) — l'horloge

## Envoi des commandes d'initialisation
```cpp
void setup()
Wire.begin(8, 9);   // SDA=8 SCL=9
```

| Commande | Action                                           | Paramètre de |
| -------- | ------------------------------------------------ | ------------ |
| 0xAE     | Éteindre l'écran                                 | —            |
| 0xD5     | Configurer l'horloge interne                     | —            |
| 0x80     | Valeur par défaut selon datasheet                | 0xD5         |
| 0xA8     | Configurer le multiplexage                       | —            |
| 0x3F     | 64 lignes (63 en comptant depuis 0)              | 0xA8         |
| 0xD3     | Configurer le décalage vertical                  | —            |
| 0x00     | Pas de décalage                                  | 0xD3         |
| 0x40     | Ligne de départ = 0                              | —            |
| 0xA1     | Colonne 0 à gauche (orientation normale)         | —            |
| 0xC8     | Scanner de haut en bas (orientation normale)     | —            |
| 0xDA     | Configurer les broches COM                       | —            |
| 0x12     | Configuration pour un écran 128x64               | 0xDA         |
| 0x81     | Configurer le contraste                          | —            |
| 0xFF     | Valeur maximale                                  | 0x81         |
| 0xD9     | Configurer la pré-charge                         | —            |
| 0x22     | Valeur recommandée par le datasheet              | 0xD9         |
| 0xDB     | Configurer le niveau VCOMH                       | —            |
| 0x35     | Valeur recommandée pour le SH1106                | 0xDB         |
| 0xA4     | Afficher le contenu de la RAM                    | —            |
| 0xA6     | Mode normal (1 = pixel allumé, 0 = pixel éteint) | —            |
| 0xAF     | Allumer l'écran — toujours en dernier            | —            |

## Sélectionner une page de l'écran (1 page = 128 pixels de large x 8 pixels de haut)
```cpp
send_command(0xB0);  // sélectionner la page 0
```
## Sélectionner la colonne de départ
```cpp
send_command(0x02);  // bits de droite de la colonne (on commence à 2 à cause du décalage du SH1106)
send_command(0x10);  // bits de gauche de la colonne
```
Le SH1106 a une RAM de 132 colonnes alors que l'écran physique n'affiche que 128 colonnes. Les 4 colonnes supplémentaires sont cachées — 2 à gauche et 2 à droite.
Donc la colonne 0 de l'écran physique correspond à la colonne 2 de la RAM du SH1106. C'est pour ça qu'on commence à 2 et pas à 0.

Exemple pour envoyer des données à la page 0:
```cpp
send_command(0xB0);  // sélectionner la page 0
send_command(0x02);  // colonne de départ : bits de droite (2 à cause du décalage SH1106)
send_command(0x10);  // colonne de départ : bits de gauche
```
