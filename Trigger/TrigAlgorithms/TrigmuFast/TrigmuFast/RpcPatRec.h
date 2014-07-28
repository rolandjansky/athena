// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_RPCPATREC_H
#define PHYSICS_MU_RPCPATREC_H

#include "TrigmuFast/SequenceBase.h"
#include "TrigmuFast/Datatypes.h"


class MuMessageInterface;
class MuServiceInterface;

class RpcPatRec: public SequenceBase {

public:
    RpcPatRec(MuMessageInterface& msg,MuServiceInterface& svc, bool tilt_field, bool usePol2ForEPt,
              const TriggerData& trigger_data );
    virtual ~RpcPatRec(void);

public:
    virtual MUFAST::ErrorCode start(MuonFeatureDetails* det=0);

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const;

public:
    const TrackFit& trig_track(void) const;
    const RoadRegions& muon_roads(void) const;


protected:                      // sevices
    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;

protected:                      // input
    bool m_tilt_field;
    bool m_usePol2ForEPt;
    const TriggerData& m_trigger_data;

protected:                      // output
    TrackFit    m_trig_track;
    RoadRegions m_muon_roads;

protected:
    static const std::string s_type;
    static const std::string s_name;

};

inline std::string
RpcPatRec::type(void) const
{
    return s_type;
}

inline std::string
RpcPatRec::name(void) const
{
    return s_name;
}


inline const TrackFit&
RpcPatRec::trig_track(void) const
{
    return m_trig_track;
}


inline const RoadRegions&
RpcPatRec::muon_roads(void) const
{
    return m_muon_roads;
}



#endif // PHYSICS_MU_RPCPATREC_H

