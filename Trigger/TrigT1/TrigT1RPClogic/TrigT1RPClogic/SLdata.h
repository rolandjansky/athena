/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SLDATA_H
#define SLDATA_H

#include <list>
#include "MuonCablingTools/BaseObject.h"

#include "TrigT1RPClogic/SLpatterns.h"
#include "TrigT1RPClogic/PADdata.h"


class SLdata : public BaseObject
{
    public:
    typedef std::list < SLpatterns >  PATTERNSlist;
    typedef std::list < SLpatterns* > PatternsList;

    private:
    unsigned long int m_debug;

    PATTERNSlist m_sl_patterns;


    void create_sl_patterns(PADpatterns*);
    SLpatterns* find(const int);

    public:
    SLdata(PADdata*,unsigned long int);
    SLdata(const SLdata&);
    ~SLdata();

    SLdata operator=(const SLdata&);


    unsigned long int debug(void) const {return m_debug;}

    const PATTERNSlist& sl_patterns(void) const {return m_sl_patterns;}
    PatternsList give_patterns(void);

    void PrintElement(std::ostream&,std::string,bool) const;
    void Print(std::ostream&,bool) const;
};

template <class X> X& operator<<(X& stream,const SLdata& data)
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[5000000];
    for (int i=0;i<5000000;++i) buffer[i] = '\0';
    __osstream display(buffer,5000000);
#endif

    data.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
