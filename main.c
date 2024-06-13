#include <stdio.h>
#include "gpio.h"
#include "i2c_lcd.h"
#include "systick.h"
#include "stm32f10x.h"
#include "adxl345.h"
#include "timer.h"
#include "math.h"


#define NORMAL 0
#define FALL 1
#define ACTIVE 1
#define STOPPED 0

uint8_t is_LED_RED_on = 1,is_LED_GREEN_on = 1, trangthai = 1,nga = 0;

void init_gpio(void);

void SystemClock_Config(void) {
   
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;  // 2 wait states for 72 MHz
 
    RCC->CFGR |= RCC_CFGR_PLLSRC;       // HSE oscillator clock selected as PLL input clock
    RCC->CFGR |= RCC_CFGR_PLLMULL9;     // PLL input clock x 9
 
    RCC->CR |= RCC_CR_PLLON;
		
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;    // HCLK = SYSCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;   // PCLK1 = HCLK/2
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   // PCLK2 = HCLK
   
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    
    SystemCoreClockUpdate();
}

void init_gpio(void) {
		gpio_init(PortA,3, 0, 1);  // Kh?i t?o chân GPIOA-5 là d?u ra t?ng quát
		gpio_write(PortA, 3, 0);    // Ð?t chân GPIOA-5 ? m?c cao
		gpio_init(PortA,4, 0, 1);  // Kh?i t?o chân GPIOA-5 là d?u ra t?ng quát
		gpio_write(PortA, 4, 0);    // Ð?t chân GPIOA-5 ? m?c cao
		gpio_init(PortA,0, 2, 0);
		GPIOA->ODR |= GPIO_ODR_ODR0;    // Pull-up
		gpio_init(PortA,1, 2, 0); 
		GPIOA->ODR |= GPIO_ODR_ODR1;    // Pull-up
		EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1;
		EXTI->FTSR |= EXTI_FTSR_TR0; // Ng?t theo c?ch âm trên chân 0
		EXTI->FTSR |= EXTI_FTSR_TR1; // Ng?t theo c?ch âm trên chân 1
		NVIC_EnableIRQ(EXTI0_IRQn);                     // B?t ng?t EXTI0
		NVIC_EnableIRQ(EXTI1_IRQn);                     // B?t ng?t EXTI1
		NVIC_SetPriority(EXTI0_IRQn, 0);
		NVIC_SetPriority(EXTI1_IRQn, 0);
}
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) { // Ki?m tra n?u ng?t du?c kích ho?t b?i chân 0
        for (int x = 500000; x > 0; x--);
				while (gpio_read(PortA, 0) == GPIO_PIN_RESET);
				if(trangthai == 0){
					nga = 0;
					trangthai = 1;
				}else{
					nga = 0;
					trangthai = 0;
				}
        EXTI->PR |= EXTI_PR_PR0; // Xác nh?n r?ng ng?t dã du?c x? lý
    }
}


void EXTI1_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR1) { // Ki?m tra n?u ng?t du?c kích ho?t b?i chân 1
        for (int x = 500000; x > 0; x--);
				nga = 0;
				init_gpio();
				systick_init();
				SystemClock_Config();
				timer_init();
        EXTI->PR |= EXTI_PR_PR1; // Xác nh?n r?ng ng?t dã du?c x? lý
    }
}
void toggle_LED_Red() {
    if (is_LED_RED_on) {
        // N?u dèn dang b?t, t?t nó di
        gpio_write(PortA, 3, GPIO_PIN_RESET); // T?t dèn xanh
        is_LED_RED_on = 0; // C?p nh?t tr?ng thái là t?t
    } else {
        // N?u dèn dang t?t, b?t nó lên
        gpio_write(PortA, 3, GPIO_PIN_SET); // B?t dèn xanh
        is_LED_RED_on = 1; // C?p nh?t tr?ng thái là b?t
    }
}

void toggle_LED_Green() {
    if (is_LED_GREEN_on) {
        // N?u dèn dang b?t, t?t nó di
        gpio_write(PortA, 4, GPIO_PIN_RESET); // T?t dèn xanh
        is_LED_GREEN_on = 0; // C?p nh?t tr?ng thái là t?t
    } else {
        // N?u dèn dang t?t, b?t nó lên
        gpio_write(PortA, 4, GPIO_PIN_SET); // B?t dèn xanh
        is_LED_GREEN_on = 1; // C?p nh?t tr?ng thái là b?t
    }
}
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF) 
    {
        TIM4->SR &= ~TIM_SR_UIF; 
				if(trangthai == 1){
					toggle_LED_Green();
				}else{
					gpio_write(PortA, 4, GPIO_PIN_RESET);
				}
    }
} 

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) { 
        TIM3->SR &= ~TIM_SR_UIF; 
				if(nga == 1){
					toggle_LED_Red();
				}else{
					gpio_write(PortA, 4, GPIO_PIN_RESET);
				}
    }
}

int main(void) {
		SystemClock_Config();
    systick_init();  // Initialize system tick for delays
		init_gpio();
		timer_init();
    lcd_i2c_init(I2C_1);  // Initialize the LCD on I2C bus 1
    float x, y, z;  // Variables to hold accelerometer data
    char buffer[32];  // Buffer to hold the string for LCD display
	
    // Clear the LCD display
//    lcd_i2c_cmd(I2C_1, 0x01);
    delay_ms(50);  // Ensure the display has enough time to clear
	  ADXL345_Init(I2C_2);
    // Initialize the ADXL345 accelerometer
    // Clear the initialization message
    lcd_i2c_cmd(I2C_1, 0x01);
    delay_ms(50);
	  float grav_x = 0;
    float grav_y = 0;
    float grav_z = 0;
    while(1) {
			if(trangthai == 1){
				if(nga == 0){
        // Read the accelerometer values
        ADXL345_ReadXYZ(I2C_2, &x, &y, &z,&grav_x, &grav_y, &grav_z);
        /*snprintf(buffer, sizeof(buffer), "X: %.1f ", x);
        lcd_i2c_msg(I2C_1, 1, 0, buffer);
        snprintf(buffer, sizeof(buffer), "Y: %.1f", y);
        lcd_i2c_msg(I2C_1, 2, 0, buffer);
        snprintf(buffer, sizeof(buffer), "Z: %.1f ", z);
        lcd_i2c_msg(I2C_1, 2, 8, buffer);*/
				lcd_i2c_msg(I2C_1, 1, 0, "  Binh thuong  ");
				if(sqrt(pow(x,2)+pow(y,2)+pow(z,2)) > 6 && fabs(z) > 3){
					nga = 1;
				}
				delay_ms(50);
				}else{
					delay_ms(50);
					lcd_i2c_msg(I2C_1, 1, 0, "      nga      ");
			}
			}else{
				nga = 0;
				lcd_i2c_cmd(I2C_1, 0x01);
				delay_ms(100);
				gpio_write(PortA, 3, GPIO_PIN_RESET);
				gpio_write(PortA, 4, GPIO_PIN_RESET);
			}
    }
}
