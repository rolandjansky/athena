// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_SAGESTIMATESTD_H
#define PHYSICS_MU_SAGESTIMATESTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/MuMessageInterface.h"
#include <string>

class TriggerData;
class TrackFit;
class MuFeature;
class SpointData;

class SagEstimateStd: public ProcessBase {

public:
    SagEstimateStd(MuMessageInterface& msg,
                   const TriggerData& triggerData,
                   const TrackFit& track_fit,
                   const SpointData super_points[],
                   MuFeature& feature);
    virtual ~SagEstimateStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    MuMessageInterface* m_msg;
    const TriggerData& m_trig_data;
    const TrackFit& m_track_fit;
    const SpointData* m_super_points;
    MuFeature& m_feature;

private:
    static const std::string s_type;

};

inline unsigned short int
SagEstimateStd::number(void) const
{
    return 0x6;
}

inline std::string
SagEstimateStd::type(void) const
{
    return s_type;
}


#endif // PHYSICS_MU_SAGESTIMATESTD_H
