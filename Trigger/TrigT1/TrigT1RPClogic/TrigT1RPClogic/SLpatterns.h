/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SLPATTERNS_H
#define SLPATTERNS_H

#include <memory>
#include <string>
#include <utility>
#include <list>
#include "TrigT1RPClogic/PADdata.h"
#include "RPC_CondCabling/RpcCablingCondData.h"

#include "TrigT1RPChardware/SectorLogic.h"

class SLpatterns : public RPCtrigDataObject
{
    private:
    int m_sector;
    unsigned long int m_debug;

    PADdata::PatternsList m_pad_patterns;
    
    std::unique_ptr<SectorLogic> m_SectorL;


    public:
    SLpatterns(int,unsigned long int);
    SLpatterns(const SLpatterns&);

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

    SectorLogic* give_SectorL(const RpcCablingCondData* readCdo);

    void Print (std::ostream&,bool) const;
};

template <class X> X& operator<<(X& stream,const SLpatterns& data)
{
    std::ostringstream display;
    data.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
