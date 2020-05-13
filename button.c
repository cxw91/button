/*****************************************************************************
File name: button.c
Description: 
Author:cxw   --参考:github:https://github.com/jiejieTop
Version: V1.0
Date: 2020-5-13
History: 
*****************************************************************************/
#include "button.h"
#include "string.h"

/*******************************************************************
 *                          变量声明                               
 *******************************************************************/
static struct button *Head_Button = NULL;
/*******************************************************************
 *                         函数声明     
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n);
static void Print_Btn_Info(Button_t *btn);
static void Add_Button(Button_t *btn);

/**************************************************
 * brief:               创建按键
 * name:                按键名
 * btn：                按键结构体指针
 * read_btn_level：     读取按键电平函数指针
 * btn_trigger_level    按键按下有效电平
 * return：             无
 * ************************************************/
void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t (*read_btn_level)(void),
                   uint8_t btn_trigger_level)
{
    if (btn == NULL)
    {
        PRINT_ERR("struct button is null!");
        ASSERT(ASSERT_ERR);
    }

    memset(btn, 0, sizeof(struct button));

    StrnCopy(btn->Name, name, BTN_NAME_MAX);

    btn->Button_State = NONE_TRIGGER;
    btn->Button_Last_State = NONE_TRIGGER;
    btn->Button_Trigger_Event = NONE_TRIGGER;
    btn->Read_Button_Level = read_btn_level;
    btn->Button_Trigger_Level = btn_trigger_level;
    btn->Button_Curr_Level = btn->Read_Button_Level();
    btn->Debounce_Time = 0;
    btn->Event_Table_Cnt = 0;
    btn->Button_Multi = (btn->Button_Multi == hasMUltiEvent) ? hasMUltiEvent : noMultiEvent;
    PRINT_DEBUG("button create success!");

    Add_Button(btn);

    //Print_Btn_Info(btn);
}
/**************************************************
 * brief:       将按键事件添加到按钮结构体中
 * btn：        按键结构体指针
 * btn_event    按键触发事件
 * return：             无
 * ************************************************/
void buttont_envtable_add(Button_t *btn, Button_Event btn_event)
{
    if (btn == NULL)
    {
        PRINT_ERR("struct button is null!");
    }
    btn->Trigger_Event_Table[btn->Event_Table_Cnt] = btn_event;
    btn->Event_Table_Cnt += 1;
}
/**************************************************
 * brief:               非组合按键绑定事件
 * btn_event：          按键事件
 * btn_callback：       对应事件回调
 * return：             无
 * ************************************************/
void Button_Attach(Button_t *btn, Button_Event btn_event, Button_CallBack btn_callback)
{
    if (btn == NULL)
    {
        PRINT_ERR("struct button is null!");
    }

    if (BUTTON_ALL_RIGGER == btn_event)
    {
        for (uint8_t i = 0; i < number_of_event - 1; i++)
            btn->CallBack_Function[i] = btn_callback;
    }
    else
    {
        btn->CallBack_Function[btn_event] = btn_callback;
    }
}
/**************************************************
 * brief:               组合按键绑定事件
 * btn1:                组合按键1
 * btn2：               组合按键2
 * btn_event：          按键事件
 * btn_callback：       对应事件回调
 * return：             无
 * ************************************************/
void Button_Multi_Attach(Button_t *btn1, Button_t *btn2, Multi_Event btn_event, Button_Multi_CallBack btn_callback)
{
#if MULTI_BUTTON_EN
    btn1->Button_Multi = hasMUltiEvent;
    btn2->Button_Multi = hasMUltiEvent;
    btn1->multi_info->CallBack_Multi[btn_event] = btn_callback;
    memcpy(btn1->multi_info->name, btn2->Name, BTN_NAME_MAX);
    PRINT_INFO("btn1->multi_info->name=%s\n", btn1->multi_info->name);
    btn1->multi_info_cnt += 1;
    btn2->multi_info->CallBack_Multi[btn_event] = btn_callback;
    memcpy(btn2->multi_info->name, btn1->Name, BTN_NAME_MAX);
    PRINT_INFO("btn2->multi_info->name=%s\n", btn2->multi_info->name);
    btn2->multi_info_cnt += 1;

#else
    PRINT_ERR("#define MULTI_BUTTON_EN is not enable\n");
#endif
}
/**************************************************
 * brief:               删除按键
 * btn：                按键结构体指针
 * return：             无
 * ************************************************/
