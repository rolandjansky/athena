/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HepMCEventConverter_H
#define HepMCEventConverter_H

#include "HepMC/GenEvent.h"
class G4Event;

namespace FADS {

class HepMCEventConverter {
public:
    virtual ~HepMCEventConverter() {;}
	virtual void HepMC2G4(const HepMC::GenEvent* ,G4Event *,const bool) const;
};

}	// end namespace

#endif
