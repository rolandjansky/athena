/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IChronoStatSvc.h"

#include "EMIsolationBuilder.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaInterfaces/IegammaIso.h"
#include "egammaInterfaces/IEMTrackIsolationTool.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticleContainer.h"

#include "FourMomUtils/P4Helpers.h"

#include <algorithm> 
#include <cmath>
#include <stdexcept>
 
//  END OF HEADER FILES INCLUDE

// ===========================================================================
EMIsolationBuilder::EMIsolationBuilder(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : egammaBaseTool(type, name, parent)
{
  //
  // constructor
  //

  // declare interface
  declareInterface<IEMIsolationBuilder>(this);
  
  // The following properties are specified at run-time
  // (declared in jobOptions file)
  
  // Names of containers which contain cells  
  declareProperty("CellsName",m_cellsName="AllCalo", "Names of containers which contain cells ");

  // list of calo to treat
  declareProperty("CaloNums",m_caloNums, "list of calo to treat");

   // Boolean to call isolation variables calculation and filling
  // (NB: this could be important when redoing calculation from AODs)
  declareProperty("UseCaloIsoTool", m_UseCaloIsoTool=true, "Boolean to call calo isolation variables calculation and filling");
  
  // Boolean to use track iso tool
  declareProperty("UseTrackIsoTool", m_UseTrackIsoTool= true, "Boolean to call track isolation variables calculation and filling");

    // Handle of the calorimetric isolation tool
  declareProperty("EMCaloIsolationTool", m_emCaloIsolationTool, "Handle of the EMCaloIsolationTool");

  // Handle of the calorimetric isolation tool
  declareProperty("EMCaloIsoPtCorrectionTool", m_emCaloIsoPtCorrectionTool, "Handle of the EMCaloIsoPtCorrectionTool");

  // Handle of the calorimetric isolation tool
  declareProperty("CaloIsolationTool", m_CaloIsolationTool, "Handle of the CaloIsolationTool");


  // Handle of the EMTrackIsolationTool
  declareProperty("EMTrackIsolationTool", m_emTrackIsolationTool,"Handle of the EMTrackIsolationTool");

  // Boolean for use of cosmics
  declareProperty("isCosmics",m_isCosmics=false,"Boolean for use of cosmics");

  // Boolean to do isolation for TopoSeededPhoton 
  declareProperty("doIsolForTopoSeeded",m_doIsolForTopoSeeded=true,"Boolean for doing isolation for TopoSeeded Photons");

  // in case we want some extra print
  declareProperty("Print",m_Print=false,"in case of extra prints");

  // in case we want some extra print
  declareProperty("Timing",m_timing=false,"do extra timing");
      
  declareProperty("IsoTypes", m_isoTypes, "The isolation types to do: vector of enum type Iso::IsolationType");
      
  declareProperty("TopoIsoTypes", m_topoIsoTypes,"The topo isolation types to do: vector of enum type  Iso::IsolationType");
      
  declareProperty("doEnergyDensityCorrection", m_doEnergyDensityCorrection=true,"Correct isolation variables based on energy density estimations");

 }

// ===============================================================
EMIsolationBuilder::~EMIsolationBuilder()
{
  //
  // destructor
  // 
}

// ==================================================================
StatusCode EMIsolationBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing EMIsolationBuilder");;
 
  unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO) ;
  //check calo number specified
  m_caloSelection = false ;
  if (m_caloNums.size()==0) {
    ATH_MSG_DEBUG(" No calorimeter selection ");
    return StatusCode::SUCCESS;
  } else if  (m_caloNums.size()>nSubCalo ) {
    ATH_MSG_ERROR(" More than "
      << nSubCalo << " calo specified. Must be wrong. Stop.");
    return StatusCode::FAILURE;
  } else {
    m_caloSelection = true ;
    
    for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
      if (m_caloNums[index]<0 ||  m_caloNums[index]>=(int)nSubCalo ) {
        ATH_MSG_ERROR("Invalid calo specification:"
          << m_caloNums[index] << "Stop.");
        return StatusCode::FAILURE;
  
      } else {
  ATH_MSG_DEBUG(" Select calorimeter " << m_caloNums[index]);
      }
    }
  }
  
  // Pointer to Tool Service
  IToolSvc* p_toolSvc = 0;
  StatusCode sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    ATH_MSG_FATAL(" Tool Service not found ");
    return StatusCode::FAILURE;
  } 
    

  m_doTopoIso = true;
  if (m_topoIsoTypes.size() == 0) {
    ATH_MSG_DEBUG("No topo iso requested, turning off");
    m_doTopoIso = false;
  }else{
  
  }

  // 
  // call calorimeter isolation tool only if needed
  //
  if (m_UseCaloIsoTool) {
    if ((sc = RetrieveEMCaloIsolationTool()).isFailure()) {
      return sc;
    }
    if ((sc = RetrieveEMCaloIsoPtCorrectionTool()).isFailure()) {
      return sc;
    }
    if (m_doTopoIso) {
      if((sc = RetrieveCaloIsolationTool()).isFailure()){
	return sc;
      }
    }
  }

  // 
  // call track isolation tool only if needed
  //
  if (m_UseTrackIsoTool) {
    if ((sc = RetrieveEMTrackIsolationTool()).isFailure()) {
      return sc;
    }
  }

  // for measuring the timing
  m_timingProfile = 0;
  if (m_timing) {
    sc = service("ChronoStatSvc",m_timingProfile);
    if(sc.isFailure() || m_timingProfile == 0) {
      ATH_MSG_ERROR("Cannot find the ChronoStatSvc " << m_timingProfile);
      return sc;
    }
  }

  // lets set up the isolation variables
  
  for (unsigned int i = 0; i < m_isoTypes.size(); i++) {
    m_specs.push_back(getSpecifier(static_cast<xAOD::Iso::IsolationType>(m_isoTypes.at(i))));
  }

    // let's optimize by using just Rs
  for (std::vector<IegammaIso::IsoSpecifier>::const_iterator specsit = m_specs.begin();
	 specsit != m_specs.end();
	 ++specsit) {
      m_Rs.push_back(specsit->R2);
  }
  


  return sc;
}

