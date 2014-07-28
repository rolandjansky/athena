// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_CONTIGUITYSTD_H
#define PHYSICS_MU_CONTIGUITYSTD_H

#include "TrigmuFast/ProcessBase.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuMessageInterface.h"

#include <string>

class LayerDigits;

class ContiguityStd: public ProcessBase {

protected:
    ContiguityStd(MuMessageInterface& msg,LayerDigits (*digits)[MAXLAYER]);
    virtual ~ContiguityStd(void);

public:
    virtual bool run(MuonFeatureDetails* det=0) = 0;
    virtual void printout(void) const;
    virtual unsigned short int number(void) const; 

public:
    virtual std::string type(void) const;
    virtual std::string name(void) const = 0;

protected:
    MuMessageInterface* m_msg;
    LayerDigits (*m_digits)[MAXLAYER];

private:
    static const std::string s_type;

};

inline unsigned short int
ContiguityStd::number() const
{
    return 0x4;
}


inline std::string
ContiguityStd::type(void) const
{
    return s_type;
}


#endif // PHYSICS_MU_CONTIGUITYSTD_H
