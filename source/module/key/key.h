/*
 * key.h
 *
 *  Created on: 2018��11��15��
 *      Author: Shouyuan
 */

#ifndef SOURCE_MODULE_KEY_KEY_H_
#define SOURCE_MODULE_KEY_KEY_H_

#define KEY_INT_NUM 35

typedef void(*KeyCallbackFunction)(void);


void keyEnable();
int keyInit(KeyCallbackFunction callBack);

#endif /* SOURCE_MODULE_KEY_KEY_H_ */
