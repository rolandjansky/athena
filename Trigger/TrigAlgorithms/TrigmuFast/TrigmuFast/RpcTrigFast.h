// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_RPCTRIGFAST_H
#define PHYSICS_MU_RPCTRIGFAST_H

#include "TrigmuFast/RpcTrigStd.h"

class RpcPad;
class RpcTriggerData;
class RpcTrigPatt;
class RPCrawData;
class CMAdata;
class RPCGeometry;
class CablingRPCBase;
class MDTGeometry;
class RPCdigit;

class RpcTrigFast: public RpcTrigStd {
  
public:
    RpcTrigFast(MuMessageInterface& msg,
		const RpcPad* rpc_pad, 
                TriggerData& out,
		RpcTrigPatt& patterns,
		RPCrawData& rpc_digits,
		CMAdata& cma_patterns,
                const RPCGeometry* rpcgeo,
                const CablingRPCBase* rpccab,
		const MDTGeometry* mdtgeo,
		const bool maskUncCMAch,
		const bool trigOnPhiOnly,
		const bool useRpcTrigPatt,
		const float RpcTimeDelay);
    virtual ~RpcTrigFast(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;


protected:
    static const std::string s_name;

private:
    float stationBeta(RPCdigit& phi, RPCdigit& eta, unsigned short int
    SubsystemId, unsigned short int SectorId) const;
};

inline std::string
RpcTrigFast::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_RPCTRIGFAST_H
