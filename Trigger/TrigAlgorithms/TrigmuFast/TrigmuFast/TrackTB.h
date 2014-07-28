// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_TRACKTB_H
#define PHYSICS_MU_TRACKTB_H

#include "TrigmuFast/TrackStd.h"

class RpcTriggerData;
class TrackFit;

class TrackTB: public TrackStd {
  
public:
    TrackTB(MuMessageInterface& msg, 
            const TriggerData& trig_data, TrackFit& track,
	    RoadRegions& roads);
    virtual ~TrackTB(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

protected:
    static const std::string s_name;

};

inline std::string
TrackTB::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_TRACKTB_H
