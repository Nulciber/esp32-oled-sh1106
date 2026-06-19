## Dans ce fichier on trouve des exemples à lancer en remplaçant la fontion void_setup() de main.cpp

## Allumer les 4 pixles des 4 coins de l'écran
```cpp
void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
    set_pixel(0, 0, 1);
    set_pixel(127, 0, 1);
    set_pixel(0, 63, 1);
    set_pixel(127, 63, 1);
    display();
}
```
## Allumer une ligne pixel après pixel
```cpp
void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();

    for (uint8_t i; i<=127;i++ )
        set_pixel(i, 0, 1);
    display();
}
```
## Dessiner un cadre pixel après pixel (sans les fonctions draw_hline et draw_vline)
```cpp
## Exemple pour dessiner un cadre
```cpp
void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();

    for (uint8_t i = 0; i <= WIDTH - 1; i++)
        set_pixel(i, 0, 1);
    for (uint8_t j = 0; j <= HEIGHT - 1; j++)
        set_pixel(WIDTH - 1, j, 1);
    for (uint8_t k = 0; k <= WIDTH - 1; k++)
        set_pixel(k, HEIGHT - 1, 1);
    for (uint8_t l = 0; l <= HEIGHT - 1; l++)
        set_pixel(0, l, 1);

    display();
}
```
## Dessiner un rectangle avec les fonctions draw_hline et draw_vline
```cpp
void draw_hline(uint8_t x0, uint8_t x1, uint8_t y)
{
    for (uint8_t x = x0; x <= x1; x++)
        set_pixel(x, y, 1);
}

void draw_vline(uint8_t x, uint8_t y0, uint8_t y1)
{
    for (uint8_t y = y0; y <= y1; y++)
        set_pixel(x, y, 1);
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();

    draw_hline(0, WIDTH - 1, 0);          // haut
    draw_hline(0, WIDTH - 1, HEIGHT - 1); // bas
    draw_vline(0, 0, HEIGHT - 1);         // gauche
    draw_vline(WIDTH - 1, 0, HEIGHT - 1); // droite

    display();
}
```
## Dessiner un rectangle avec la fontion draw_rectangle
```cpp
void draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    draw_hline(x0, x1, y0);
    draw_vline(x1, y0, y1);
    draw_hline(x0, x1, y1);
    draw_vline(x0, y0, y1);
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();

    draw_rectangle(0, WIDTH - 1, 0, HEIGHT - 1);

    display();
}
```
## Dessiner un cercle
```cpp
void draw_circle(uint8_t cx, uint8_t cy, uint8_t r) {
    int x = 0;     // On fait débuter le cercle...
    int y = r;     //...par le point le plus haut
    int err = 3 - 2 * r; // Valeur initiale de l'erreur entre le cercle mathématique exact et le pixle choisi

    while (x <= y) {
        set_pixel(cx + x, cy + y, 1); // octant 1
        set_pixel(cx - x, cy + y, 1); // octant 2 (symétrie verticale)
        set_pixel(cx + x, cy - y, 1); // octant 3 (symétrie horizontale)
        set_pixel(cx - x, cy - y, 1); // octant 4 (symétrie centrale)
        set_pixel(cx + y, cy + x, 1); // octant 5 (symétrie diagonale)
        set_pixel(cx - y, cy + x, 1); // octant 6
        set_pixel(cx + y, cy - x, 1); // octant 7
        set_pixel(cx - y, cy - x, 1); // octant 8

        if (err < 0) {
            err += 4 * x + 6;
        } else {
            err += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
```

## Dessiner une ligne partant d'un point et allant à un autre (Bredenham)
```cpp
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = (dx > 0) ? 1 : -1;  // direction en X
    int sy = (dy > 0) ? 1 : -1;  // direction en Y
    dx = abs(dx);
    dy = abs(dy);
    int err = dx - dy;

    while (1) {
        set_pixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}
```

## Écrire un caractère
```cpp
void draw_char(uint8_t x, uint8_t y, uint8_t c)
{
    for (int i = 0; i < sizeof(police_standard) / sizeof(Caractere); i++)
    {
        if (police_standard[i].code == c)
        {
            draw_glyph(x, y, police_standard[i]);
            return;
        }
    }
    for (int i = 0; i < sizeof(police_accents) / sizeof(Caractere); i++)
    {
        if (police_accents[i].code == c)
        {
            draw_glyph(x, y, police_accents[i]);
            return;
        }
    }
}
```

## Écrire une chaîne de caractères
```cpp
// Affiche une chaîne de caractères à la position (x, y)
// str : pointeur sur le premier caractère de la chaîne (se termine par \0)
// Chaque caractère fait 8 pixels de large
void draw_string(uint8_t x, uint8_t y, const char *str)
{
    while (*str)
    {                          // tant qu'on n'est pas à la fin de la chaîne (\0)
        draw_char(x, y, *str); // on dessine le caractère courant
        x += 8;                // on avance de 8 pixels
        str++;                 // on passe au caractère suivant
    }
}
```

## Dessiner un cercle qui s'agrandit
```cpp
void setup()
{
    int a =5;
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();

    for (int i = 10; i < 70; i++)
    {
        draw_circle(63, 31, i);
        display();
        delay(a);
        clear();
    }
    for (int i = 10; i < 75; i++)
    {
        draw_circle(63, 31, i);
        display();
        delay(a);
        clear();
    }
}
```

## Faire défiler une phrase qui vient d'un fichier
Il faut ajouter au début de main.cpp
```cpp
#include "bateauIvre.h" / ou <nom_du_fichier>.h
```
```cpp
void loop()
{
    int longueur = strlen(phrase) * 8;
    static int i = 0;
    
    clear();
    draw_string(WIDTH - i, 28, phrase);
    if (i > longueur - WIDTH)
        draw_string(WIDTH - i + longueur, 28, phrase);
    display();
    delay(1);
    i++;
    if (i >= longueur) i = 0;
}
```

## Affichage de l'alphabet grec ancien
```cpp
void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
    // draw_char(0, 10, 0x03B1);  // α
    // draw_char(10, 10, 0x03B2); // β
    // draw_char(20, 10, 0x03C9); // Ω
    // display();
    // clear();
    draw_string(0, 0, "ΑΒΓΔΕΖΗΘ"); 
    draw_string(0, 10, "ΙΚΛΜΝΞΟΠ");
    draw_string(0, 20, "ΡΣΤΥΦΧΨΩ");
    draw_string(0, 30, "αβγδεζηθ");
    draw_string(0, 40, "ικλμνξοπ");
    draw_string(0, 50, "ρστυφχψω");
    display();
}

void loop()
{
}
```
## Make Love Not War
```cpp
void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    sh1106_init();
    clear();
    display();
    draw_circle(63, 31, 30);
    draw_line(63, 2, 63, 60);
    draw_line(40, 48, 63, 31);
    draw_line(87, 48, 63, 31);
    display();
}

void loop()
{
}
```