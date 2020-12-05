/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "AsgMessaging/StatusCode.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

// local include
#include "testAthenaEgammaCalibTool.h"

testAthenaEgammaCalibTool::testAthenaEgammaCalibTool( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_EgammaCalibrationAndSmearingTool("CP::EgammaCalibrationAndSmearingTool/EgammaCalibrationAndSmearingTool", this)
{
  declareProperty( "SG_Electrons", m_sg_electrons = "Electrons" );
  declareProperty( "EgammaCalibrationAndSmearingTool", m_EgammaCalibrationAndSmearingTool);
}


StatusCode testAthenaEgammaCalibTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initialising..." );

  // Retrieve the tools:
  ATH_CHECK(m_EgammaCalibrationAndSmearingTool.retrieve());

  return StatusCode::SUCCESS;
}


StatusCode testAthenaEgammaCalibTool::execute()
{
  const xAOD::EventInfo* event_info = 0;
  ATH_CHECK( evtStore()->retrieve( event_info) );

  const xAOD::ElectronContainer* electrons;
  ATH_CHECK( evtStore()->retrieve(electrons, m_sg_electrons) );

  //Clone 
  std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy = xAOD::shallowCopyContainer( *electrons );
  //Record in StoreGate
  CHECK( evtStore()->record( electrons_shallowCopy.first, "ElectronCollectionCorr") );
  CHECK( evtStore()->record( electrons_shallowCopy.second, "ElectronCollectionCorrAux.") );

  //===========SYSTEMATICS
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  std::vector<CP::SystematicSet> sysList;
  // this is the nominal set
  sysList.push_back(CP::SystematicSet());

  ATH_MSG_INFO("SIZE of the systematics set:" << recommendedSystematics.size()); 
   
  for(CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin();
      sysItr != recommendedSystematics.end(); ++sysItr)
    {
      sysList.push_back(CP::SystematicSet());
      sysList.back().insert(*sysItr);
    }
   
  std::vector<CP::SystematicSet>::const_iterator sysListItr;
  
  //Iterate over the shallow copy
  xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;
  xAOD::ElectronContainer::iterator el_it      = elsCorr->begin();
  xAOD::ElectronContainer::iterator el_it_last = elsCorr->end();
  unsigned int i = 0;
  for (; el_it != el_it_last; ++el_it, ++i) {
    xAOD::Electron* el = *el_it;
    ATH_MSG_INFO("Electron " << i); 
    ATH_MSG_INFO("xAOD/raw pt, eta, phi = " << el->pt() << ", " << el->eta() << ", " << el->phi()); 
    
    if(m_EgammaCalibrationAndSmearingTool->applyCorrection(*el) != CP::CorrectionCode::Ok){
      ATH_MSG_WARNING("Cannot calibrate electron");
    }
    ATH_MSG_INFO("Calibrated pt = " << el->pt()); 
    
    //systematics
    ATH_MSG_INFO("=============SYSTEMATICS CHECK NOW"); 
    for (sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr)
      {
	// Tell the calibration tool which variation to apply
	if (m_EgammaCalibrationAndSmearingTool->applySystematicVariation(*sysListItr) != StatusCode::SUCCESS)
	  {
	    ATH_MSG_ERROR("Cannot configure calibration tool for systematics");
	  }
	//For now remove by hand the photon ones
	TString syst_name = TString(sysListItr->name());
	if(!syst_name.BeginsWith("EL") && !syst_name.BeginsWith("EG")) continue;
           
	if(m_EgammaCalibrationAndSmearingTool->applyCorrection(*el) != CP::CorrectionCode::Ok){ 
	  ATH_MSG_WARNING("Cannot calibrate electron"); 
	}

	ATH_MSG_INFO("Calibrated pt with systematic " << syst_name << " = " << el->pt()); 
      }
	ATH_MSG_INFO("=============END SYSTEMATICS "); 
  }
  
  //test the correctedCopy method
  unsigned int j = 0; 
  for (; el_it != el_it_last; ++el_it, ++j) { 
    xAOD::Electron *copy_el = NULL; // new object 
    if (m_EgammaCalibrationAndSmearingTool->correctedCopy( **el_it, copy_el) != CP::CorrectionCode::Ok){ 
      ATH_MSG_WARNING("Could not apply correction to new electron object"); 
      continue; 
    }
  }
  
  return StatusCode::SUCCESS;
}

