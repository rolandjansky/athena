/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTracking/TrackParticleContainer.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class TrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      TrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~TrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      //Main thinning service
      ServiceHandle<IThinningSvc> m_thinningSvc;

      //Expression for object thinning selection
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
      std::string m_selectionString;

      //Counters and keys for xAOD::TrackParticle container
      mutable unsigned int m_ntot, m_npass;
      std::string m_inDetSGKey;

      //Counters and keys for xAOD::TrackStateValidation and xAOD::TrackMeasurementValidation containers
      mutable unsigned int m_ntot_pix_states, m_npass_pix_states;
      std::string m_statesPixSGKey;
      mutable unsigned int m_ntot_pix_measurements, m_npass_pix_measurements;
      std::string m_measurementsPixSGKey;
      mutable unsigned int m_ntot_sct_states, m_npass_sct_states;
      std::string m_statesSctSGKey;
      mutable unsigned int m_ntot_sct_measurements, m_npass_sct_measurements;
      std::string m_measurementsSctSGKey;
      mutable unsigned int m_ntot_trt_states, m_npass_trt_states;
      std::string m_statesTrtSGKey;
      mutable unsigned int m_ntot_trt_measurements, m_npass_trt_measurements;
      std::string m_measurementsTrtSGKey;


      //logic
      bool m_and;
      bool m_thinHitsOnTrack;

      //Track state types
      // Subset of enum MeasurementType from Athena: Tracking/TrkEvent/TrkEventPrimitives/TrkEventPrimitives/TrackStateDefs.h
      enum MeasurementType {
	TrkState_unidentified = 0,
	TrkState_Pixel      = 1,
	TrkState_SCT        = 2,
	TrkState_TRT        = 3,
	TrkState_Pseudo     = 8,
	TrkState_Vertex     = 9,
	TrkState_SpacePoint = 11,
	TrkState_NumberOfMeasurementTypes=16
      };

      /// Select TrackStateValidation and TrackMeasurementValidation objects that are used in the (thinned) track container
      void selectTrackHits(const xAOD::TrackParticleContainer *inputTrackParticles, std::vector<bool>& inputMask,
			   MeasurementType detTypeToSelect,
			   std::vector<bool>& outputStatesMask, std::vector<bool>& outputMeasurementsMask) const;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H
