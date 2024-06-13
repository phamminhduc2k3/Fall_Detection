#include "gpio.h"

void gpio_init(unsigned short PORT, unsigned short PIN, unsigned short CNF, unsigned short MODE){
    GPIO_TypeDef *GPIOx;
    switch (PORT) {
        case PortA:
            GPIOx = GPIOA;
            RCC->APB2ENR |= (1 << 2); // enable GPIOA
            break;
        case PortB:
            GPIOx = GPIOB;
            RCC->APB2ENR |= (1 << 3); // enable GPIOB
            break;
        case PortC:
            GPIOx = GPIOC;
            RCC->APB2ENR |= (1 << 4); // enable GPIOC
            break;
        default:
            // Handle error or default case
            return;
    }

    if (PIN < 8){
        GPIOx->CRL &= ~(uint32_t)((0xF) << (PIN) * 4); // clear 4 bit in config register
        GPIOx->CRL |= (MODE) << (PIN * 4);
        GPIOx->CRL |= (CNF) << (PIN * 4 + 2);
    } else {
        GPIOx->CRH &= ~(uint32_t)((0xF) << ((PIN- 8)) * 4); // clear 4 bit in config register
        GPIOx->CRH |= (MODE) << ((PIN - 8) * 4);
        GPIOx->CRH |= (CNF) << ((PIN - 8) * 4 + 2);
    }
}


void gpio_write(unsigned short PORT, unsigned short PIN, unsigned short PinState){
    if (PinState == GPIO_PIN_SET) {
        switch (PORT) {
            case PortA:
                GPIOA->BSRR |= (1 << PIN);
                break;
            case PortB:
                GPIOB->BSRR |= (1 << PIN);
                break;
            case PortC:
                GPIOC->BSRR |= (1 << PIN);
                break;
            default:
                break;
        }
    } else {
        switch (PORT) {
            case PortA:
                GPIOA->BSRR |= (1 << (PIN + 16));
                break;
            case PortB:
                GPIOB->BSRR |= (1 << (PIN + 16));
                break;
            case PortC:
                GPIOC->BSRR |= (1 << (PIN + 16));
                break;
            default:
                break;
        }
    }
}

uint8_t gpio_read(unsigned short PORT, unsigned short PIN){
    uint32_t pin_mask = (1 << PIN);  // Chuy?n d?i PIN thành giá tr? bit
    switch (PORT) {
        case PortA:
            return ((GPIOA->IDR & pin_mask) != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        case PortB:
            return ((GPIOB->IDR & pin_mask) != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        case PortC:
            return ((GPIOC->IDR & pin_mask) != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        default:
            return GPIO_PIN_RESET; 
    }
}
