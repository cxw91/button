#ifndef BUTTON_H
#define BUTTON_H

#include "include.h"

#define BTN_NAME_MAX 32  //名字最大为32字节
#define ENABLE_DOUBALE 0 //使能双击
/* 按键消抖时间40ms, 建议调用周期为20ms
 只有连续检测到40ms状态不变才认为有效，包括弹起和按下两种事件
*/

/* 是否支持单击&双击同时存在触发，如果选择开启宏定义的话，单双击都回调，只不过单击会延迟响应，
   因为必须判断单击之后是否触发了双击否则，延迟时间是双击间隔时间 BUTTON_DOUBLE_TIME。
   而如果不开启这个宏定义，建议工程中只存在单击/双击中的一个，否则，在双击响应的时候会触发一次单击，
   因为双击必须是有一次按下并且释放之后才产生的 */
#define SINGLE_AND_DOUBLE_TRIGGER 0
//多按键使能 注意：组合按键情况下禁止长按连续触发
#define MULTI_BUTTON_EN 1
/* 是否支持长按释放才触发，如果打开这个宏定义，那么长按释放之后才触发单次长按，
   否则在长按指定时间就一直触发长按，触发周期由 BUTTON_LONG_CYCLE 决定 */
#if MULTI_BUTTON_EN
#define LONG_FREE_TRIGGER 1
#else
#define LONG_FREE_TRIGGER 0 //用户自定义
#endif
/* 长按未弹起触发一次使能  **LONG_FREE_TRIGGER不能同时使能*/
#if LONG_FREE_TRIGGER
#define LONG_TRIGGER_ONCE 0
#else
#define LONG_TRIGGER_ONCE 0 //用户自定义
#endif
#define BUTTON_DEBOUNCE_TIME 2 //消抖时间      (n-1)*调用周期
#define BUTTON_LONG_CYCLE 50   //长按触发周期时间  (n-1)*调用周期
#define BUTTON_DOUBLE_TIME 15  //双击间隔时间  (n-1)*调用周期  建议在200-600ms
#define BUTTON_LONG_TIME 50    /* 持续n秒((n-1)*调用周期 ms)，认为长按事件 */
#define EVENT_TABLE_CNT 20     /*事件表大小*/
#define EVENT_NAME_LEN 30
#define MUlti_CNT 10

#define TRIGGER_CB(event)            \
  if (btn->CallBack_Function[event]) \
  btn->CallBack_Function[event]((Button_t *)btn)

typedef void (*Button_CallBack)(void *);     /* 按键触发回调函数，需要用户实现 */
typedef void (*Button_Multi_CallBack)(void); /* 按键触发回调函数，需要用户实现 */
typedef enum
{
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_SINGLE_FREE,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event, /* 触发回调的事件 */
  NONE_TRIGGER
} Button_Event;

typedef enum
{
  BUTTON_MULTI_LONG = 0,
  number_of_multi_event, /* 触发回调的事件 */
} Multi_Event;
typedef enum
{
  noMultiEvent = 0,             /* 没有组合按键事件注册 */
  hasMUltiEvent = !noMultiEvent /* 有组合按键事件注册*/
} Multi_Env;
/*
	每个按键对应1个全局的结构体变量。
	其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct _multi
{
  char name[BTN_NAME_MAX];                                     //组合按键的名字
  Button_Multi_CallBack CallBack_Multi[number_of_multi_event]; /*组合按键对应的事件*/
} multi_t;

typedef struct button
{
  /* 下面是一个函数指针，指向判断按键手否按下的函数 */
  uint8_t (*Read_Button_Level)(void); /* 读取按键电平函数，需要用户实现 */

  char Name[BTN_NAME_MAX];

  uint8_t Button_State : 4;         /* 按键当前状态（按下还是弹起） */
  uint8_t Button_Last_State : 4;    /* 上一次的按键状态，用于判断双击 */
  uint8_t Button_Trigger_Level : 2; /* 按键触发电平 */
  uint8_t Button_Curr_Level : 2;    /* 按键当前电平 */
  uint8_t Button_Multi : 2;

  uint8_t Button_Trigger_Event; /* 按键触发事件，单击，双击，长按等 */

  Button_CallBack CallBack_Function[number_of_event]; /*用户添加回调的事件*/

  uint8_t multi_info_cnt;        /*与当前按键有组合关系的按键数量*/
  multi_t multi_info[MUlti_CNT]; /*与当前按键有组合关系的按键信息*/
  uint8_t Event_Table_Cnt;
  Button_Event Trigger_Event_Table[EVENT_TABLE_CNT]; /*记录用户触发的事件*/

  uint8_t Button_Cycle; /* 连续按键周期 */

  uint8_t Timer_Count;   /* 计时 */
  uint8_t Debounce_Time; /* 消抖时间 */

  uint8_t Long_Time; /* 按键按下持续时间 */

  struct button *Next;

} Button_t;

/* 供外部调用的函数声明 */

void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t (*read_btn_level)(void),
                   uint8_t btn_trigger_level);

void Button_Attach(Button_t *btn, Button_Event btn_event, Button_CallBack btn_callback);
void Button_Multi_Attach(Button_t *btn1, Button_t *btn2, Multi_Event btn_event, Button_Multi_CallBack btn_callback);
void Button_Cycle_Process(Button_t *btn);

void Button_Process(void);

void Button_Delete(Button_t *btn);

void Search_Button(void);

void Get_Button_EventInfo(Button_t *btn);
uint8_t Get_Button_Event(Button_t *btn);
uint8_t Get_Button_State(Button_t *btn);
void Button_Process_CallBack(void *btn);

#endif