void Button_Delete(Button_t *btn)
{
    struct button *pass_btn;
    if (btn->Button_Multi == hasMUltiEvent)
    {
        for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
        {
            if (pass_btn->Button_Multi == hasMUltiEvent)
            {
                for (int i = 0; i < pass_btn->multi_info_cnt; i++)
                {
                    if (memcmp(pass_btn->multi_info->name, btn->Name, BTN_NAME_MAX) == 0)
                    {
                        for (int j = 0; j < pass_btn->multi_info_cnt; j++)
                        {
                            if (j >= i)
                            {
                                pass_btn->multi_info[j] = pass_btn->multi_info[j + 1];
                            }
                        }
                        pass_btn->multi_info_cnt -= 1;
                    }
                }
            }
        }
    }
    struct button **curr;
    for (curr = &Head_Button; *curr;)
    {
        struct button *entry = *curr;
        if (entry == btn)
        {
            *curr = entry->Next;
        }
        else
        {
            curr = &entry->Next;
        }
    }
}
/**************************************************
 * brief:               获取按键所有绑定事件
 * btn：                按键结构体指针
 * return：             无
 * ************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
    for (uint8_t i = 0; i < number_of_event - 1; i++)
    {
        if (btn->CallBack_Function[i] != 0)
        {
            PRINT_INFO("Button_Event:%d", i);
        }
    }
}
/**************************************************
 * brief:               获取按键事件
 * btn：                按键结构体指针
 * return：             事件序号
 * ************************************************/
uint8_t Get_Button_Event(Button_t *btn)
{
    return (uint8_t)(btn->Button_Trigger_Event);
}
/**************************************************
 * brief:               获取按键状态
 * btn：                按键结构体指针
 * return：             返回按键状态
 * ************************************************/
uint8_t Get_Button_State(Button_t *btn)
{
    return (uint8_t)(btn->Button_State);
}
/**************************************************
 * brief:               按键周期调用处理函数
 * btn：                按键结构体指针
 * return               无
 * ************************************************/
