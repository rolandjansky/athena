/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:
// Project: TRT recontstruction
//  Author: Jim Loken
//
// Implementation of TRT_Timer

#include <TRT_Rec/TRT_Timer.h>
#include <iostream>
#define timex   timex_
extern "C" {void timex(float*);}

TRT_Timer* TRT_Timer::s_instance = 0;
static const int s_max_timer = 20;
static float s_timer[s_max_timer];
static int s_count[s_max_timer];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TRT_Timer::TRT_Timer () 
{
    for (int i = 0; i != s_max_timer; ++i)
    {
	s_timer[i] = 0.0;
	s_count[i] = 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TRT_Timer::start (int i)
//---------------------------------------
{
    if (i < s_max_timer)
    {
	float time;
	timex(&time);
	s_timer[i] -= time;
	++s_count[i];
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TRT_Timer::stop (int i)
//---------------------------------------
{
    if (i < s_max_timer)
    {
	float time;
	timex(&time);
	s_timer[i] += time;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TRT_Timer::print (int i)
//---------------------------------------
{
    if (i < s_max_timer && s_timer[i] != 0.0)
    {
	std::cout << "timer " << i << " = " << s_timer[i] 
		  << ", count = " << s_count[i] << std::endl;
    }
    
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TRT_Timer::max_timer()
//---------------------------------------
{
    return s_max_timer;
}
