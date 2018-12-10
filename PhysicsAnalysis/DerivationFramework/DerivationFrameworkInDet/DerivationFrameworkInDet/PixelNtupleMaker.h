/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_PIXELNTUPLEMAKER_H
#define DERIVATIONFRAMEWORK_PIXELNTUPLEMAKER_H


#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

namespace DerivationFramework {

  class PixelNtupleMaker : public AthAlgTool, public ISkimmingTool {
    public: 
      PixelNtupleMaker(const std::string& t, const std::string& n, const IInterface* p);

      ~PixelNtupleMaker();

      StatusCode initialize();
      StatusCode finalize();
      virtual bool eventPassesFilter() const;
      void GetLayerEtaPhiFromId(uint64_t id,int *barrelEC, int *layer, int *eta, int *phi) const;

    private:
      std::string m_containerName;

      typedef std::vector<ElementLink< xAOD::TrackStateValidationContainer > > MeasurementsOnTrack;
      typedef std::vector<ElementLink< xAOD::TrackStateValidationContainer > >::const_iterator MeasurementsOnTrackIter;
  }; 
}

#endif
