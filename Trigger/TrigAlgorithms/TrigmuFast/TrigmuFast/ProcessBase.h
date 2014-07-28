// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_PROCESSBASE_H
#define PHYSICS_MU_PROCESSBASE_H


#include <string>

namespace MUFAST {

enum CsmErrorCode
{
    CSM_NOT_CONFIGURED,
    CSM_WITH_NO_GEOMETRY,
    TDC_GEOMETRY_MISSING
};

}

class MuonFeatureDetails;

class ProcessBase {

public:
    ProcessBase(void);
    virtual ~ProcessBase(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const = 0;
    virtual unsigned short int number() const = 0;

public:
    virtual std::string type(void) const = 0;
    virtual std::string name(void) const = 0;
    
};


#endif // PHYSICS_MU_PROCESSBASE_H