void Button_Cycle_Process(Button_t *btn)
{
    uint8_t current_level = (uint8_t)btn->Read_Button_Level();

    if ((current_level != btn->Button_Curr_Level) && (++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME))
    {
        btn->Button_Curr_Level = current_level;
        btn->Debounce_Time = 0;
        if ((btn->Button_State == NONE_TRIGGER) || (btn->Button_State == BUTTON_DOUBLE))
        {
            btn->Button_State = BUTTON_DOWM;
            TRIGGER_CB(BUTTON_DOWM);
            //buttont_envtable_add(btn, BUTTON_DOWM);
        }
        else if (btn->Button_State == BUTTON_DOWM)
        {
            btn->Button_State = BUTTON_UP;
            TRIGGER_CB(BUTTON_UP);
            //buttont_envtable_add(btn, BUTTON_UP);
        }
    }

    switch (btn->Button_State)
    {
    case BUTTON_DOWM:
    {
        if (btn->Button_Curr_Level == btn->Button_Trigger_Level)
        {
            btn->Button_Trigger_Event = BUTTON_DOWM;
            if (++(btn->Long_Time) >= BUTTON_LONG_TIME)
            {
#if LONG_FREE_TRIGGER
                btn->Button_Trigger_Event = BUTTON_LONG;
#else
                if ((btn->Long_Time) == BUTTON_LONG_TIME)
                {
                    TRIGGER_CB(BUTTON_LONG);
                    //buttont_envtable_add(btn, BUTTON_LONG);
                }
#if (LONG_TRIGGER_ONCE == 0)
                if (++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE)
                {
                    btn->Button_Cycle = 0;
                    btn->Button_Trigger_Event = BUTTON_LONG;
                    TRIGGER_CB(BUTTON_LONG);
                    // buttont_envtable_add(btn, BUTTON_LONG);
                }
#endif
#endif
                if (btn->Long_Time == 0xFF)
                {
                    btn->Long_Time = BUTTON_LONG_TIME;
                }
            }
        }
        break;
    }

    case BUTTON_UP:
    {
        if (btn->Button_Trigger_Event == BUTTON_DOWM)
        {
            if ((btn->Timer_Count <= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State == BUTTON_DOUBLE))
            {
                if ((btn->Long_Time) < BUTTON_LONG_TIME)
                {
                    btn->Button_Trigger_Event = BUTTON_DOUBLE;
                    TRIGGER_CB(BUTTON_DOUBLE);
                    //buttont_envtable_add(btn, BUTTON_DOUBLE);
                }
                btn->Button_State = NONE_TRIGGER;
                btn->Button_Last_State = NONE_TRIGGER;
            }
            else
            {
#if (SINGLE_AND_DOUBLE_TRIGGER == 0)
                if ((btn->Long_Time) < BUTTON_LONG_TIME)
                {
                    TRIGGER_CB(BUTTON_SINGLE_FREE);
                    //buttont_envtable_add(btn, BUTTON_SINGLE_FREE);
                }
#endif
#if ENABLE_DOUBALE
                if ((btn->Long_Time) < BUTTON_LONG_TIME)
                {

                    btn->Button_State = BUTTON_DOUBLE;
                    btn->Button_Last_State = BUTTON_DOUBLE;
                }
                else
                {
                    btn->Button_State = NONE_TRIGGER;
                    btn->Button_Last_State = NONE_TRIGGER;
                }
#else
                btn->Button_State = NONE_TRIGGER;
                btn->Button_Last_State = NONE_TRIGGER;
#endif
                btn->Timer_Count = 0;
                btn->Long_Time = 0;
                btn->Button_Cycle = 0;
            }
        }

        else if (btn->Button_Trigger_Event == BUTTON_LONG)
        {
#if LONG_FREE_TRIGGER
            // TRIGGER_CB(BUTTON_LONG_FREE);
            buttont_envtable_add(btn, BUTTON_LONG_FREE);
#endif
            btn->Long_Time = 0;
            btn->Button_Cycle = 0;
            btn->Button_State = NONE_TRIGGER;
            btn->Button_Last_State = BUTTON_LONG;
        }
        break;
    }

    case BUTTON_DOUBLE:
    {
        btn->Timer_Count++;
        if (btn->Timer_Count >= BUTTON_DOUBLE_TIME)
        {
            btn->Button_State = NONE_TRIGGER;
            btn->Button_Last_State = NONE_TRIGGER;
        }
#if SINGLE_AND_DOUBLE_TRIGGER

        if ((btn->Timer_Count >= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State != BUTTON_DOWM))
        {
            btn->Timer_Count = 0;
            if ((btn->Long_Time) < BUTTON_LONG_TIME)
            {
                // TRIGGER_CB(BUTTON_SINGLE_FREE);
                buttont_envtable_add(btn, BUTTON_SINGLE_FREE);
            }
            btn->Button_State = NONE_TRIGGER;
            btn->Button_Last_State = BUTTON_DOWM;
        }
#endif
        break;
    }
    default:
        break;
    }
}
/**************************************************
 * brief:          按键处理函数供周期调用(20~50ms)
 * ************************************************/
