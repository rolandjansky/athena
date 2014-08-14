/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header:
// Project: TRT Reconstruction
//  Author: Jim Loken
//

#ifndef TRTREC_TRTTIMER_H
#define TRTREC_TRTTIMER_H

class TRT_Timer {
public:
    static TRT_Timer* instance (void);    //access to singleton.
    void start (int i);
    void stop (int i);
    void print (int i);
    int  max_timer ();
    
private:
    TRT_Timer ();
    static TRT_Timer* s_instance;
};

inline TRT_Timer*
TRT_Timer::instance()
{
    if (s_instance == 0) s_instance = new TRT_Timer();
    return s_instance;
}

#endif




