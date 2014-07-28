// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_PTFROMRAD_H
#define PHYSICS_MU_PTFROMRAD_H

#include "TrigmuFast/PtEstimateStd.h"
#include "TrigmuFast/ProcessBase.h"
#include <string>

class MuFeature;

class PtFromRad: public PtEstimateStd {

public:
    PtFromRad(MuMessageInterface& msg,
               MuFeature& feature,
               const MuLUT* mu_lut);
    virtual ~PtFromRad(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);

public:
    virtual std::string name(void) const;

private:
    static const std::string s_name;

};

inline std::string
PtFromRad::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_PTFROMRAD_H