// ====================================================================
StatusCode EMIsolationBuilder::RetrieveEMCaloIsolationTool()
{
  //
  // retrieve EMCaloIsolationTool
  //
  
  if (m_emCaloIsolationTool.empty()) {
    ATH_MSG_INFO("EMCaloIsolation is empty");
    return StatusCode::SUCCESS;
  } 

  if(m_emCaloIsolationTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_emCaloIsolationTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_emCaloIsolationTool);
  
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMIsolationBuilder::RetrieveEMTrackIsolationTool()
{
  //
  // retrieve EMTrackIsolationTool
  //
  
  if (m_emTrackIsolationTool.empty()) {
    ATH_MSG_INFO("EMTrackIsolation is empty");
    return StatusCode::SUCCESS;
  } 

  if(m_emTrackIsolationTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_emTrackIsolationTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_emTrackIsolationTool);
  return StatusCode::SUCCESS;
}



// ====================================================================
StatusCode EMIsolationBuilder::RetrieveEMCaloIsoPtCorrectionTool()
{
  //
  // retrieve EMCaloIsolationPtCorrectionTool
  //

  if (m_emCaloIsoPtCorrectionTool.empty()) {
    ATH_MSG_INFO("EMCaloIsolationPtCorrection is empty");
    return StatusCode::SUCCESS;
  }

  if(m_emCaloIsoPtCorrectionTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_emCaloIsoPtCorrectionTool);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_emCaloIsoPtCorrectionTool);

  return StatusCode::SUCCESS;
}


