本实验将实现如下功能：开机的时候先检测触摸屏是否校准过，如果没有，则校准。如果校准过了，则开始触摸屏画图，然后将我们的坐标数据上传到电脑（假定USB已经配置成功了，DS1亮），这样就可以用触摸屏来控制电脑的鼠标了。我们用按键KEY0模拟鼠标右键，用按键KEY2模拟鼠标左键，用按键WK_UP和KEY1模拟鼠标滚轮的上下滚动。同样我们也是用DS0来指示程序正在运行。

注意：
1,请确认P13的跳线帽PA11连接在D-,PA12连接在D+上。
2,请屏蔽stm32f10x_it.h里面的如下代码：
void NMI_Handler(void)        __attribute__ ((alias("NMIException")));
void HardFault_Handler(void)  __attribute__ ((alias("HardFaultException")));
void MemManage_Handler(void)  __attribute__ ((alias("MemManageException")));
void BusFault_Handler(void)   __attribute__ ((alias("BusFaultException")));
void UsageFault_Handler(void) __attribute__ ((alias("UsageFaultException")));
void DebugMon_Handler(void)   __attribute__ ((alias("DebugMonitor")));
void SVC_Handler(void)        __attribute__ ((alias("SVCHandler")));
void PendSV_Handler(void)     __attribute__ ((alias("PendSVC")));
void SysTick_Handler(void)    __attribute__ ((alias("SysTickHandler")));
将以上代码注释掉！！！