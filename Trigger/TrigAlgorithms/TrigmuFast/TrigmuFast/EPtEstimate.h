// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_EPTESTIMATE_H
#define PHYSICS_MU_EPTESTIMATE_H

#include "TrigmuFast/PtEstimateStd.h"
#include "TrigmuFast/ProcessBase.h"
#include <string>

class MuFeature;
class PtLUTSvc;

class EPtEstimate: public PtEstimateStd
{

public:
    EPtEstimate(MuMessageInterface& msg,
                bool tilt_field,
                const PtLUTSvc* LUTsvc,
                MuFeature& feature,
                const MuLUT* mu_lut);
    virtual ~EPtEstimate(void);

public:
    virtual bool run(MuonFeatureDetails* det=0);
    // CID 23035, 37: NOT_EMITTED, BAD_VA_START
    void display(MuMessageInterface* msg, const char* fmt, ...);

public:
    virtual std::string name(void) const;

private:
    bool m_tilt_field;
    const PtLUTSvc* pPtLUTSvc;
    static const std::string s_name;

};

inline std::string
EPtEstimate::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_EPTESTIMATE_H
