/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/VertexContainer.h"

// Local include(s):
#include "DerivationFrameworkInDet/TrackParticleThinning.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

namespace DerivationFramework {

   TrackParticleThinning::TrackParticleThinning( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )

      : AthAlgTool      ( type, name, parent    ),
        m_thinningSvc   ( "ThinningSvc", name   ),
        m_trkSelTool    ("",this                ){

      // Declare the tool's interface to the framework:
      declareInterface< DerivationFramework::IThinningTool >( this );

      // Set up the tool's properties:
      declareProperty( "ThinningService", m_thinningSvc,
                       "The thinning service to use" );
      declareProperty("TrackSelectionTool", m_trkSelTool,
                      "TrackSelectionTool, used instead of parser if provided");
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the track particles" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
      declareProperty( "ThinHitsOnTrack", m_thinHitsOnTrack );
      declareProperty( "InDetTrackParticlesKey", m_inDetSGKey,
                       "StoreGate key for the track particles" );
      declareProperty( "InDetTrackStatesPixKey", m_statesPixSGKey );
      declareProperty( "InDetTrackMeasurementsPixKey",
                       m_measurementsPixSGKey );
      declareProperty( "InDetTrackStatesSctKey", m_statesSctSGKey );
      declareProperty( "InDetTrackMeasurementsSctKey", m_measurementsSctSGKey );
      declareProperty( "InDetTrackStatesTrtKey", m_statesTrtSGKey );
      declareProperty( "InDetTrackMeasurementsTrtKey", m_measurementsTrtSGKey );
   }

