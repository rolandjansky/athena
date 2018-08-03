///////////////////////////////////////////////////////////////////
// PixelOfflineMonitoring.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PIXELOFFLINEMONITORING_H
#define DERIVATIONFRAMEWORK_PIXELOFFLINEMONITORING_H


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

  class PixelOfflineMonitoring : public AthAlgTool, public ISkimmingTool {
    public: 
      PixelOfflineMonitoring(const std::string& t, const std::string& n, const IInterface* p);

      ~PixelOfflineMonitoring();

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

#endif // DERIVATIONFRAMEWORK_PIXELOFFLINEMONITORING_H
