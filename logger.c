/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.c
 * Author: emiel
 *
 * Created on February 9, 2017, 7:14 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>

/*
 * 
 */
void logger(int level, const char* message) {
    //openlog("GBE",LOG_PID|LOG_CONS, LOG_USER);
    //syslog(level,"%s", message);
    //closelog();
    //printf("%d %s",level, message);
    //printf("\n");
}

