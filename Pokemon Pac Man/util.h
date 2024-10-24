#ifndef H_UTIL
#define H_UTIL

typedef enum {EVENT_TIMER, EVENT_KEY} EVENT_TYPE;

typedef struct {
    EVENT_TYPE type;
} TimerEvent;

typedef struct {
    EVENT_TYPE type;
} DisplayCloseEvent;

typedef struct {
    EVENT_TYPE type;
    int key;
} KeyDownEvent;

typedef union {
    EVENT_TYPE type;
    TimerEvent timerEvent;
    DisplayCloseEvent displayCloseEvent;
    KeyDownEvent keyDownEvent;
} Event;


#endif