// ====================================================================
StatusCode EMIsolationBuilder::RetrieveCaloIsolationTool()
{
  //
  // retrieve DebugCaloIsolationTool
  //

  if (m_CaloIsolationTool.empty()) {
    ATH_MSG_INFO("CaloIsolation is empty");
    m_doTopoIso = false;
    return StatusCode::SUCCESS;
  }

  if(m_CaloIsolationTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_CaloIsolationTool);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_CaloIsolationTool);

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMIsolationBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode EMIsolationBuilder::execute(xAOD::Egamma* eg)
{ 
  // 
  // execute method as used in offline reconstruction
  // 

  ATH_MSG_DEBUG("Executing EMIsolationBuilder::execute");
  
  // protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;

  // retrieve the containers
  StatusCode sc = retrieveContainers();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("EMIsolationBuilder returns failure on retrieveContainers");
    return sc;
  }

  sc = recoExecute(eg,m_cellcoll);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("execute(egamma* eg) returns failure on recoExecute");
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode EMIsolationBuilder::recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll,
					const xAOD::TrackParticleContainer* aTrackParticleContainer /*= 0*/)
{ 
  // 
  // execute method as used by Offline reconstruction
  // 
  
  ATH_MSG_DEBUG("Executing recoExecute");

  // put the CaloCellContainer into the global variable
  m_cellcoll = cellcoll;
   
  //if We do not want to do isolation for topoSeeded 
  //Do just Shower Shapes
  if(!m_doIsolForTopoSeeded){ 
    if(eg->author(xAOD::EgammaParameters::AuthorCaloTopo35)) {
      m_clus = eg->caloCluster(); 
      if (m_clus == 0 ) return StatusCode::SUCCESS; 
    } 
  }
  
  // call calorimeter isolation tool only if needed
  CalcCaloIsolation(eg);
  CalcCaloIsolationPtCorrection(eg);
  
  // calculate track isolation
  CalcFromTracker(eg,aTrackParticleContainer);

  ATH_MSG_DEBUG("Exiting recoExecute");
  
  return StatusCode::SUCCESS;
  
}

// =====================================================================
StatusCode EMIsolationBuilder::retrieveContainers()
{ 
  //
  // method shared by the various execute method to retrieve 
  // the cell and cluster containers
  //

  // retrieve Calo Cell Container
  if ( m_UseCaloIsoTool) {
    StatusCode sc = evtStore()->retrieve(m_cellcoll, m_cellsName) ; 
    if(sc.isFailure() || !m_cellcoll) {
      ATH_MSG_WARNING("no Calo Cell Container " << m_cellsName << " found");
      return sc;
    }
  }

  ATH_MSG_DEBUG("Will retrieve egamma density containers");
  if (evtStore()->contains<xAOD::EventShape>("DensityForEgammaCentral")) {
    m_evtShapeC = evtStore()->retrieve<xAOD::EventShape>("DensityForEgammaCentral") ;
  } else {
    ATH_MSG_WARNING("DensityForEgammaCentral does not exist in evtStore");
    m_evtShapeC = 0;
  }
  if (evtStore()->contains<xAOD::EventShape>("DensityForEgammaForward")) {
    m_evtShapeF = evtStore()->retrieve<xAOD::EventShape>("DensityForEgammaForward") ;
  } else {
    ATH_MSG_WARNING("DensityForEgammaForward does not exist in evtStore");
    m_evtShapeF = 0;
  }

  return StatusCode::SUCCESS;
}