void Button_Process(void)
{
    struct button *pass_btn;
    uint8_t find_multi_flag = 0;
    uint8_t process_multi_flag = 0;
    uint8_t is_all_idle = 1;
    char tempname[BTN_NAME_MAX] = {0};
    for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
    {
        Button_Cycle_Process(pass_btn);
        if (pass_btn->Button_State != NONE_TRIGGER)
        {
            is_all_idle = 0;
        }
    }
    //读取完所有注册按键状态 处理事件
    if (is_all_idle == 1)
    {
        pass_btn = Head_Button;
        while (pass_btn != NULL)
        {
            if ((find_multi_flag == 0) && (pass_btn->Button_Multi == hasMUltiEvent) && (pass_btn->Event_Table_Cnt > 0) && (pass_btn->Button_State == NONE_TRIGGER))
            {
                find_multi_flag = 1;
                memcpy(tempname, pass_btn->multi_info->name, BTN_NAME_MAX);
                pass_btn = pass_btn->Next;
                if (pass_btn == NULL)
                {
                    find_multi_flag = 0;
                }
                continue;
            }
            if ((find_multi_flag == 1) && (pass_btn->Button_Multi == hasMUltiEvent) && (pass_btn->Event_Table_Cnt > 0) && (pass_btn->Button_State == NONE_TRIGGER))
            {
                for (int j = 0; j < pass_btn->multi_info_cnt; j++)
                {
                    if (memcmp(pass_btn->Name, tempname, BTN_NAME_MAX) == 0)
                    {
                        pass_btn->multi_info->CallBack_Multi[BUTTON_MULTI_LONG]();
                        process_multi_flag = 1;
                        break;
                    }
                }
            }
            else
            {
                find_multi_flag = 0;
            }
            pass_btn = pass_btn->Next;
        }
        for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
        {
            if ((process_multi_flag == 0) && (find_multi_flag == 0) && (pass_btn->Event_Table_Cnt > 0) && (pass_btn->Button_State == NONE_TRIGGER)) //没有组合按键
            {
                for (int i = 0; i < pass_btn->Event_Table_Cnt; i++)
                {
                    if (pass_btn->CallBack_Function[pass_btn->Trigger_Event_Table[i]])
                    {
                        pass_btn->CallBack_Function[pass_btn->Trigger_Event_Table[i]]((Button_t *)pass_btn);
                    }
                }
            }
            //执行完事件 清空相关数据
            pass_btn->Event_Table_Cnt = 0;
            find_multi_flag = 0;
        }
    }
}
/**************************************************
 * brief:               查找并打印所有已创建按键
 * return:              无
 * ************************************************/
void Search_Button(void)
{
    struct button *pass_btn;
    for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
    {
        PRINT_INFO("button node have %s\n", pass_btn->Name);
    }
}
/**************************************************
 * brief:               按键处理回调
 * btn：                按键结构体指针
 * return:              无
 * ************************************************/
void Button_Process_CallBack(void *btn)
{
    uint8_t btn_event = Get_Button_Event(btn);

    switch (btn_event)
    {
    case BUTTON_DOWM:
    {
        PRINT_INFO("BUTTON_DOWM\r\n");
        break;
    }

    case BUTTON_UP:
    {
        PRINT_INFO("BUTTON_UP\r\n");
        break;
    }
    case BUTTON_SINGLE_FREE:
    {
        PRINT_INFO("BUTTON_SINGLE_FREE\r\n");
        break;
    }
    case BUTTON_DOUBLE:
    {
        PRINT_INFO("BUTTON_DOUBLE\r\n");
        break;
    }

    case BUTTON_LONG:
    {
        PRINT_INFO("BUTTON_LONG\r\n");
        break;
    }

    case BUTTON_LONG_FREE:
    {
        PRINT_INFO("BUTTON_LONG_FREE\r\n");
        break;
    }
    }
}
/**************************************************
 * brief:               指定长度字符串拷贝
 * dst:                 拷贝目的地地址
 * src:                 拷贝源数据地址
 * n:                   拷贝字符串长度
 * return:              返回拷贝后数据地址
 * ************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;
        do
        {
            if ((*d++ = *s++) == 0)
            {
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }
    return (dst);
}
/**************************************************
 * brief:               打印按键信息
 * btn：                按键结构体指针
 * return:              无
 * ************************************************/
static void Print_Btn_Info(Button_t *btn)
{

    PRINT_INFO("button struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Curr_Level:%d \n\
              btn->multi_info_cnt:%d \n\
              btn->>multi_info->name:%s \n\
              btn->Event_Table_Cnt:%d \n\
              ",
               btn->Name,
               btn->Button_State,
               btn->Button_Trigger_Event,
               btn->Button_Trigger_Level,
               btn->Button_Curr_Level,
               btn->multi_info_cnt,
               btn->multi_info->name,
               btn->Event_Table_Cnt);
    // Search_Button();
}
/**************************************************
 * brief:               增加按键
 * btn：                按键结构体指针
 * return：             无
 * ************************************************/
static void Add_Button(Button_t *btn)
{
    btn->Next = Head_Button;
    Head_Button = btn;
}
