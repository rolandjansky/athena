// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_SAGESTIMATE_H
#define PHYSICS_MU_SAGESTIMATE_H

#include "TrigmuFast/SagEstimateStd.h"

#include <string>

class TriggerData;
class TrackFit;
class MuFeature;
class SpointData;

class SagEstimate: public SagEstimateStd {

public:
    SagEstimate(MuMessageInterface& msg,
                const TriggerData& triggerData,
                const TrackFit& track_fit,
                const SpointData super_points[],
                MuFeature& feature);
    virtual ~SagEstimate(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

protected:
    float f(float x, float c0, float c1, float c2, float c3) const;
    float fp(float x, float c33, float c22, float c1) const;

private:
    static const std::string s_name;

};

inline std::string
SagEstimate::name(void) const
{
    return s_name;
}

inline float
SagEstimate::f(float x, float c0, float c1, float c2, float c3) const
{
    return c0 + x * (c1 + x * (c2 + x * c3));
}

inline float
SagEstimate::fp(float x, float c33, float c22, float c1) const
{
    return c1 + x * (c22 + x * c33);
}


#endif // PHYSICS_MU_SAGESTIMATE_H
