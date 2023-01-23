#ifndef HID_MOUSE
#define HID_MOUSE

typedef struct __attribute__((__packed__)) logitech_mouse
{
    uint8_t buttons;
    int16_t x;
    int16_t y;
    int8_t wheel;
} logitech_mouse_t;

typedef struct __attribute__((__packed__)) logitech_mouse1
{
    uint8_t buttons;
    int8_t x1;
    int8_t x2;
    int8_t y1;
    int8_t y2;
    int8_t wheel;
} logitech_mouse_t1;

typedef struct __attribute__((__packed__)) ms_mouse
{
    uint8_t buttons;
    int16_t x;
    int16_t y;
    int16_t wheel;
    int16_t pan;
} ms_mouse_t;

typedef struct __attribute__((__packed__)) joystick
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t rz;
    uint8_t brake;
    uint8_t accel;
    uint8_t hat;
    uint16_t buttons;
} joystick_t;

typedef struct __attribute__((__packed__)) trackball_mouse
{
    uint8_t buttons;
    int16_t x;
    int16_t y;
    int8_t wheel;
} trackball_mouse_t;

#endif