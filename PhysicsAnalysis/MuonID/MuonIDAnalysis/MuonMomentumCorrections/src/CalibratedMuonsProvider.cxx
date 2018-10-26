/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "CalibratedMuonsProvider.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODEventInfo/EventInfo.h"

namespace CP {
static SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");
 
CalibratedMuonsProvider::CalibratedMuonsProvider( const std::string& name, ISvcLocator* svcLoc ):
       AthAlgorithm( name, svcLoc ),
         m_tool( "CP::MuonCalibrationPeriodTool/MuonCalibrationAndSmearingTool"),
         m_prwTool(""),
         m_useRndNumber(false) {
      declareProperty( "Input", m_inputKey = "Muons" );
      declareProperty( "Output", m_outputKey = "CalibratedMuons"); 
      declareProperty( "Tool", m_tool );
      declareProperty( "prwTool", m_prwTool );
      


}

StatusCode CalibratedMuonsProvider::initialize() {
      ATH_CHECK( m_tool.retrieve() );
      if (!m_prwTool.empty()){
         m_useRndNumber = true;
         ATH_MSG_DEBUG("prwTool is given assume that the selection of the periods is based on the random run number");
         ATH_CHECK(m_prwTool.retrieve());
      }
      return StatusCode::SUCCESS;
}

StatusCode CalibratedMuonsProvider::execute() {
       
      xAOD::MuonContainer* nonconst_muons = 0;
      const xAOD::MuonContainer* muons = 0;
      std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > out;
    
      if(m_inputKey==m_outputKey) { //attempt to modify in-situ, but must retrieve as non-const
         CHECK( evtStore()->retrieve( nonconst_muons, m_inputKey ) );
              muons = nonconst_muons;
               out.first = nonconst_muons;
      } else {
         CHECK( evtStore()->retrieve( muons, m_inputKey ) );
         out = xAOD::shallowCopyContainer( *muons );
         //record to storegate 
         CHECK( evtStore()->record( out.first, m_outputKey ) );
         CHECK( evtStore()->record( out.second, m_outputKey+"Aux.") );
         //add original object link to met recalculations work
         if(!setOriginalObjectLink( *muons, *out.first )) { 
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
         ATH_MSG_DEBUG(" Old pt=" << iParticle->pt());
         if(m_tool->applyCorrection(*iParticle).code()==CorrectionCode::Error) return StatusCode::FAILURE;
         ATH_MSG_DEBUG(" New pt=" << iParticle->pt());
     }


      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace CP
