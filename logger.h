/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.h
 * Author: emiel
 *
 * Created on February 9, 2017, 7:14 PM
 */

#ifndef LOGGER_H
#define LOGGER_H
#include <syslog.h>
void logger(int level, const char* message);

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */

