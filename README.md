# Button Driver
include single,double，long，long_release ，multi_long press

## 使用方法
1. 创建按键句柄
```
Button_t Button1;
Button_t Button2; 
```
2. 创建按键，初始化按键信息，包括按键名字、按键电平检测函数接口、按键触发电平。
```
  Button_Create("Button1",				//按键名字
                &Button1, 				//按键句柄
                Read_Button1_Level, 	//按键电平检测函数接口
                BTN_TRIGGER);		   	//触发电平
                
                ......
```
3. 按键触发事件与事件回调函数链接映射，当按键事件被触发的时候，自动跳转回调函数中处理业务逻辑。
```
  Button_Attach(&Button1,BUTTON_DOWM,Btn2_Dowm_CallBack);		  //按键单击
  Button_Attach(&Button1,BUTTON_DOUBLE,Btn2_Double_CallBack);	//双击
  Button_Attach(&Button1,BUTTON_LONG,Btn2_Long_CallBack);		  //长按
  Button_Multi_Attach(&Button1, &Button2, BUTTON_MULTI_LONG, Multi_long_CallBack); //组合长按			
				.......
```
4. 周期调用回调按键处理函数即可，建议调用周期20-50ms。
```
Button_Process();     //需要周期调用按键处理函数
```

---
需要用户实现的 **2** 个函数：

- 按键电平检测接口：
```
uint8_t Read_Button1_Level(void)
{
  return GPIO_ReadInputDataBit(BTN1_GPIO_PORT,BTN1_GPIO_PIN);
}

uint8_t Read_Button2_Level(void)
{
  return GPIO_ReadInputDataBit(BTN2_GPIO_PORT,BTN2_GPIO_PIN);
}

```

- 按键逻辑处理
```
void Btn1_Dowm_CallBack(void *btn)
{
  PRINT_INFO("Button1 单击!");
}

void Btn1_Double_CallBack(void *btn)
{
  PRINT_INFO("Button1 双击!");
}

void Btn1_Long_CallBack(void *btn)
{
  PRINT_INFO("Button1 长按!");
  
  Button_Delete(&Button2);
  PRINT_INFO("删除Button1");
  Search_Button();
}
void Multi_long_CallBack()
{
    PRINT_INFO("Multi_long_CallBack!\n");
}
```

