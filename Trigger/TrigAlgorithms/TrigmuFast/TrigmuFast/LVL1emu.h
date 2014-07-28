// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_LVL1EMU_H
#define PHYSICS_MU_LVL1EMU_H

#include "TrigmuFast/SequenceBase.h"
#include "TrigmuFast/Datatypes.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"


class MuMessageInterface;
class MuServiceInterface;

class LVL1emu: public SequenceBase {

public:
    LVL1emu(MuMessageInterface& msg,MuServiceInterface& svc, bool maskUncCMAch,
            bool trigOnPhiOnly,bool useRpcTrigPatt,float RpcTimeDelay,
	    const MuData& mu_data);
    virtual ~LVL1emu(void);

public:
    virtual MUFAST::ErrorCode start(MuonFeatureDetails* det=0);

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const;

public:
    const TriggerData& trigger_data(void) const;
    const RpcTrigPatt& rpc_patterns(void) const;
    const RPCrawData&  rpc_digits(void)   const;
    const LUTDigitVec& tgc_digits(void)   const;

protected:                      // sevices and configuration
    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;
    bool m_maskUncCMAch;
    bool m_trigOnPhiOnly;
    bool m_useRpcTrigPatt;
    float m_RpcTimeDelay;

protected:                      // input
    const MuData& m_data;

    RPCrawData m_rpc_digits;
    CMAdata    m_cma_patterns;

    RawVec      m_TgcRawVector;              // List of digits Raw data
    LUTDigitVec m_tgcLutDigits;

protected:                      // output
    TriggerData m_trigger_data;
    RpcTrigPatt m_rpc_patterns;

protected:
    static const std::string s_type;
    static const std::string s_name;

};

inline std::string
LVL1emu::type(void) const
{
    return s_type;
}

inline std::string
LVL1emu::name(void) const
{
    return s_name;
}

inline const TriggerData&
LVL1emu::trigger_data(void) const
{
    return m_trigger_data;
}

inline const RpcTrigPatt&
LVL1emu::rpc_patterns(void) const
{
    return m_rpc_patterns;
}

inline const RPCrawData&
LVL1emu::rpc_digits(void) const
{
    return m_rpc_digits;
}

inline const LUTDigitVec&
LVL1emu::tgc_digits(void) const
{
    return m_tgcLutDigits;
}

#endif // PHYSICS_MU_LVL1EMU_H

