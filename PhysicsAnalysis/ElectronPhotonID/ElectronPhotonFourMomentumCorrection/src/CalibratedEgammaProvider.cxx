/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):

#include "xAODEventInfo/EventInfo.h"

// local include
#include "CalibratedEgammaProvider.h"

#include "xAODBase/IParticleHelpers.h"

#include "Gaudi/Interfaces/IOptionsSvc.h"

namespace CP {

CalibratedEgammaProvider::CalibratedEgammaProvider( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_tool("")
{
  declareProperty( "InputEventInfo", m_inputEventInfo = "", "Specify an EventInfo, if blank, will try to retrieve without key" );
  declareProperty( "Input", m_inputKey = "Electrons", "Electron or photon input collection to calibrate" );
  declareProperty( "Output", m_outputKey = "CalibratedElectrons", "Name of output collection. If same as input key, will try to modify in-situ" );
  declareProperty( "Tool", m_tool, "Leave blank to get an autoconfigured instance");
}

StatusCode CalibratedEgammaProvider::initialize() {
  ATH_MSG_INFO( "Initialising..." );

  if(m_tool.empty()) { //set up a default tool with the es2012c calibration
      m_tool.setTypeAndName("CP::EgammaCalibrationAndSmearingTool/AutoConfiguredEgammaCalibTool");
      ServiceHandle<Gaudi::Interfaces::IOptionsSvc> josvc("JobOptionsSvc",name());
      josvc->set("ToolSvc.AutoConfiguredEgammaCalibTool.esModel", "es2012c");
  }

   ATH_CHECK(m_tool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode CalibratedEgammaProvider::execute() {

   const xAOD::EventInfo* event_info = 0;
   if(m_inputEventInfo.length()>0) ATH_CHECK( evtStore()->retrieve( event_info, m_inputEventInfo) );
   else ATH_CHECK( evtStore()->retrieve( event_info) );

   xAOD::EgammaContainer* nonconst_egamma = 0;
   const xAOD::EgammaContainer* egamma = 0;
   std::pair< xAOD::EgammaContainer*, xAOD::ShallowAuxContainer* > out;
   const xAOD::ElectronContainer* a = 0;
   const xAOD::PhotonContainer* b = 0;
   if(m_inputKey==m_outputKey) { //attempt to modify in-situ, but must retrieve as non-const
      CHECK( evtStore()->retrieve( nonconst_egamma, m_inputKey ) );
      egamma = nonconst_egamma;
      out.first = nonconst_egamma;
   } else {
      CHECK( evtStore()->retrieve( egamma, m_inputKey ) );
      if( (a = dynamic_cast<const xAOD::ElectronContainer*>(egamma)) ) {
         out = xAOD::shallowCopyContainer( *a );
         if(!setOriginalObjectLink( *a, *out.first )) {
            ATH_MSG_ERROR("Failed to add original object links to shallow copy of " << m_inputKey);
            return StatusCode::FAILURE;
         }
      } else if( (b = dynamic_cast<const xAOD::PhotonContainer*>(egamma)) ) {
         out = xAOD::shallowCopyContainer( *b );
         if(!setOriginalObjectLink( *b, *out.first )) {
            ATH_MSG_ERROR("Failed to add original object links to shallow copy of " << m_inputKey);
            return StatusCode::FAILURE;
         }
      }
      else { ATH_MSG_ERROR("Egamma container " << m_inputKey << " is not photons or electrons!!??"); return StatusCode::FAILURE; }

   }

   for(auto iParticle : *(out.first)) {
         if(msg().level()==MSG::VERBOSE) std::cout << name() << " Old pt=" << iParticle->pt();
         if(m_tool->applyCorrection(*iParticle).code()==CorrectionCode::Error) return StatusCode::FAILURE;
         if(msg().level()==MSG::VERBOSE) std::cout << " New pt=" << iParticle->pt() << std::endl;
   }

   //record to storegate (if not modifying insitu) .. must ensure we record with the proper type
   if(m_inputKey!=m_outputKey) {
      if(a) CHECK( evtStore()->record( static_cast<xAOD::ElectronContainer*>(out.first), m_outputKey ) );
      else CHECK( evtStore()->record( static_cast<xAOD::PhotonContainer*>(out.first), m_outputKey ) );
      CHECK( evtStore()->record( out.second, m_outputKey+"Aux.") );
   }



  return StatusCode::SUCCESS;
}

}
