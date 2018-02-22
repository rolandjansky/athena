// MyPackage includes
#include "MyPackageAlg.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "PathResolver/PathResolver.h"
#include "xAODRootAccess/TEvent.h"


MyPackageAlg::MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){
}


MyPackageAlg::~MyPackageAlg() {}


StatusCode MyPackageAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed
  	
   // Initialize photonFS tool
   CP::IsolationCorrectionTool m_isoCorrTool("isoCorrTool");
   m_isoCorrTool.setProperty("IsMC", true); //if MC, else false

   std::string file_unc = PathResolverFindCalibFile("IsolationCorrections/v1/isolation_ptcorrections_rel207.root");
   m_isoCorrTool.setProperty("CorrFile",file_unc);
  
   if(!m_isoCorrTool.initialize()){
     std::cout <<"Failed to initialize the tool, check for errors"<<std::endl;
     return 0;
   }
   
   
   const xAOD::EventInfo* ei = 0;
	CHECK( evtStore()->retrieve( ei , "EventInfo" ) ); //retrieves the event info
	ATH_MSG_INFO(" --- eventNumber: " << ei->eventNumber() ); //example printout of the event number
   
  	// Get the Photon container from the event:
    const xAOD::PhotonContainer *photons = 0;
	evtStore()->retrieve( photons, "Photons" ); 
  
  	//Clone 
	std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = xAOD::shallowCopyContainer( *photons );
	
	//Iterate over the shallow copy
	xAOD::PhotonContainer* phsCorr = photons_shallowCopy.first;
	xAOD::PhotonContainer::iterator ph_itr      = phsCorr->begin();
	xAOD::PhotonContainer::iterator ph_end      = phsCorr->end();
  
  
  	unsigned int i = 0;
	for( ; ph_itr != ph_end; ++ph_itr, ++i ) {
	  xAOD::Photon* ph = *ph_itr;
	  
	  // skip photons with pt outsize the acceptance
	  if(ph->pt()<100.0||ph->pt()>999000.0) continue;
	  if( fabs(ph->eta())>2.47) continue;
	   
	   pt_corrVal_cone20 = m_isoCorrTool.GetPtCorrection(*ph , xAOD::Iso::topoetcone20) ;
	   pt_corrVal_cone40 = m_isoCorrTool.GetPtCorrection(*ph , xAOD::Iso::topoetcone40) ;
	   
	   dd_corrVal_cone20 = m_isoCorrTool.GetDDCorrection(*ph , xAOD::Iso::topoetcone20) ;
	   dd_corrVal_cone40 = m_isoCorrTool.GetDDCorrection(*ph , xAOD::Iso::topoetcone40) ;
		
		std::cout << pt_corrVal_cone20 << " " << pt_corrVal_cone40 << " " << dd_corrVal_cone20 << " " << dd_corrVal_cone40 << std::endl ;
	   	   
	}  
	
	  m_isoCorrTool.finalize();
   
   
  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::beginInputFile() { 
  return StatusCode::SUCCESS;
}


