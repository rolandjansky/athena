/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParticleManipulator_H
#define ParticleManipulator_H

#include "FadsKinematics/ManipulatorBase.h"
#include "FadsKinematics/GeneratorCenter.h"
#include <string>
class G4PrimaryParticle;

namespace FADS {


  class ParticleManipulator : public ManipulatorBase {
  public:

    ParticleManipulator(const std::string& s)
      : ManipulatorBase(s)
    {
      GeneratorCenter::GetGeneratorCenter()->AddParticleManipulator(this);
	}

	virtual ~ParticleManipulator() {}

	virtual bool EditParticle(G4PrimaryParticle* v) = 0;
	virtual void EventInitialization() {}

  };


}

#endif
