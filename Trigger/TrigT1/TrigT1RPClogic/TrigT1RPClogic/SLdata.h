/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    std::ostringstream display;
    data.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
