/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PADPATTERNS_H
#define PADPATTERNS_H

#include <string>
#include <utility>
#include <list>
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPChardware/Pad.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

class PADpatterns : public RPCtrigDataObject
{
  //public:
  //  typedef std::list< const CMApatterns* > CMApatternsList;

    private:
    int m_sector;
    int m_pad_id;
    unsigned long int m_debug;

    CMAdata::PatternsList m_cma_patterns;
    const IRPCcablingSvc*  m_cabling;
    
    
    Pad* m_pad;

    void create_hardware(void);

    public:
    PADpatterns(int,int,const IRPCcablingSvc*, unsigned long int);

    PADpatterns(const PADpatterns&);
    ~PADpatterns();

    PADpatterns operator=(const PADpatterns&);

    bool operator==(const PADpatterns&) const;
    bool operator!=(const PADpatterns&) const;
    bool operator< (const PADpatterns&) const; 

    void load_cma_patterns(CMApatterns*);
    PADpatterns& operator<<(CMApatterns*);

    int sector(void) const {return m_sector;}
    int pad_id(void) const {return m_pad_id;}
    unsigned long int debug(void) const {return m_debug;}
    const IRPCcablingSvc*  cabling(void) const {return m_cabling;};
    
    const CMAdata::PatternsList cma_patterns(void) const 
                                      {return m_cma_patterns;}

    Pad* give_pad(void);

    void Print (std::ostream&,bool) const;
};

template <class X> X& operator<<(X& stream,const PADpatterns& data)
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
