#include "system.h"

uint8_t sys_Inited = HAL_ERROR;

FATFS FatFs;

uint8_t sys_init(void)
{
	slog("");
	slog("SDMPlayer by Alexander Lutsai");
	slog("System initing");
	if(gui_init() == SYS_ERROR)
	{	
		elog("DISPLAY NOT INITIALIZED! HALT!");
		SYS_ERROR_HANDLER();
	}
	
	gui_showMessage("SDMPlayer initing\nSSD1306 - OK\n...");
	gui_draw();
	
	if(keyboard_init() == SYS_ERROR)
	{
		gui_showMessage("SDMPlayer initing\nSSD1306 - OK\nKeyboard - FAIL");
		gui_draw();
		elog("KEYBOARD NOT INITIALIZED! HALT!");
		SYS_ERROR_HANDLER();
	}
	keyboard_setHandler(&sys_input_handler);
	gui_showMessage("SDMPlayer initing...\nSSD1306 - OK\nKeyboard - OK\n...");
	gui_draw();
	
	uint8_t res = 0;
	res = f_mount(&FatFs, "", 1);
	if(res != FR_OK)
	{
		elog("FatFS error %d", res);
	}
	slog("FatFS - OK");
	uint16_t sz = hw_sdcard_capacity();
	slog("Size: %iMb", sz);
	char msg[100] = {0};
	sprintf(msg, "SDMPlayer initing\nSSD1306 - OK\nKeyboard - OK\nFatFS - OK\nSD size: %iMb", sz);
	gui_showMessage(msg);
	gui_draw();
	HAL_Delay(500);
	
	
	sys_start_threads();
	sys_Inited = SYS_OK;
	slog("System inited");
	
	app_set(APP_TEST_ID);
	
	return 1;
}

uint8_t sys_isInited(void)
{
	return sys_Inited;
}

uint8_t sys_start_threads(void)
{
	osThreadDef(sys_draw_THREAD, sys_draw, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(sys_draw_THREAD), NULL);

	osThreadDef(sys_update_THREAD, sys_update, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(sys_update_THREAD), NULL);
	
	osThreadDef(sys_keyboard_THREAD, sys_thread_keyboard, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(sys_keyboard_THREAD), NULL);
	
	return SYS_OK;
}

void sys_draw(void const * argument)
{
  for(;;)
	{
		gui_setFont(&DEFAULT_FONT);
		app_draw();
		gui_setFont(&DEFAULT_FONT);
		sys_draw_gui();
		gui_setFont(&DEFAULT_FONT);
		gui_draw();
		osDelay(10);
	}
}
uint32_t co = 0;
char bu[30];
void sys_update(void const * argument)
{
	for(;;)
	{
		app_update();
		osDelay(100);
	}
}

void sys_input_handler(int8_t key, uint32_t state)
{
		app_input_handler(key, state);
}


void sys_thread_keyboard(void const * argument)
{
	for(;;)
	{
		keyboard_update();
		osDelay(30);
	}
}
void sys_thread_player(void const * argument)
{
}
void sys_thread_hw(void const * argument)
{
}