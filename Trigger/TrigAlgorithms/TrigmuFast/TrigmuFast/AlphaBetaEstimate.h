// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_ALPHABETAESTIMATE_H
#define PHYSICS_MU_ALPHABETAESTIMATE_H

#include "TrigmuFast/SagEstimateStd.h"

#include <string>

class TriggerData;
class TrackFit;
class MuFeature;
class SpointData;
class PtLUTSvc;

class AlphaBetaEstimate: public SagEstimateStd {

public:
    AlphaBetaEstimate(MuMessageInterface& msg,
                const PtLUTSvc* LUTsvc,
                const TriggerData& triggerData,
                const TrackFit& track_fit,
                const SpointData super_points[],
                MuFeature& feature);
    virtual ~AlphaBetaEstimate(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;


private:
    const PtLUTSvc* pPtLUTSvc;
    static const std::string s_name;
    
    double compute_radius(double InnerSlope, double InnerR, double InnerZ,
                          double MiddleSlope, double MiddleR, double MiddleZ);

};

inline std::string
AlphaBetaEstimate::name(void) const
{
    return s_name;
}



#endif // PHYSICS_MU_ALPHABETAESTIMATE_H
