// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_PTESTIMATESTD_H
#define PHYSICS_MU_PTESTIMATESTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuLUT.h"

#include <string>

class MuFeature;

class PtEstimateStd: public ProcessBase {

public:
    PtEstimateStd(MuMessageInterface& msg,
                  MuFeature& feature,
                  const MuLUT* mu_lut);
    virtual ~PtEstimateStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const;

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    MuMessageInterface* m_msg;
    MuFeature& m_feature;
    const MuLUT* m_lut;

private:
    static const std::string s_type;

};

inline unsigned short int 
PtEstimateStd::number(void) const
{
    return 0x7;
}

inline std::string
PtEstimateStd::type(void) const
{
    return s_type;
}


#endif // PHYSICS_MU_PTESTIMATESTD_H
