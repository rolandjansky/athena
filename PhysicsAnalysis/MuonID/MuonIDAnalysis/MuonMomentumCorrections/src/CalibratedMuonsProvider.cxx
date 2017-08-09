/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "CalibratedMuonsProvider.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

namespace CP {

static const SG::AuxElement::Accessor<ElementLink<xAOD::IParticleContainer> > origLinkAcc("originalObjectLink");

CalibratedMuonsProvider::CalibratedMuonsProvider( const std::string& name, ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
         m_tool( "CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool")
{
      declareProperty( "Input", m_inputKey = "Muons" );
      declareProperty( "Output", m_outputKey = "CalibratedMuons"); 
      declareProperty( "Tool", m_tool );


}

StatusCode CalibratedMuonsProvider::initialize() {
      ATH_CHECK( m_tool.retrieve() );
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
         //add original object link to met recalculations work
         if(!setOriginalObjectLink( *muons, *out.first )) { 
            ATH_MSG_ERROR("Failed to add original object links to shallow copy of " << m_inputKey);
            return StatusCode::FAILURE;
         }  
      }
      
      for(auto iParticle : *(out.first)) {
//          if(m_inputKey!=m_outputKey && !muons->isAvailable< ElementLink<xAOD::IParticleContainer> >("originalObjectLink")) {
// 	   const ElementLink<xAOD::IParticleContainer> link( *muons , iParticle->index() );
//             origLinkAcc(*iParticle) = link;
//             //iParticle->auxdecor< ElementLink<xAOD::IParticleContainer>  >("originalObjectLink") = link;
//          }
         
         if(msg().level()==MSG::VERBOSE) std::cout << name() << " Old pt=" << iParticle->pt();
         if(m_tool->applyCorrection(*iParticle).code()==CorrectionCode::Error) return StatusCode::FAILURE;
         if(msg().level()==MSG::VERBOSE) std::cout << " New pt=" << iParticle->pt() << std::endl;
      }

      //record to storegate 
      CHECK( evtStore()->record( out.first, m_outputKey ) );
      CHECK( evtStore()->record( out.second, m_outputKey+"Aux.") );


      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace CP
