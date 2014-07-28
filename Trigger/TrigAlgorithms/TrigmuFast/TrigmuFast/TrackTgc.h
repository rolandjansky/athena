// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_TRACKTGC_H
#define PHYSICS_MU_TRACKTGC_H

#include "TrigmuFast/TrackStd.h"

class TrackFit;
class MuServiceInterface;

class TrackTgc: public TrackStd {
  
public:
    TrackTgc(MuMessageInterface& msg, const MuServiceInterface* svc,
	     bool tilt_field, bool use_pol2,
	     const TriggerData& trig_data, TrackFit& track,
	     RoadRegions& roads);
    virtual ~TrackTgc(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

protected:
    const MuServiceInterface* m_svc;
    bool m_tilt_field;
    bool m_use_pol2;
    static const std::string s_name;

};

inline std::string
TrackTgc::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_TRACKTGC_H
