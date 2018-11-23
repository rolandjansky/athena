/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "CalibratedTracksProvider.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODEventInfo/EventInfo.h"

namespace CP {

static SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");
CalibratedTracksProvider::CalibratedTracksProvider( const std::string& name, ISvcLocator* svcLoc ): 
         AthAlgorithm( name, svcLoc ),
         m_tool( "CP::MuonCalibrationPeriodTool/MuonCalibrationAndSmearingTool"),
         m_prwTool(""),         
         m_detType(2), // MS = 1, ID = 2
         m_useRndNumber(false) {
      declareProperty( "Input", m_inputKey = "InDetTrackParticles" ); // or ExtrapolatedMuonTrackParticles
      declareProperty( "Output", m_outputKey = "CalibratedInDetTrackParticles");
      declareProperty( "Tool", m_tool );
      declareProperty( "prwTool", m_prwTool );      
      declareProperty( "DetType", m_detType );

}

StatusCode CalibratedTracksProvider::initialize() {
      ATH_CHECK( m_tool.retrieve());
       if (!m_prwTool.empty()){
         m_useRndNumber = true;
         ATH_MSG_DEBUG("prwTool is given assume that the selection of the periods is based on the random run number");
         ATH_CHECK(m_prwTool.retrieve());
      }
      return StatusCode::SUCCESS;
}

StatusCode CalibratedTracksProvider::execute() {

      xAOD::TrackParticleContainer* nonconst_tracks = 0;
      const xAOD::TrackParticleContainer* tracks = 0;
      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > out;

      if(m_inputKey==m_outputKey) { //attempt to modify in-situ, but must retrieve as non-const
         CHECK( evtStore()->retrieve( nonconst_tracks, m_inputKey ) );
              tracks = nonconst_tracks;
               out.first = nonconst_tracks;
      } else {
         CHECK( evtStore()->retrieve( tracks, m_inputKey ) );
         out = xAOD::shallowCopyContainer( *tracks );
         //record to storegate 
         CHECK( evtStore()->record( out.first, m_outputKey ) );
         CHECK( evtStore()->record( out.second, m_outputKey+"Aux.") );
         //add original object link to met recalculations work
         if(!setOriginalObjectLink( *tracks, *out.first )) { 
            ATH_MSG_ERROR("Failed to add original object links to shallow copy of " << m_inputKey);
            return StatusCode::FAILURE;
         }  
      }
      if (m_useRndNumber) {
            const xAOD::EventInfo* evInfo = nullptr;
            ATH_CHECK(evtStore()->retrieve(evInfo, "EventInfo"));
            if (!evInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
                m_useRndNumber = false;
                ATH_MSG_DEBUG("On data no random run number is needed.");
            } else if (!acc_rnd.isAvailable(*evInfo)){
                 ATH_MSG_DEBUG("Apply the prw tool");
                 ATH_CHECK(m_prwTool->apply(*evInfo));
            }
      }
      for(auto iParticle : *(out.first)) {
        ATH_MSG_VERBOSE("Old pt=" << iParticle->pt());
        if(m_tool->applyCorrectionTrkOnly(*iParticle, m_detType).code()==CorrectionCode::Error) return StatusCode::FAILURE;
        ATH_MSG_VERBOSE("New pt=" << iParticle->pt());
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace CP
