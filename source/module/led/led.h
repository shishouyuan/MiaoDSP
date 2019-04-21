/*
 * led.h
 *
 *  Created on: 2018年11月9日
 *      Author: Shouyuan
 */

#ifndef SOURCE_MODULE_LED_LED_H_
#define SOURCE_MODULE_LED_LED_H_

//根据led的极性设置下面的值
typedef enum{
    LED_ST_ON=0,
    LED_ST_OFF=1
}LEDState;

LEDState ledState();
void ledSet(LEDState st);
int ledInit();


#endif /* SOURCE_MODULE_LED_LED_H_ */
