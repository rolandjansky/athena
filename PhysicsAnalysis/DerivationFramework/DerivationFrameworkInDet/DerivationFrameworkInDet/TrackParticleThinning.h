/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKINDET_TRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORKINDET_TRACKPARTICLETHINNING_H

// System include(s):
#include <string>
#include <memory>
#include <vector>

// Framework include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

// DF include(s):
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/ExpressionParserHelper.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace InDet
{
  class IInDetTrackSelectionTool;
}

namespace DerivationFramework {

   /// Tool selecting track particles to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning of a
   /// track particle container.
   ///
   /// @author James Catmore (James.Catmore@cern.ch)
   ///
   class TrackParticleThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      TrackParticleThinning( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// @name Function(s) implementing the @c IAlgTool interface
      /// @{

      /// Function initialising the tool
      StatusCode initialize() override;
      /// Function finalising the tool
      StatusCode finalize() override;

      /// @}

      /// @name Function(s) implementing the
      ///       @c DerivationFramework::IThinningTool interface
      /// @{

      /// Function performing the configured thinning operation
      StatusCode doThinning() const override;

      /// @}

   private:
      // Track state types
      // Subset of enum MeasurementType from Athena:
      // Tracking/TrkEvent/TrkEventPrimitives/TrkEventPrimitives/TrackStateDefs.h
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

      /// Select @c TrackStateValidation and @c TrackMeasurementValidation
      /// objects that are used in the (thinned) track container
      StatusCode selectTrackHits( const xAOD::TrackParticleContainer* tpc,
                                  const std::vector< bool >& inputMask,
                                  MeasurementType detTypeToSelect,
                                  std::vector< bool >& outputStatesMask,
                                  std::vector< bool >& outputMeasurementsMask ) const;

      /// Main thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;

      /// Track selection tool
      ToolHandle< InDet::IInDetTrackSelectionTool > m_trkSelTool;

      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_parser;
      /// Selection string to use with the expression evaluation
      std::string m_selectionString;

      /// Counters and keys for @c xAOD::TrackParticle container
      /// @{

      mutable unsigned int m_ntot = 0, m_npass = 0;
      std::string m_inDetSGKey = "InDetTrackParticles";

      /// @}

      /// @name Counters and keys for @c xAOD::TrackStateValidation and
      ///       @c xAOD::TrackMeasurementValidation containers
      /// @{

      mutable unsigned int m_ntot_pix_states = 0, m_npass_pix_states = 0;
      std::string m_statesPixSGKey = "PixelMSOSs";
      mutable unsigned int m_ntot_pix_measurements = 0,
                           m_npass_pix_measurements = 0;
      std::string m_measurementsPixSGKey = "PixelClusters";
      mutable unsigned int m_ntot_sct_states = 0, m_npass_sct_states = 0;
      std::string m_statesSctSGKey = "SCT_MSOSs";
      mutable unsigned int m_ntot_sct_measurements = 0,
                           m_npass_sct_measurements = 0;
      std::string m_measurementsSctSGKey = "SCT_Clusters";
      mutable unsigned int m_ntot_trt_states = 0, m_npass_trt_states = 0;
      std::string m_statesTrtSGKey = "TRT_MSOSs";
      mutable unsigned int m_ntot_trt_measurements = 0,
                           m_npass_trt_measurements = 0;
      std::string m_measurementsTrtSGKey = "TRT_DriftCircles";

      /// @}

      /// logic
      bool m_and = false;
      bool m_thinHitsOnTrack = false;

   }; // class TrackParticleThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKINDET_TRACKPARTICLETHINNING_H
