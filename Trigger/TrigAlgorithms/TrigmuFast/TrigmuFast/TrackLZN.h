// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_TRACKLZN_H
#define PHYSICS_MU_TRACKLZN_H

#include "TrigmuFast/TrackStd.h"

class RpcTriggerData;
class TrackFit;

class TrackLZN: public TrackStd {
  
public:
    TrackLZN(MuMessageInterface& msg, 
             const TriggerData& trig_data, TrackFit& track,
	     RoadRegions& roads);
    virtual ~TrackLZN(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

protected:
    float f(float x, float c0, float c1, float c2, float c3) const;
    float fp(float x, float c33, float c22, float c1) const;

protected:
    static const std::string s_name;

};

inline std::string
TrackLZN::name(void) const
{
    return s_name;
}

inline float
TrackLZN::f(float x, float c0, float c1, float c2, float c3) const
//          { return c3*x*x*x + c2*x*x + c1*x + c0; }
{
    return c0 + x * (c1 + x * (c2 + x * c3)); // faster
}

inline float
TrackLZN::fp(float x, float c33, float c22, float c1) const
//          { return c33*x*x + c22*x + c1; }
{
    return c1 + x * (c22 + x * c33); // faster
}


#endif // PHYSICS_MU_TRACKLZN_H