// =====================================================================
void EMIsolationBuilder::CalcFromTracker(xAOD::Egamma* eg,const xAOD::TrackParticleContainer* aTrackParticleContainer /*= 0*/)
{ 
  // 
  // Estimate track isolation from tracker
  // 

  ATH_MSG_DEBUG("Executing CalcFromTracker");

  if (m_timingProfile) {
    std::string chronoName = this->name()+"_CalcFromTracker";         
    m_timingProfile->chronoStart(chronoName);
  }


  // Apply Track Isolation calculation
  if (m_UseTrackIsoTool && !m_isCosmics) {
    
    // protection in case tool is not available
    // return as algorithm may be able to run without it 
    // in degraded mode
    if (m_emTrackIsolationTool.empty()) return;
    
    // execute the tool
    StatusCode sc = m_emTrackIsolationTool->execute(eg,aTrackParticleContainer);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("EMTrackisolation reports failure");
      return;
    }
    
    // retrieve and set value
    float value=0;
    value= static_cast<float>(m_emTrackIsolationTool->ptcone20());
    eg->setIsolationValue(value, xAOD::Iso::ptcone20);
    value= static_cast<float>(m_emTrackIsolationTool->ptcone30());
    eg->setIsolationValue(value, xAOD::Iso::ptcone30);
    value= static_cast<float>(m_emTrackIsolationTool->ptcone40());
    eg->setIsolationValue(value, xAOD::Iso::ptcone40);
    value= static_cast<float>(m_emTrackIsolationTool->nucone20());
    eg->setIsolationValue(value, xAOD::Iso::nucone20);
    value= static_cast<float>(m_emTrackIsolationTool->nucone30());
    eg->setIsolationValue(value, xAOD::Iso::nucone30);
    value= static_cast<float>(m_emTrackIsolationTool->nucone40());
    eg->setIsolationValue(value, xAOD::Iso::nucone40);
  }
    
  
  if (m_timingProfile) {
    std::string chronoName = this->name()+"_CalcFromTracker";         
    m_timingProfile->chronoStop(chronoName);
  }
  return;
}

