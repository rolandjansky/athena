// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_CONTIGUITYADM_H
#define PHYSICS_MU_CONTIGUITYADM_H

#include "TrigmuFast/ContiguityStd.h"
#include <string>

class LayerDigits;

class ContiguityADM: public ContiguityStd {

public:
    ContiguityADM(MuMessageInterface& msg,
                  LayerDigits (*digits)[MAXLAYER]); // output
    virtual ~ContiguityADM(void);

public:
    virtual std::string name(void) const;

public:
    virtual bool run(MuonFeatureDetails* det=0);

private:
    static const std::string s_name;

};

inline std::string
ContiguityADM::name(void) const
{
    return s_name;
}


#endif // PHYSICS_MU_CONTIGUITYADM_H
