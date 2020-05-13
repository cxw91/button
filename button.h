#ifndef BUTTON_H
#define BUTTON_H

#include "include.h"

#define BTN_NAME_MAX 32  //�������Ϊ32�ֽ�
#define ENABLE_DOUBALE 0 //ʹ��˫��
/* ��������ʱ��40ms, �����������Ϊ20ms
 ֻ��������⵽40ms״̬�������Ϊ��Ч����������Ͱ��������¼�
*/

/* �Ƿ�֧�ֵ���&˫��ͬʱ���ڴ��������ѡ�����궨��Ļ�����˫�����ص���ֻ�����������ӳ���Ӧ��
   ��Ϊ�����жϵ���֮���Ƿ񴥷���˫�������ӳ�ʱ����˫�����ʱ�� BUTTON_DOUBLE_TIME��
   ���������������궨�壬���鹤����ֻ���ڵ���/˫���е�һ����������˫����Ӧ��ʱ��ᴥ��һ�ε�����
   ��Ϊ˫����������һ�ΰ��²����ͷ�֮��Ų����� */
#define SINGLE_AND_DOUBLE_TRIGGER 0
//�ఴ��ʹ�� ע�⣺��ϰ�������½�ֹ������������
#define MULTI_BUTTON_EN 1
/* �Ƿ�֧�ֳ����ͷŲŴ��������������궨�壬��ô�����ͷ�֮��Ŵ������γ�����
   �����ڳ���ָ��ʱ���һֱ�������������������� BUTTON_LONG_CYCLE ���� */
#if MULTI_BUTTON_EN
#define LONG_FREE_TRIGGER 1
#else
#define LONG_FREE_TRIGGER 0 //�û��Զ���
#endif
/* ����δ���𴥷�һ��ʹ��  **LONG_FREE_TRIGGER����ͬʱʹ��*/
#if LONG_FREE_TRIGGER
#define LONG_TRIGGER_ONCE 0
#else
#define LONG_TRIGGER_ONCE 0 //�û��Զ���
#endif
#define BUTTON_DEBOUNCE_TIME 2 //����ʱ��      (n-1)*��������
#define BUTTON_LONG_CYCLE 50   //������������ʱ��  (n-1)*��������
#define BUTTON_DOUBLE_TIME 15  //˫�����ʱ��  (n-1)*��������  ������200-600ms
#define BUTTON_LONG_TIME 50    /* ����n��((n-1)*�������� ms)����Ϊ�����¼� */
#define EVENT_TABLE_CNT 20     /*�¼����С*/
#define EVENT_NAME_LEN 30
#define MUlti_CNT 10

#define TRIGGER_CB(event)            \
  if (btn->CallBack_Function[event]) \
  btn->CallBack_Function[event]((Button_t *)btn)

typedef void (*Button_CallBack)(void *);     /* ���������ص���������Ҫ�û�ʵ�� */
typedef void (*Button_Multi_CallBack)(void); /* ���������ص���������Ҫ�û�ʵ�� */
typedef enum
{
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_SINGLE_FREE,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event, /* �����ص����¼� */
  NONE_TRIGGER
} Button_Event;

typedef enum
{
  BUTTON_MULTI_LONG = 0,
  number_of_multi_event, /* �����ص����¼� */
} Multi_Event;
typedef enum
{
  noMultiEvent = 0,             /* û����ϰ����¼�ע�� */
  hasMUltiEvent = !noMultiEvent /* ����ϰ����¼�ע��*/
} Multi_Env;
/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
	���Ա������ʵ���˲��Ͷ��ְ���״̬�������
*/
typedef struct _multi
{
  char name[BTN_NAME_MAX];                                     //��ϰ���������
  Button_Multi_CallBack CallBack_Multi[number_of_multi_event]; /*��ϰ�����Ӧ���¼�*/
} multi_t;

typedef struct button
{
  /* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
  uint8_t (*Read_Button_Level)(void); /* ��ȡ������ƽ��������Ҫ�û�ʵ�� */

  char Name[BTN_NAME_MAX];

  uint8_t Button_State : 4;         /* ������ǰ״̬�����»��ǵ��� */
  uint8_t Button_Last_State : 4;    /* ��һ�εİ���״̬�������ж�˫�� */
  uint8_t Button_Trigger_Level : 2; /* ����������ƽ */
  uint8_t Button_Curr_Level : 2;    /* ������ǰ��ƽ */
  uint8_t Button_Multi : 2;

  uint8_t Button_Trigger_Event; /* ���������¼���������˫���������� */

  Button_CallBack CallBack_Function[number_of_event]; /*�û���ӻص����¼�*/

  uint8_t multi_info_cnt;        /*�뵱ǰ��������Ϲ�ϵ�İ�������*/
  multi_t multi_info[MUlti_CNT]; /*�뵱ǰ��������Ϲ�ϵ�İ�����Ϣ*/
  uint8_t Event_Table_Cnt;
  Button_Event Trigger_Event_Table[EVENT_TABLE_CNT]; /*��¼�û��������¼�*/

  uint8_t Button_Cycle; /* ������������ */

  uint8_t Timer_Count;   /* ��ʱ */
  uint8_t Debounce_Time; /* ����ʱ�� */

  uint8_t Long_Time; /* �������³���ʱ�� */

  struct button *Next;

} Button_t;

/* ���ⲿ���õĺ������� */

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