// ==========================================================================
void EMIsolationBuilder::CalcCaloIsolation(xAOD::Egamma* eg)
{
  //
  // Call calorimeter isolation tool
  //
  
  ATH_MSG_DEBUG("Executing CalcCaloIsolation with doTopoIso = " << m_doTopoIso);
  
  // protection against bad pointers
  if (eg==0) return;
  
  // retrieve the cluster 
  m_clus = eg->caloCluster(); 
  // Protect against non-existent structures.
  if (m_clus == 0) return;  
  
  // call calorimeter isolation tool only if needed
  if (m_UseCaloIsoTool){

    if (m_timingProfile) {
      std::string chronoName = this->name()+"_CalcCaloIsolation";         
      m_timingProfile->chronoStart(chronoName);
    }
    
    xAOD::CaloIsolation CaloIsolationResult;
    std::vector<xAOD::Iso::IsolationType> m_topoIsoTypesCast;
    for(unsigned int i=0;i<m_topoIsoTypes.size();i++){
      m_topoIsoTypesCast.push_back(static_cast<xAOD::Iso::IsolationType>(m_topoIsoTypes.at(i)));
    }
    
    // protection in case tool does not exist
    if (m_emCaloIsolationTool.empty()) {
      return;
    }
    if (m_doTopoIso) {

      bool bsc =  m_CaloIsolationTool->caloIsolation( CaloIsolationResult, *eg, m_topoIsoTypesCast, xAOD::ICaloIsolationTool::Eeg57);
      if (!bsc) {
	ATH_MSG_WARNING("Call to CaloIsolationTool failed to execute");
	return;
      }
    }
    
    if ( m_caloNums.size() < 3 ) {
      ATH_MSG_WARNING("Less than 3 subCalos, skipping");
      return;
    }
    
    // define a new Calo Cell list corresponding to EM Calo
    // retrieve the corresponding CaloCell_ID
    CaloCell_ID::SUBCALO theCalo = 
      static_cast<CaloCell_ID::SUBCALO>(m_caloNums[0]) ;
    // define a new Calo Cell list
    CaloCellList* EMccl = new CaloCellList(m_cellcoll,theCalo); 
    
    // define a new Calo Cell list corresponding to HAD Calo
    // retrieve the corresponding CaloCell_ID for Tile
    CaloCell_ID::SUBCALO theCalo1 = 
      static_cast<CaloCell_ID::SUBCALO>(m_caloNums[1]) ;
    // retrieve the corresponding CaloCell_ID for FCAL
    CaloCell_ID::SUBCALO theCalo2 = 
      static_cast<CaloCell_ID::SUBCALO>(m_caloNums[2]) ;
    
    std::vector<CaloCell_ID::SUBCALO> theVecCalo;
    theVecCalo.push_back(theCalo1);
    theVecCalo.push_back(theCalo2);
    // define a new Calo Cell list
    CaloCellList* HADccl = new CaloCellList(m_cellcoll,theVecCalo); 
    
    
    // calculate information concerning isolation behind em clusters in the 
    // hadronic calorimeter and around the em cluster
    // for isolation around em clustr use cone size 0.45
    StatusCode sc =  m_emCaloIsolationTool->execute(m_clus,HADccl,EMccl,m_Rs,5,7); 
    
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("call to Iso returns failure for execute");
      // delete ccls
      delete EMccl;
      delete HADccl;
      return;
    }
    
    // pt correction for topoetcone40 
    m_dotopoptcor = false;
    
    try {
      for (unsigned int i = 0; i < m_isoTypes.size(); i++) {
	float isovalue=static_cast<float>(m_emCaloIsolationTool->etcone(i));
	eg->setIsolationValue(isovalue,static_cast<xAOD::Iso::IsolationType>(m_isoTypes.at(i)));
      }
      if (m_doTopoIso){
	if(CaloIsolationResult.etcones.size()<m_topoIsoTypesCast.size()){
	  ATH_MSG_WARNING("Size of isolation results not correct = " << CaloIsolationResult.etcones.size());
	}
	for (unsigned int i = 0; i < m_topoIsoTypesCast.size(); i++) {
	  if (m_topoIsoTypesCast.at(i) == xAOD::Iso::topoetcone20_core57cells
	      ||m_topoIsoTypesCast.at(i) == xAOD::Iso::topoetcone30_core57cells
	      ||m_topoIsoTypesCast.at(i) == xAOD::Iso::topoetcone40_core57cells ) {
	    m_dotopoptcor = true;
	    float isotopovalue=static_cast<float>(CaloIsolationResult.etcones.at(i));
	    eg->setIsolationValue(isotopovalue,static_cast<xAOD::Iso::IsolationType>(m_topoIsoTypesCast.at(i)));
	    
	  }
	}
      }
    }
    catch(std::out_of_range) {
      ATH_MSG_WARNING("out of range exception caught");
    }
    
    // delete ccls
    delete EMccl;
    delete HADccl;
    
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_CalcCaloIsolation"; 
      m_timingProfile->chronoStop(chronoName);
    }
    
  }
}


