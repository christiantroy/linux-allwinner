/*
 * drivers/video/sun5i/lcd/lcd_bak/cpu_320x240_kgm281i0.c
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Danling <danliang@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**********************************************************************
 *
 *  cpu_320x240_kgm281i0.c
 *	
 **********************************************************************/
 
#include "lcd_panel_cfg.h"

//delete this line if you want to use the lcd para define in sys_config1.fex
#define LCD_PARA_USE_CONFIG

#ifdef LCD_PARA_USE_CONFIG
/*********************************************************
 *
 *  tcon parameters
 *	
 *********************************************************/
static void LCD_cfg_panel_info(__panel_para_t * info)
{ 
    memset(info,0,sizeof(__panel_para_t));

	//interface
    info->lcd_if            = 1;        	//0:hv; 		1:cpu/8080; 	2:reserved; 	3:lvds
    info->lcd_cpu_if        = 0;        	//0:18bit		4:16bit
    
    //timing                       	
    info->lcd_x             = 320;			//Hor Pixels
    info->lcd_y             = 240;			//Ver Pixels
    info->lcd_dclk_freq     = 6;       		//Pixel Data Cycle
    info->lcd_ht            = 320+30;     	//Hor Total Time
    info->lcd_hbp           = 20;      		//Hor Back Porch
    info->lcd_vt            = (240+30)*2;  	//Ver Total Time*2
    info->lcd_vbp           = 20;       	//Ver Back Porch
    info->lcd_hv_hspw       = 10;       	//Hor Sync Time
    info->lcd_hv_vspw       = 10;       	//Ver Sync Time
    info->lcd_io_cfg0       = 0x10000000;	//Clock Phase
    
	//color
    info->lcd_frm           = 1;        	//0: direct; 	1: rgb666 dither;	2:rgb656 dither
    info->lcd_gamma_correction_en = 0;

	//backlight
    info->lcd_pwm_not_used  = 0;
    info->lcd_pwm_ch        = 0;
    info->lcd_pwm_freq      = 12500;		//Hz
    info->lcd_pwm_pol       = 0;
}
#endif

/*********************************************************
 *
 *  lcd flow function
 *	CPU Panel:first TCON_open,than lcd_panel_init 
 *	
 *********************************************************/
static __s32 LCD_open_flow(__u32 sel)
{
	LCD_OPEN_FUNC(sel, LCD_power_on, 	50);   //open lcd power, than delay 50ms
	LCD_OPEN_FUNC(sel, TCON_open,		500);  //open lcd controller, than delay 500ms
	LCD_OPEN_FUNC(sel, LCD_panel_init,	50);   //lcd panel initial, than delay 50ms	
	LCD_OPEN_FUNC(sel, LCD_bl_open, 	0);    //open lcd backlight, than delay 0ms

	return 0;
}

static __s32 LCD_close_flow(__u32 sel)
{	
	LCD_CLOSE_FUNC(sel, LCD_bl_close, 	0);		//close lcd backlight, than delay 0ms
	LCD_CLOSE_FUNC(sel, LCD_panel_exit, 0);		//lcd panel exit, than delay 0ms	
	LCD_CLOSE_FUNC(sel, TCON_close, 	0);		//close lcd controller, than delay 0ms	
	LCD_CLOSE_FUNC(sel, LCD_power_off, 	1000);	//close lcd power, than delay 1000ms

	return 0;
}


/*********************************************************
 *
 *  lcd step function
 *	
 *********************************************************/
static void LCD_power_on(__u32 sel)
{
    LCD_POWER_EN(sel, 1);
}

static void LCD_power_off(__u32 sel)
{
    LCD_POWER_EN(sel, 0);
}

static void LCD_bl_open(__u32 sel)
{
    LCD_PWM_EN(sel, 1);
    LCD_BL_EN(sel, 1);
}

static void LCD_bl_close(__u32 sel)
{
    LCD_BL_EN(sel, 0);
    LCD_PWM_EN(sel, 0);
}


/*********************************************************
 *
 *  lcd panel initial
 *	cpu 8080 bus initial
 *
 *********************************************************/
#define kgm281i0_rs(sel,data)	LCD_GPIO_write(sel,0,data)

static void kgm281i0_write_gram_origin(__u32 sel)
{
	LCD_CPU_WR(sel,0x0020, 0);		// GRAM horizontal Address
	LCD_CPU_WR(sel,0x0021, 319);	// GRAM Vertical Address
	LCD_CPU_WR_INDEX(sel,0x22);		// Write Memery Start
}

