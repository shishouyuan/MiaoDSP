/*
 * led.h
 *
 *  Created on: 2018��11��9��
 *      Author: Shouyuan
 */

#ifndef SOURCE_MODULE_LED_LED_H_
#define SOURCE_MODULE_LED_LED_H_

//����led�ļ������������ֵ
typedef enum{
    LED_ST_ON=0,
    LED_ST_OFF=1
}LEDState;

LEDState ledState();
void ledSet(LEDState st);
int ledInit();


#endif /* SOURCE_MODULE_LED_LED_H_ */
