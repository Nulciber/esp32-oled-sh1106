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
## dessiner un rectangle avec la fontion draw_rectangle
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