static void kgm281i0_init(__u32 sel)
{    
	kgm281i0_rs(sel,1);
	LCD_delay_ms(50);
	kgm281i0_rs(sel,0);
	LCD_delay_ms(50);
	kgm281i0_rs(sel,1);	
		
	LCD_CPU_WR(sel,0x0000, 0x0001);
	LCD_CPU_WR(sel,0x0001, 0x0100);
	LCD_CPU_WR(sel,0x0002, 0x0400);
	LCD_CPU_WR(sel,0x0003, 0x1018);
	LCD_CPU_WR(sel,0x0004, 0x0000);		
	LCD_CPU_WR(sel,0x0008, 0x0202);		
	LCD_CPU_WR(sel,0x0009, 0x0000);		
	LCD_CPU_WR(sel,0x000A, 0x0000);		
	LCD_CPU_WR(sel,0x000C, 0x0000);		
	LCD_CPU_WR(sel,0x000D, 0x0000);		
	LCD_CPU_WR(sel,0x000F, 0x0000);		
	LCD_CPU_WR(sel,0x0010, 0x0000);		
	LCD_CPU_WR(sel,0x0011, 0x0007);		
	LCD_CPU_WR(sel,0x0012, 0x0000);		
	LCD_CPU_WR(sel,0x0013, 0x0000);		
	LCD_delay_ms(50);					
	LCD_CPU_WR(sel,0x0010, 0x17B0);		
	LCD_CPU_WR(sel,0x0011, 0x0001);		
	LCD_delay_ms(50);					
	LCD_CPU_WR(sel,0x0012, 0x013C);		
	LCD_delay_ms(50);					
	LCD_CPU_WR(sel,0x0013, 0x1300);		
	LCD_CPU_WR(sel,0x0029, 0x0012);		
	LCD_delay_ms(50);					
	LCD_CPU_WR(sel,0x0020, 0x0000);		
	LCD_CPU_WR(sel,0x0021, 0x0000);		
	LCD_CPU_WR(sel,0x002B, 0x0020);		
	LCD_CPU_WR(sel,0x0030, 0x0000);		
	LCD_CPU_WR(sel,0x0031, 0x0306);		
	LCD_CPU_WR(sel,0x0032, 0x0200);		
	LCD_CPU_WR(sel,0x0035, 0x0107);		
	LCD_CPU_WR(sel,0x0036, 0x0404);		
	LCD_CPU_WR(sel,0x0037, 0x0606);		
	LCD_CPU_WR(sel,0x0038, 0x0105);		
	LCD_CPU_WR(sel,0x0039, 0x0707);		
	LCD_CPU_WR(sel,0x003C, 0x0600);		
	LCD_CPU_WR(sel,0x003D, 0x0807);		
	LCD_CPU_WR(sel,0x0050, 0x0000);		
	LCD_CPU_WR(sel,0x0051, 0x00EF);		
	LCD_CPU_WR(sel,0x0052, 0x0000);		
	LCD_CPU_WR(sel,0x0053, 0x013F);		
	LCD_CPU_WR(sel,0x0060, 0x2700);		
	LCD_CPU_WR(sel,0x0061, 0x0001);		
	LCD_CPU_WR(sel,0x006A, 0x0000);		
	LCD_CPU_WR(sel,0x0080, 0x0000);	        
	LCD_CPU_WR(sel,0x0081, 0x0000);	     
	LCD_CPU_WR(sel,0x0082, 0x0000);	   
	LCD_CPU_WR(sel,0x0083, 0x0000);	        
	LCD_CPU_WR(sel,0x0084, 0x0000);	     
	LCD_CPU_WR(sel,0x0085, 0x0000);	                                  
	LCD_CPU_WR(sel,0x0090, 0x0013);	        
	LCD_CPU_WR(sel,0x0092, 0x0000);	
	LCD_CPU_WR(sel,0x0093, 0x0003);	
	LCD_CPU_WR(sel,0x0095, 0x0110);	        
	LCD_CPU_WR(sel,0x0097, 0x0000);	
	LCD_CPU_WR(sel,0x0098, 0x0000);		                                
	LCD_CPU_WR(sel,0x0007, 0x0001);	
	LCD_delay_ms(50);               
	LCD_CPU_WR(sel,0x0007, 0x0021);	
	LCD_CPU_WR(sel,0x0007, 0x0023);	
	LCD_delay_ms(50);               
	LCD_CPU_WR(sel,0x0007, 0x0173);	        
}

static void Lcd_cpuisr_proc(void)				//irq func
{
	kgm281i0_write_gram_origin(0);
}

static void LCD_panel_init(__u32 sel)
{
	kgm281i0_init(sel);							//initial lcd panel
	kgm281i0_write_gram_origin(sel);			//set gram origin
	LCD_CPU_register_irq(sel,Lcd_cpuisr_proc);	//resgister cpu irq func
	LCD_CPU_AUTO_FLUSH(sel,1);					//start sent gram data
}

static void LCD_panel_exit(__u32 sel)
{

}
/*********************************************************
 *
 *  user define function
 *
 *********************************************************/
static __s32 LCD_user_defined_func(__u32 sel, __u32 para1, __u32 para2, __u32 para3)
{
    return 0;
}

/*********************************************************
 *
 *  do not modify
 *
 *********************************************************/
void LCD_get_panel_funs_0(__lcd_panel_fun_t * fun)
{
#ifdef LCD_PARA_USE_CONFIG
    fun->cfg_panel_info = LCD_cfg_panel_info;
#endif
    fun->cfg_open_flow = LCD_open_flow;
    fun->cfg_close_flow = LCD_close_flow;
    fun->lcd_user_defined_func = LCD_user_defined_func;
}
