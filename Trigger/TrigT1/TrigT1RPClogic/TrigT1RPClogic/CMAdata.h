/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMADATA_H
#define CMADATA_H

#include <list>
#include "MuonCablingTools/BaseObject.h"

#include "TrigT1RPClogic/CMApatterns.h"
#include "TrigT1RPClogic/RPCdata.h"

#include "RPC_CondCabling/CMAparameters.h"


#ifndef LVL1_STANDALONE
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
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
    CMAdata(const RPCdata* rpcData);
#else
    CMAdata(const RPCdata* rpcData,const IRPCcablingSvc* rpcCabling, const unsigned long int debug);
    CMAdata(const RPCdata* rpcData,const RpcCablingCondData* rpcCabling, const unsigned long int debug);
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

#endif