// ==========================================================================
void EMIsolationBuilder::CalcCaloIsolationPtCorrection(xAOD::Egamma* eg)
{ 
  //
  // Call calorimeter pt corrected  isolation tool
  //
  ATH_MSG_DEBUG("Executing CalcCaloIsolationPtCorrection with doTopoIso = " << m_doTopoIso);
  
  // protection against bad pointers
  if (eg==0) return;
    
  // retrieve the cluster 
  m_clus = eg->caloCluster(); 
  // Protect against non-existent structures.
  if (m_clus == 0) return;  
  
  // call calorimeter isolation tool only if needed
  if (m_UseCaloIsoTool){
    
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_CalcCaloIsolation";         
      m_timingProfile->chronoStart(chronoName);
    }
    
    float value=0;
    float initialvalue=0;
    if (eg->isolationValue(initialvalue,xAOD::Iso::etcone20)){
      value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.20));
      eg->setIsolationValue(value, xAOD::Iso::etcone20_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::Iso::etcone30)){
      value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.30));
      eg->setIsolationValue(value, xAOD::Iso::etcone30_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::Iso::etcone40)){
      value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.40));
      eg->setIsolationValue(value, xAOD::Iso::etcone40_ptcorrected);
    }
    
    if ( m_dotopoptcor ){
      double rho   = 0.;
      float edcorr = 0.;
      float ptcorr = 0.;
      float dR     = 0.;
      // retrieve rho median for pile-up correction
      const xAOD::EventShape* m_evtShape(0);
      if (fabs(eg->eta()) > 1.5 && m_evtShapeF) {
	m_evtShape = m_evtShapeF;
      } else if (fabs(eg->eta()) < 1.5 && m_evtShapeC) {
	m_evtShape = m_evtShapeC;
      }
      if (m_evtShape) {
	//	bool status = m_evtShape->getDensity(xAOD::EventShape::Density,rho);
	bool status = m_evtShape->getDensity(xAOD::EventShape::EventDensityID::Density,rho);
	if (!status)
	  ATH_MSG_WARNING("EventDensity for egamma not OK");
	else
	  ATH_MSG_DEBUG("Egamma eventDensity = " << rho << " GeV");
      }
      
      if (eg->isolationValue(initialvalue,xAOD::Iso::topoetcone20_core57cells)){
	dR=0.2;
	ptcorr = m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  dR, "topo");
	edcorr = rho*M_PI*(dR*dR-0.875/128.);
	eg->setIsolationValue(static_cast<float>(initialvalue - ptcorr), xAOD::Iso::topoetcone20_core57cells_ptcorrected);
	eg->setIsolationValue(static_cast<float>(initialvalue - ptcorr - edcorr), xAOD::Iso::topoetcone20_core57cells_corrected);
	ATH_MSG_DEBUG("new topo 0.2 " << initialvalue - ptcorr - edcorr);
      }
      if (eg->isolationValue(initialvalue,xAOD::Iso::topoetcone30_core57cells)){
	dR=0.3;
	ptcorr = m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  dR, "topo");
	edcorr = rho*M_PI*(dR*dR-0.875/128.);
	eg->setIsolationValue(static_cast<float>(initialvalue - ptcorr), xAOD::Iso::topoetcone30_core57cells_ptcorrected);
	eg->setIsolationValue(static_cast<float>(initialvalue - ptcorr - edcorr), xAOD::Iso::topoetcone30_core57cells_corrected);
	ATH_MSG_DEBUG("new topo 0.3 " << initialvalue - ptcorr - edcorr);
      }
      if (eg->isolationValue(initialvalue,xAOD::Iso::topoetcone40_core57cells)){
	dR=0.4;
	ptcorr = m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  dR, "topo");
	edcorr = rho*M_PI*(dR*dR-0.875/128.);
	eg->setIsolationValue(static_cast<float>(initialvalue - ptcorr), xAOD::Iso::topoetcone40_core57cells_ptcorrected);
	eg->setIsolationValue(static_cast<float>(initialvalue - ptcorr - edcorr), xAOD::Iso::topoetcone40_core57cells_corrected);
	ATH_MSG_DEBUG("new topo 0.4 " << initialvalue - ptcorr - edcorr);
      }
      
    }
    
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_CalcCaloIsolation"; 
      m_timingProfile->chronoStop(chronoName);
    }  
    
  }
}


// ==========================================================================
IegammaIso::IsoSpecifier EMIsolationBuilder::getSpecifier(xAOD::Iso::IsolationType par) const
{
  switch (par) {
  case xAOD::Iso::etcone20:
    return IegammaIso::IsoSpecifier(0, 0.20, -1);
  case xAOD::Iso::etcone30:
    return IegammaIso::IsoSpecifier(0, 0.30, -1);
  case xAOD::Iso::etcone40:
    return IegammaIso::IsoSpecifier(0, 0.40, -1);

  default:
    ATH_MSG_WARNING("found an unrecognized isolation parameter: " << par);
    return IegammaIso::IsoSpecifier();
  }
}

