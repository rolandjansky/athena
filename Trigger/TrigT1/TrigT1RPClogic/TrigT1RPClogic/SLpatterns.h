/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SLPATTERNS_H
#define SLPATTERNS_H

#include <string>
#include <utility>
#include <list>
#include "TrigT1RPClogic/PADdata.h"

#include "TrigT1RPChardware/SectorLogic.h"

class SLpatterns : public RPCtrigDataObject
{
    private:
    int m_sector;
    unsigned long int m_debug;

    PADdata::PatternsList m_pad_patterns;
    
    SectorLogic* m_SectorL;

    void create_hardware(void);

    public:
    SLpatterns(int,unsigned long int);
    SLpatterns(const SLpatterns&);
    ~SLpatterns();

    SLpatterns operator=(const SLpatterns&);

    bool operator==(const SLpatterns&) const;
    bool operator!=(const SLpatterns&) const;
    bool operator< (const SLpatterns&) const; 

    void load_pad_patterns(PADpatterns*);
    SLpatterns& operator<<(PADpatterns*);

    int sector(void) const {return m_sector;}

    unsigned long int debug(void) const {return m_debug;}

    const PADdata::PatternsList pad_patterns(void) const 
                                      {return m_pad_patterns;}

    SectorLogic* give_SectorL(void);

    void Print (std::ostream&,bool) const;
};

template <class X> X& operator<<(X& stream,const SLpatterns& data)
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[300000];
    for (int i=0;i<300000;++i) buffer[i] = '\0';
    __osstream display(buffer,300000);
#endif

    data.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
