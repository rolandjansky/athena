// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_PTESTIMATE_H
#define PHYSICS_MU_PTESTIMATE_H

#include "TrigmuFast/PtEstimateStd.h"
#include "TrigmuFast/ProcessBase.h"
#include <string>

class MuFeature;

class PtEstimate: public PtEstimateStd {

public:
    PtEstimate(MuMessageInterface& msg,
               MuFeature& feature,
               const MuLUT* mu_lut);
    virtual ~PtEstimate(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

private:
    static const std::string s_name;

};

inline std::string
PtEstimate::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_PTESTIMATE_H