   StatusCode TrackParticleThinning::initialize() {

      ATH_MSG_VERBOSE( "initialize() ..." );
      if( m_selectionString == ""  && m_trkSelTool.empty() ) {
         ATH_MSG_FATAL( "No inner detector track selection string or track selection tool provided!" );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Track thinning selection string: " << m_selectionString );

      // Set up the text-parsing machinery for thinning the tracks directly
      // according to user cuts
      if( m_selectionString!="" ){
        m_parser.reset( new ExpressionParserHelper( evtStore() ) );
        if( ! m_parser->parser().loadExpression( m_selectionString ) ) {
           ATH_MSG_FATAL( "Failed to interpret expression: "
                          << m_selectionString );
           return StatusCode::FAILURE;
        }
      }

      // Track selection tool
      if( !m_trkSelTool.empty() ){
        ATH_CHECK(m_trkSelTool.retrieve());
      }

      // check xAOD::InDetTrackParticle collection
      if( m_inDetSGKey == "" ) {
         ATH_MSG_FATAL( "No inner detector track collection provided for "
                        "thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Using " << m_inDetSGKey
                    << " as the source collection for inner detector track "
                       "particles" );

      // check availability of xAOD::TrackStateValidation and
      // xAOD::TrackMeasurementValidation containers
      if( m_thinHitsOnTrack ) {
         ATH_MSG_INFO( "Pixel states collection as source for thinning: "
                       << m_statesPixSGKey );
         ATH_MSG_INFO( "Pixel measurements collection as source for thinning: "
                       << m_measurementsPixSGKey );
         if( ( m_statesPixSGKey == "" ) || ( m_measurementsPixSGKey == "" ) ) {
            ATH_MSG_WARNING( "No Pixel measurement or state collection "
                             "specified. Pixel hits won't be thinned." );
         }

         ATH_MSG_INFO( "SCT states collection as source for thinning: "
                       << m_statesSctSGKey );
         ATH_MSG_INFO( "SCT measurements collection as source for thinning: "
                       << m_measurementsSctSGKey );
         if( ( m_statesSctSGKey == "" ) || ( m_measurementsSctSGKey == "" ) ) {
            ATH_MSG_WARNING( "No SCT measurement or state collection "
                             "specified. SCT hits won't be thinned." );
         }

         ATH_MSG_INFO( "TRT states collection as source for thinning: "
                       << m_statesTrtSGKey );
         ATH_MSG_INFO( "TRT measurements collection as source for thinning: "
                       << m_measurementsTrtSGKey );
         if( ( m_statesTrtSGKey == "" ) || ( m_measurementsTrtSGKey == "" ) ) {
            ATH_MSG_WARNING( "No TRT measurement or state collection "
                             "specified. TRT hits won't be thinned." );
         }
      }

      // Access the thinning service:
      ATH_CHECK( m_thinningSvc.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrackParticleThinning::finalize() {

      ATH_MSG_VERBOSE( "finalize() ..." );
      ATH_MSG_INFO( "Processed " << m_ntot << " tracks, " << m_npass
                    << " were retained" );
      if( m_thinHitsOnTrack ) {
         ATH_MSG_INFO( "Pixel state objects thinning, "
                       "Total / Passed (Efficiency): "
                       << m_ntot_pix_states << " / " << m_npass_pix_states
                       << " (" << ( m_ntot_pix_states == 0 ? 0 :
                                    static_cast< float >( m_npass_pix_states ) /
                                    m_ntot_pix_states ) << ")" );
         ATH_MSG_INFO( "Pixel measurements objects thinning, "
                       "Total / Passed (Efficiency): "
                       << m_ntot_pix_measurements << " / "
                       << m_npass_pix_measurements
                       << " (" << ( m_ntot_pix_measurements == 0 ? 0 :
                                    static_cast< float >( m_npass_pix_measurements ) /
                                    m_ntot_pix_measurements ) << ")" );
         ATH_MSG_INFO( "SCT state objects thinning, "
                       "Total / Passed (Efficiency): "
                       << m_ntot_sct_states << " / " << m_npass_sct_states
                       << " (" << ( m_ntot_sct_states == 0 ? 0 :
                                    static_cast< float >( m_npass_sct_states ) /
                                    m_ntot_sct_states ) << ")" );
         ATH_MSG_INFO( "SCT measurements objects thinning, "
                       "Total / Passed (Efficiency): "
                       << m_ntot_sct_measurements << " / "
                       << m_npass_sct_measurements
                       << " (" << ( m_ntot_sct_measurements == 0 ? 0 :
                                    static_cast< float >( m_npass_sct_measurements ) /
                                    m_ntot_sct_measurements ) << ")" );
         ATH_MSG_INFO("TRT state objects thinning, "
                      "Total / Passed (Efficiency): "
                      << m_ntot_trt_states << " / " << m_npass_trt_states
                      << " (" << ( m_ntot_trt_states == 0 ? 0 :
                                   static_cast< float >( m_npass_trt_states ) /
                                   m_ntot_trt_states ) << ")" );
         ATH_MSG_INFO( "TRT measurements objects thinning, "
                       "Total / Passed (Efficiency): "
                       << m_ntot_trt_measurements << " / "
                       << m_npass_trt_measurements
                       << " (" << ( m_ntot_trt_measurements == 0 ? 0 :
                                    static_cast< float >( m_npass_trt_measurements ) /
                                    m_ntot_trt_measurements ) << ")" );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrackParticleThinning::doThinning() const {

      // Retrieve main TrackParticle collection
      const xAOD::TrackParticleContainer* importedTrackParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTrackParticles, m_inDetSGKey ) );

      // Check the event contains tracks
      const size_t nTracks = importedTrackParticles->size();
      if( nTracks == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full TrackParticle
      // collection
      std::vector< bool > mask( nTracks, false );
      m_ntot += nTracks;

      if( !m_trkSelTool.empty() ){

        const xAOD::VertexContainer* primaryVertices { nullptr };
        ATH_CHECK( evtStore()->retrieve( primaryVertices, "PrimaryVertices" ) );

        const xAOD::Vertex* priVtx { nullptr };
        for( const auto* vtx : *primaryVertices ) {
          if( vtx->vertexType() == xAOD::VxType::PriVtx ) {
            priVtx = vtx;
            break;
          }
        }

        // Measure relative to PV
        // Thin tracks if there is no PV in the event
        if( priVtx ){
          for (const auto& trk : *importedTrackParticles ) {
            if( m_trkSelTool->accept(*trk,priVtx) ){
              int index = trk->index();
              mask[index] = true;
            }
          }
        }
      } // Track selection tool
      else{
        // Execute the text parser and update the mask
        const std::vector< int > entries = m_parser->parser().evaluateAsVector();
        const size_t nEntries = entries.size();
        // check the sizes are compatible
        if( nTracks != nEntries ) {
           ATH_MSG_ERROR( "Sizes incompatible! Are you sure your selection "
                          "string used ID TrackParticles?" );
           return StatusCode::FAILURE;
        }
        // set mask
        for( size_t i = 0; i < nTracks; ++i ) {
          if( entries.at( i ) == 1 ) {
            mask.at( i ) = true;
          }
        }
      } // Text parser

      // Count the mask
      for( bool bit : mask ) {
         if( bit ) {
            ++m_npass;
         }
      }

      // If thinning hits on track, look over States and Measurements
      // collections as well note: declare containers and masks anyway
      const xAOD::TrackStateValidationContainer *importedStatesPix(0),
            *importedStatesSct(0), *importedStatesTrt(0);
      std::vector<bool> maskStatesPix, maskStatesSct, maskStatesTrt;
      const xAOD::TrackMeasurementValidationContainer *importedMeasurementsPix(0),
            *importedMeasurementsSct(0), *importedMeasurementsTrt(0);
      std::vector<bool> maskMeasurementsPix, maskMeasurementsSct, maskMeasurementsTrt;
      if (m_thinHitsOnTrack) {
         // -- Retrieve containers and setup mask vectors
         ATH_MSG_DEBUG( "Retrieving states and measurements containers" );
         // Pixels
         if( ! evtStore()->contains< xAOD::TrackStateValidationContainer >( m_statesPixSGKey ) ) {
            ATH_MSG_WARNING( "Skipping Pixel states thinning. No "
                             "xAOD::TrackStateValidation Pixel collection with "
                             "name "
                             << m_statesPixSGKey << " found in StoreGate." );
         } else {
            ATH_CHECK( evtStore()->retrieve( importedStatesPix,
                                             m_statesPixSGKey ) );
            const size_t size_states_pix = importedStatesPix->size();
            if( size_states_pix == 0 ) {
               ATH_MSG_WARNING( "Pixel states container is empty: "
                                << m_statesPixSGKey );
            } else {
               maskStatesPix.assign( size_states_pix, false ); // default: don't keep any
               m_ntot_pix_states += size_states_pix;
            }
         }
         if( ! evtStore()->contains< xAOD::TrackMeasurementValidationContainer >( m_measurementsPixSGKey ) ) {
            ATH_MSG_WARNING( "Skipping Pixel measurements thinning. No "
                             "xAOD::TrackMeasurementValidation Pixel "
                             "collection with name "
                             << m_measurementsPixSGKey
                             << " found in StoreGate." );
         } else {
            ATH_CHECK( evtStore()->retrieve( importedMeasurementsPix,
                                             m_measurementsPixSGKey ) );
            const size_t size_measurements_pix =
                  importedMeasurementsPix->size();
            if( size_measurements_pix == 0 ) {
               ATH_MSG_WARNING( "Pixel measurements container is empty: "
                                << m_measurementsPixSGKey );
            } else {
               maskMeasurementsPix.assign( size_measurements_pix,
                                           false ); // default: don't keep any
               m_ntot_pix_measurements += size_measurements_pix;
            }
         }
         // SCT
         if( ! evtStore()->contains< xAOD::TrackStateValidationContainer >( m_statesSctSGKey ) ) {
            ATH_MSG_WARNING( "Skipping SCT states thinning. No "
                             "xAOD::TrackStateValidation SCT collection with "
                             "name "
                             << m_statesSctSGKey << " found in StoreGate." );
         } else {
            ATH_CHECK( evtStore()->retrieve( importedStatesSct,
                                             m_statesSctSGKey ) );
            const size_t size_states_sct = importedStatesSct->size();
            if( size_states_sct == 0 ) {
               ATH_MSG_WARNING( "SCT states container is empty: "
                                << m_statesSctSGKey );
            } else {
               maskStatesSct.assign( size_states_sct,
                                     false ); // default: don't keep any
               m_ntot_sct_states += size_states_sct;
            }
         }
         if( ! evtStore()->contains< xAOD::TrackMeasurementValidationContainer >( m_measurementsSctSGKey ) ) {
            ATH_MSG_WARNING( "Skipping SCT measurements thinning. No "
                             "xAOD::TrackMeasurementValidation SCT collection "
                             "with name "
                             << m_measurementsSctSGKey
                             << " found in StoreGate." );
         } else {
            ATH_CHECK( evtStore()->retrieve( importedMeasurementsSct,
                                             m_measurementsSctSGKey ) );
            const size_t size_measurements_sct =
                  importedMeasurementsSct->size();
            if( size_measurements_sct == 0 ) {
               ATH_MSG_WARNING( "SCT measurements container is empty: "
                                << m_measurementsSctSGKey );
            } else {
               maskMeasurementsSct.assign( size_measurements_sct,
                                           false ); // default: don't keep any
               m_ntot_sct_measurements += size_measurements_sct;
            }
         }
         // TRT
         if( ! evtStore()->contains< xAOD::TrackStateValidationContainer >( m_statesTrtSGKey ) ) {
            ATH_MSG_WARNING( "Skipping TRT states thinning. No "
                             "xAOD::TrackStateValidation TRT collection with "
                             "name "
                             << m_statesTrtSGKey << " found in StoreGate." );
         } else {
            ATH_CHECK( evtStore()->retrieve( importedStatesTrt,
                                             m_statesTrtSGKey ) );
            const size_t size_states_trt = importedStatesTrt->size();
            if( size_states_trt == 0 ) {
               ATH_MSG_WARNING( "TRT states container is empty: "
                                << m_statesTrtSGKey );
            } else {
               maskStatesTrt.assign( size_states_trt,
                                     false ); // default: don't keep any
               m_ntot_trt_states += size_states_trt;
            }
         }
         if( ! evtStore()->contains< xAOD::TrackMeasurementValidationContainer >( m_measurementsTrtSGKey ) ) {
            ATH_MSG_WARNING( "Skipping TRT measurements thinning. No "
                             "xAOD::TrackMeasurementValidation TRT collection "
                             "with name "
                             << m_measurementsTrtSGKey
                             << " found in StoreGate." );
         } else {
            ATH_CHECK( evtStore()->retrieve( importedMeasurementsTrt,
                                             m_measurementsTrtSGKey ) );
            const size_t size_measurements_trt =
                  importedMeasurementsTrt->size();
            if( size_measurements_trt == 0 ) {
               ATH_MSG_WARNING( "TRT measurements container is empty: "
                                << m_measurementsTrtSGKey );
            } else {
               maskMeasurementsTrt.assign( size_measurements_trt,
                                           false ); // default: don't keep any
               m_ntot_trt_measurements += size_measurements_trt;
            }
         }

         // -- Thin containers based on selected xAOD::TrackParticle objects
         ATH_MSG_DEBUG( "Now calculating thinning masks the Pixel hits "
                        "containers" );
         ATH_CHECK( selectTrackHits( importedTrackParticles, mask,
                                     TrkState_Pixel, maskStatesPix,
                                     maskMeasurementsPix ) );
         ATH_MSG_DEBUG( "Now calculating thinning masks the SCT hits "
                        "containers" );
         ATH_CHECK( selectTrackHits( importedTrackParticles, mask,
                                     TrkState_SCT, maskStatesSct,
                                     maskMeasurementsSct ) );
         ATH_MSG_DEBUG( "Now calculating thinning masks the TRT hits "
                        "containers" );
         ATH_CHECK( selectTrackHits( importedTrackParticles, mask,
                                     TrkState_TRT, maskStatesTrt,
                                     maskMeasurementsTrt ) );

         if (importedStatesPix)       for (bool isPassed : maskStatesPix) if (isPassed) m_npass_pix_states++;
         if (importedMeasurementsPix) for (bool isPassed : maskMeasurementsPix) if (isPassed) m_npass_pix_measurements++;
         if (importedStatesSct)       for (bool isPassed : maskStatesSct) if (isPassed) m_npass_sct_states++;
         if (importedMeasurementsSct) for (bool isPassed : maskMeasurementsSct) if (isPassed) m_npass_sct_measurements++;
         if (importedStatesTrt)       for (bool isPassed : maskStatesTrt) if (isPassed) m_npass_trt_states++;
         if (importedMeasurementsTrt) for (bool isPassed : maskMeasurementsTrt) if (isPassed) m_npass_trt_measurements++;
      }

      // Execute the thinning service based on the mask. Finish.
      IThinningSvc::Operator::Type thinningOperator =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
      ATH_CHECK( m_thinningSvc->filter( *importedTrackParticles, mask,
                                        thinningOperator ) );

      if( m_thinHitsOnTrack ) {
         //apply thinning to non-null containers
         if( importedStatesPix ) {
            ATH_MSG_DEBUG( "Now thinning the Pixel states containers" );
            ATH_CHECK( m_thinningSvc->filter( *importedStatesPix, maskStatesPix,
                                              thinningOperator ) );
         }
         if( importedMeasurementsPix ) {
            ATH_MSG_DEBUG( "Now thinning the Pixel measurements containers" );
            ATH_CHECK( m_thinningSvc->filter( *importedMeasurementsPix,
                                              maskMeasurementsPix,
                                              thinningOperator ) );
         }
         if( importedStatesSct ) {
            ATH_MSG_DEBUG( "Now thinning the SCT states containers" );
            ATH_CHECK( m_thinningSvc->filter( *importedStatesSct, maskStatesSct,
                                              thinningOperator ) );
         }
         if( importedMeasurementsSct ) {
            ATH_MSG_DEBUG( "Now thinning the SCT measurements containers" );
            ATH_CHECK( m_thinningSvc->filter( *importedMeasurementsSct,
                                              maskMeasurementsSct,
                                              thinningOperator ) );
         }
         if( importedStatesTrt ) {
            ATH_MSG_DEBUG( "Now thinning the TRT states containers" );
            ATH_CHECK( m_thinningSvc->filter( *importedStatesTrt, maskStatesTrt,
                                              thinningOperator ) );
         }
         if( importedMeasurementsTrt ) {
            ATH_MSG_DEBUG( "Now thinning the TRT measurements containers" );
            ATH_CHECK( m_thinningSvc->filter( *importedMeasurementsTrt,
                                              maskMeasurementsTrt,
                                              thinningOperator ) );
         }
      } //m_thinHitsOnTrack

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrackParticleThinning::
   selectTrackHits( const xAOD::TrackParticleContainer* inputTrackParticles,
                    const std::vector< bool >& inputMask,
                    MeasurementType detTypeToSelect,
                    std::vector< bool >& outputStatesMask,
                    std::vector< bool >& outputMeasurementsMask ) const {

      // loop over track particles, consider only the ones pre-selected by the
      // mask
      for( size_t i = 0; i < inputTrackParticles->size(); ++i ) {

         if( ! inputMask.at( i ) ) {
            continue;
         }
         const xAOD::TrackParticle* trkIt = inputTrackParticles->at( i );

         // Type of the accessor used for the strack state validation links:
         typedef SG::AuxElement::ConstAccessor<
            std::vector< ElementLink< xAOD::TrackStateValidationContainer > >
               > StatesOnTrackAcc;

         // loop over the TrackStateValidation objects, and add them to the
         // outputStatesMask
         static const StatesOnTrackAcc acc( "msosLink" );
         if( ! acc.isAvailable( *trkIt ) ) {
            ATH_MSG_DEBUG( "Cannot find TrackState link from "
                           "xAOD::TrackParticle. Skipping track." );
            continue;
         }
         const auto& measurementsOnTrack = acc( *trkIt );
         for( const auto& trkState_el : measurementsOnTrack ) {
            if( ! trkState_el.isValid() ) {
               ATH_MSG_DEBUG( "Cannot find a valid link to "
                              "TrackStateValidation object for track index: "
                              << i );
               continue; //not a valid link
            }
            if( ( *trkState_el )->detType() != detTypeToSelect ) {
               ATH_MSG_VERBOSE( "Discarding TrackState as not of correct type "
                                << detTypeToSelect );
               continue;
            }
            outputStatesMask.at( trkState_el.index() ) = true;

            // get the corresponding TrackMeasurementValidation object, if any, and add it to the outputMeasurementsMask
            const auto& trkMeasurement_el =
                  ( *trkState_el )->trackMeasurementValidationLink();
            if( ! trkMeasurement_el.isValid() ) {
               ATH_MSG_VERBOSE( "Cannot find a valid link to "
                                "TrackMeasurementValidation object from track "
                                "state for track index: " << i
                                << ", trackState index: "
                                << trkState_el.index() );
               continue; //not a valid link
            }
            outputMeasurementsMask.at( trkMeasurement_el.index() ) = true;
         }
      } // end loop over xAOD::TrackParticle container

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
