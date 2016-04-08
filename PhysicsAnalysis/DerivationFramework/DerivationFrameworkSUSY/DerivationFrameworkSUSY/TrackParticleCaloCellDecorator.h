/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  TrackParticleCaloCellDecorator.h  */

#ifndef DERIVATIONFRAMEWORK_TRACKPARTICLECALOCELLDECORATOR_H
#define DERIVATIONFRAMEWORK_TRACKPARTICLECALOCELLDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class TrackParticleCaloCellDecorator : public AthAlgTool, public IAugmentationTool {
	public:
      		TrackParticleCaloCellDecorator(const std::string& t, const std::string& n, const IInterface* p);
		StatusCode initialize();
      		StatusCode finalize();
      		virtual StatusCode addBranches() const;

    	private:
      		std::string m_sgName;
      		std::string m_containerName;
  };
}

#endif // DERIVATIONFRAMEWORK_TRACKPARTICLECALOCELLDECORATOR_H

