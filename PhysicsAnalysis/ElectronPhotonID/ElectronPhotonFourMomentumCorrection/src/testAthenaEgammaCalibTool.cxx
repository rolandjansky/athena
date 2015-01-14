/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "PATInterfaces/SystematicCode.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

// local include
#include "testAthenaEgammaCalibTool.h"

testAthenaEgammaCalibTool::testAthenaEgammaCalibTool( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_EgammaCalibrationAndSmearingTool("CP::EgammaCalibrationAndSmearingTool/EgammaCalibrationAndSmearingTool", this)
{
  declareProperty( "SG_Electrons", m_sg_electrons = "ElectronCollection" );
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
   
  std::cout << "SIZE of the systematics set:" << recommendedSystematics.size() << std::endl;
   
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
    std::cout << "Electron " << i << std::endl;
    std::cout << "xAOD/raw pt = " << el->pt() << std::endl;
    
    if(!m_EgammaCalibrationAndSmearingTool->applyCorrection(*el, event_info)){
      ATH_MSG_WARNING("Cannot calibrate electron");
    }
    
    std::cout << "Calibrated pt = " << el->pt() << std::endl;
    
    //systematics
    std::cout << "=============SYSTEMATICS CHECK NOW" << std::endl;
    for (sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr)
      {
	// Tell the calibration tool which variation to apply
	if (m_EgammaCalibrationAndSmearingTool->applySystematicVariation(*sysListItr) != CP::SystematicCode::Ok)
	  {
	    ATH_MSG_ERROR("Cannot configure calibration tool for systematics");
	  }
	//For now remove by hand the photon ones
	TString syst_name = TString(sysListItr->name());
	if(!syst_name.BeginsWith("EL") && !syst_name.BeginsWith("EG")) continue;
	//std::cout << "SYSTEMATIC: " << syst_name << std::endl;
           
	if(m_EgammaCalibrationAndSmearingTool->applyCorrection(*el, event_info) != CP::CorrectionCode::Ok){ 
	  ATH_MSG_WARNING("Cannot calibrate electron"); 
	}
	std::cout << "Calibrated pt with systematic " << syst_name << " = " << el->pt() << std::endl;
      }
    std::cout << "=============END SYSTEMATICS " << std::endl;
  }
  
  //test the correctedCopy method
  unsigned int j = 0; 
  for (; el_it != el_it_last; ++el_it, ++j) { 
    xAOD::Egamma *copy_el = NULL; // new object 
    if (m_EgammaCalibrationAndSmearingTool->correctedCopy( **el_it, copy_el, event_info) != CP::CorrectionCode::Ok){ 
      ATH_MSG_WARNING("Could not apply correction to new electron object"); 
      continue; 
    }
  }
  
  return StatusCode::SUCCESS;
}

