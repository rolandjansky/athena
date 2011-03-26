/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMADATA_H
#define CMADATA_H

#include <list>
#include "MuonCablingTools/BaseObject.h"

#include "TrigT1RPClogic/CMApatterns.h"
#include "TrigT1RPClogic/RPCdata.h"

#include "RPCcablingInterface/CMAparameters.h"


#ifndef LVL1_STANDALONE
#include "RPCcablingInterface/IRPCcablingSvc.h"
#endif


class CMAdata : public BaseObject
{
    public:
    typedef std::list < CMApatterns > PATTERNSlist;
    typedef std::list < CMApatterns* > PatternsList;

    private:
    unsigned long int m_debug;

    PATTERNSlist m_eta_cma_patterns;
    PATTERNSlist m_phi_cma_patterns;

    
    private:
    CMApatterns* find(const int,const CMAparameters*);

    public:
    CMAdata(unsigned long int);
    
#ifdef LVL1_STANDALONE
    CMAdata(const RPCdata*);
#else
    CMAdata(const RPCdata*,const IRPCcablingSvc*,unsigned long int);
#endif
    CMAdata(const CMAdata&);
    ~CMAdata();

    CMAdata operator=(const CMAdata&);


    void create_patterns(const CMAparameters*,const RPCdigit*);

    unsigned long int debug(void) const {return m_debug;}

    const PATTERNSlist& eta_cma_patterns(void) const 
                                                   {return m_eta_cma_patterns;}
    const PATTERNSlist& phi_cma_patterns(void) const  
                                                   {return m_phi_cma_patterns;}
    PatternsList give_patterns(void);

    void PrintElement(std::ostream&,std::string,bool) const;
    void Print(std::ostream&,bool) const;
};
/*
template <class X> X& operator<<(X& stream,const CMAdata& data)
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
*/

#endif
