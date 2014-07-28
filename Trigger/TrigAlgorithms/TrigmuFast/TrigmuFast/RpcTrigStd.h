// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_RPCTRIGSTD_H
#define PHYSICS_MU_RPCTRIGSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/MuMessageInterface.h"

class RpcPad;
class TriggerData;
class RpcTrigPatt;
class RPCrawData;
class CMAdata;
class RPCGeometry;
class CablingRPCBase;
class MDTGeometry;

class RpcTrigStd: public ProcessBase {

public:
    RpcTrigStd(MuMessageInterface& msg,
	       const RpcPad* rpc_pad,
	       RPCrawData& rpc_digits,
	       CMAdata& cma_patterns,
               TriggerData& out,
	       RpcTrigPatt& patterns,
               const RPCGeometry* rpcgeo,
               const CablingRPCBase* rpccab,
	       const MDTGeometry* mdtgeo,
	       const bool maskUncCMAch,
	       const bool trigOnPhiOnly,
	       const bool useRpcTrigPatt,
	       const float RpcTimeDelay);
    virtual ~RpcTrigStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:                       // service and configuration
    MuMessageInterface* m_msg;
    const RPCGeometry* m_rpcgeo;
    const CablingRPCBase* m_rpccab;
    const MDTGeometry*  m_mdtgeo;
    const bool m_maskUncCMAch;
    const bool m_trigOnPhiOnly;
    const bool m_useRpcTrigPatt;
    const float m_RpcTimeDelay;

protected:                       // input
    const RpcPad* m_rpc_pad;
    
    RPCrawData& m_rpc_digits;
    CMAdata&    m_cma_patterns;

protected:                       // output
    TriggerData& m_rpc_data;
    RpcTrigPatt& m_rpc_patterns;

protected:
    void fillGeoDataForESD(uint32_t,float&,float&,float&,float&,float&);

private:
    static const std::string s_type;

};

inline unsigned short int
RpcTrigStd::number() const
{
    return 0x0;
}

inline std::string
RpcTrigStd::type(void) const
{
    return s_type;
}

#endif // PHYSICS_MU_RPCTRIGSTD_H
