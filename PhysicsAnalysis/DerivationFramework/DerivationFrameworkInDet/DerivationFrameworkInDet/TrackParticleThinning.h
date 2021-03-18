/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <atomic>
#include <vector>

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class TrackParticleThinning : public extends<ExpressionParserUser<AthAlgTool>, IThinningTool> {
    public: 
      TrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~TrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      //Expression for object thinning selection
      std::string m_selectionString;

      //Counters and keys for xAOD::TrackParticle container
      mutable std::atomic<unsigned int> m_ntot, m_npass;
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
        { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };

      //Counters and keys for xAOD::TrackStateValidation and xAOD::TrackMeasurementValidation containers
      mutable std::atomic<unsigned int> m_ntot_pix_states, m_npass_pix_states;
      SG::ThinningHandleKey<xAOD::TrackStateValidationContainer> m_statesPixSGKey
        { this, "InDetTrackStatesPixKey", "PixelMSOSs", "" };
      mutable std::atomic<unsigned int> m_ntot_pix_measurements, m_npass_pix_measurements;
      SG::ThinningHandleKey<xAOD::TrackMeasurementValidationContainer> m_measurementsPixSGKey
        { this, "InDetTrackMeasurementsPixKey", "PixelClusters", "" };
      mutable std::atomic<unsigned int> m_ntot_sct_states, m_npass_sct_states;
      SG::ThinningHandleKey<xAOD::TrackStateValidationContainer> m_statesSctSGKey
        { this, "InDetTrackStatesSctKey", "SCT_MSOSs", "" };
      mutable std::atomic<unsigned int> m_ntot_sct_measurements, m_npass_sct_measurements;
      SG::ThinningHandleKey<xAOD::TrackMeasurementValidationContainer> m_measurementsSctSGKey
        { this, "InDetTrackMeasurementsSctKey", "SCT_Clusters", "" };
      mutable std::atomic<unsigned int> m_ntot_trt_states, m_npass_trt_states;
      SG::ThinningHandleKey<xAOD::TrackStateValidationContainer> m_statesTrtSGKey
        { this, "InDetTrackStatesTrtKey", "TRT_MSOSs", "" };
      mutable std::atomic<unsigned int> m_ntot_trt_measurements, m_npass_trt_measurements;
      SG::ThinningHandleKey<xAOD::TrackMeasurementValidationContainer> m_measurementsTrtSGKey
        { this, "InDetTrackMeasurementsTrtKey", "TRT_DriftCircles", "" };

      // For P->T converter of SCT_Clusters
      SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

      //logic
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
      void selectTrackHits(const xAOD::TrackParticleContainer& inputTrackParticles,
                           const std::vector<bool>& inputMask,
			   MeasurementType detTypeToSelect,
			   std::vector<bool>& outputStatesMask, std::vector<bool>& outputMeasurementsMask) const;

    void filterTrackHits
      (const EventContext& ctx,
       MeasurementType detTypeToSelect,
       const xAOD::TrackParticleContainer& inputTrackParticles,
       const std::vector<bool>& inputMask,
       const SG::ThinningHandleKey<xAOD::TrackStateValidationContainer>& statesKey,
       const SG::ThinningHandleKey<xAOD::TrackMeasurementValidationContainer>& measurementsKey,
       std::atomic<unsigned int>& ntot_states,
       std::atomic<unsigned int>& ntot_measurements,
       std::atomic<unsigned int>& npass_states,
       std::atomic<unsigned int>& npass_measurements) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H
