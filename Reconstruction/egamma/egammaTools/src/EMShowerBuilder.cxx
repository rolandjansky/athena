/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IChronoStatSvc.h"

#include "EMShowerBuilder.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaInterfaces/IegammaShowerShape.h"
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
EMShowerBuilder::EMShowerBuilder(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : egammaBaseTool(type, name, parent),
    m_ShowerShapeTool("egammaShowerShape/egammashowershape")
{
  //
  // constructor
  //

  // declare interface
  declareInterface<IEMShowerBuilder>(this);
  
  // The following properties are specified at run-time
  // (declared in jobOptions file)
  
  // Names of containers which contain cells  
  declareProperty("CellsName",m_cellsName="AllCalo", "Names of containers which contain cells ");

  // list of calo to treat
  declareProperty("CaloNums",m_caloNums, "list of calo to treat");

  // Boolean to call shower shape calculation and filling
  // (NB: this could be important when redoing calculation from AODs)
  declareProperty("UseShowerShapeTool", m_UseShowerShapeTool=true, "Boolean to call shower shape calculation and filling");
  
  // Boolean to call isolation variables calculation and filling
  // (NB: this could be important when redoing calculation from AODs)
  declareProperty("UseCaloIsoTool", m_UseCaloIsoTool=true, "Boolean to call calo isolation variables calculation and filling");
  
  // Boolean to use track iso tool
  declareProperty("UseTrackIsoTool", m_UseTrackIsoTool= true, "Boolean to call track isolation variables calculation and filling");

  // Handles of instance of egammaShowerShape Tool to be run 
  declareProperty("ShowerShapeTool", m_ShowerShapeTool, "Handle of instance of egammaShowerShape Tool to be run");

  // Handle of the calorimetric isolation tool
  declareProperty("EMCaloIsolationTool", m_emCaloIsolationTool, "Handle of the EMCaloIsolationTool");

  // Handle of the calorimetric isolation tool
  declareProperty("EMCaloIsoPtCorrectionTool", m_emCaloIsoPtCorrectionTool, "Handle of the EMCaloIsoPtCorrectionTool");

  // Handle of the calorimetric isolation tool
  declareProperty("EMTopoCaloIsolationTool", m_emTopoCaloIsolationTool, "Handle of the EMTopoCaloIsolationTool");

  // Handle of the EMTrackIsolationTool
  declareProperty("EMTrackIsolationTool", m_emTrackIsolationTool,"Handle of the EMTrackIsolationTool");

  // Boolean for use of cosmics
  declareProperty("isCosmics",m_isCosmics=false,"Boolean for use of cosmics");

  // Boolean to do isolation for TopoSeededPhoton 
  declareProperty("doIsolForTopoSeeded",m_doIsolForTopoSeeded=true,"Boolean for doing isolation for TopoSeeded Photons");

  // Name of the input Topo Calo container
  declareProperty("TopoCaloClusterInputName",m_topoCaloClusterInputName="CaloCalTopoCluster","Topo CaloCluster input conainer to compute the isolation");
  
  // in case we want some extra print
  declareProperty("Print",m_Print=false,"in case of extra prints");

  // in case we want some extra print
  declareProperty("Timing",m_timing=false,"do extra timing");
      
      
  declareProperty("IsoTypes", m_isoTypes, "The isolation types to do: vector of enum type egammaParameter::ParamDef");
      
  declareProperty("TopoIsoTypes", m_topoIsoTypes,"The topo isolation types to do: vector of enum type egammaParameter::ParamDef");
      
  declareProperty("doEnergyDensityCorrection", m_doEnergyDensityCorrection=true,"Correct isolation variables based on energy density estimations");

 }

// ===============================================================
EMShowerBuilder::~EMShowerBuilder()
{
  //
  // destructor
  // 
}

// ==================================================================
StatusCode EMShowerBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing EMShowerBuilder");;
 
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
      if ((sc = RetrieveEMTopoCaloIsolationTool()).isFailure()) {
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
  m_doNoiseCalc = false; // initialize to false

  for (unsigned int i = 0; i < m_isoTypes.size(); i++) {
    m_specs.push_back(getSpecifier(static_cast<xAOD::EgammaParameters::IsolationType>(m_isoTypes.at(i))));
  }

  if (!m_doNoiseCalc) {
    // let's optimize by using just Rs
    for (std::vector<IegammaIso::IsoSpecifier>::const_iterator specsit = m_specs.begin();
	 specsit != m_specs.end();
	 ++specsit) {
      m_Rs.push_back(specsit->R2);
    }

  }

  return sc;
}

// ====================================================================
StatusCode EMShowerBuilder::RetrieveShowerShapeTool()
{
  //
  // retrieve shower shape tool
  //

  if (m_ShowerShapeTool.empty()) {
    ATH_MSG_INFO("ShowerShape is empty");
    return StatusCode::SUCCESS;
  } 

  // retrieve egammaShowerShape Tool
  if(m_ShowerShapeTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_ShowerShapeTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool " << m_ShowerShapeTool); 
  return StatusCode::SUCCESS;
}


// ====================================================================
StatusCode EMShowerBuilder::RetrieveEMCaloIsolationTool()
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
StatusCode EMShowerBuilder::RetrieveEMTrackIsolationTool()
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
StatusCode EMShowerBuilder::RetrieveEMCaloIsoPtCorrectionTool()
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
StatusCode EMShowerBuilder::RetrieveEMTopoCaloIsolationTool()
{
  //
  // retrieve EMTopoCaloIsolationTool
  //

  if (m_emTopoCaloIsolationTool.empty()) {
    ATH_MSG_INFO("EMTopoCaloIsolation is empty");
    m_doTopoIso = false;
    return StatusCode::SUCCESS;
  }

  if(m_emTopoCaloIsolationTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_emTopoCaloIsolationTool);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_emTopoCaloIsolationTool);

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMShowerBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode EMShowerBuilder::execute(xAOD::Egamma* eg)
{ 
  // 
  // execute method as used in offline reconstruction
  // 

  ATH_MSG_DEBUG("Executing EMShowerBuilder::execute");
  
  // protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;

  // retrieve the containers
  StatusCode sc = retrieveContainers();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("EMShowerBuilder returns failure on retrieveContainers");
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
StatusCode EMShowerBuilder::caloExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll)
{ 
  // 
  // execute method as used by Event Filter
  // 
  
  ATH_MSG_DEBUG("Executing caloExecute");

  // put the CaloCellContainer into the global variable
  m_cellcoll = cellcoll;

  // calculate shower shapes
  CalcShowerShape(eg);

  return StatusCode::SUCCESS;

}

StatusCode EMShowerBuilder::recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll,
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
    
      if (m_ShowerShapeTool.empty())  return StatusCode::SUCCESS; 
    
      StatusCode sc  = m_ShowerShapeTool->execute(m_clus,m_cellcoll);
      if ( sc.isFailure() ) {
	ATH_MSG_WARNING("call to ShowerShape returns failure ");
	return sc; 
      } 
      else {
	FillEMShowerShape(eg);
	return StatusCode::SUCCESS; 
      }
    }
  }
  
   
  // calculate shower shapes
  CalcShowerShape(eg);
  // calculate track isolation
  CalcFromTracker(eg,aTrackParticleContainer);

  ATH_MSG_DEBUG("Exiting recoExecute");
  
  return StatusCode::SUCCESS;
  
}

// =====================================================================
StatusCode EMShowerBuilder::retrieveContainers()
{ 
  //
  // method shared by the various execute method to retrieve 
  // the cell and cluster containers
  //

  // retrieve Calo Cell Container
  if (m_UseShowerShapeTool || m_UseCaloIsoTool) {
    StatusCode sc = evtStore()->retrieve(m_cellcoll, m_cellsName) ; 
    if(sc.isFailure() || !m_cellcoll) {
      ATH_MSG_WARNING("no Calo Cell Container " << m_cellsName << " found");
      return sc;
    }
  }

  //SL+JBdV
  ATH_MSG_DEBUG("Will retrieve EMTopoEventShape");
  if (evtStore()->contains<xAOD::EventShape>("EMTopoEventShape")) {
    m_evtShape = evtStore()->retrieve<xAOD::EventShape>("EMTopoEventShape") ;
  } else {
    ATH_MSG_WARNING("EMTopoEventShape does not exist in evtStore");
    m_evtShape = 0;
  }

  return StatusCode::SUCCESS;
}

// =====================================================================
void EMShowerBuilder::CalcShowerShape(xAOD::Egamma* eg)
{ 
  // 
  // Estimate shower shapes and fill the EMShower object associated to eg
  // 

  ATH_MSG_DEBUG("Executing CalcShowerShape");

  // protection against bad pointers
  if (eg==0) return;

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve the cluster 
  m_clus = eg->caloCluster(); 
  // Protect against non-existent structures.
  if (m_clus == 0) return;  

  // call calorimeter isolation tool only if needed
  if (m_UseCaloIsoTool){
  
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_CalcCaloIsolation";         
      m_timingProfile->chronoStart(chronoName);
      CalcCaloIsolation(eg);
      CalcCaloIsolationPtCorrection(eg);
      m_timingProfile->chronoStop(chronoName);
    } else {
      CalcCaloIsolation(eg);
      CalcCaloIsolationPtCorrection(eg);
    }
  } 

  // Calculate shower shapes in all samplings
  if (m_UseShowerShapeTool) {
    // protection in case tool does not exist 
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_ShowerShapeTool";         
      m_timingProfile->chronoStart(chronoName);
    }
    if (!m_ShowerShapeTool.empty()) {
      sc = m_ShowerShapeTool->execute(m_clus,m_cellcoll);
      if ( sc.isFailure() ) {
	ATH_MSG_WARNING("call to ShowerShape returns failure ");
      } else {
	FillEMShowerShape(eg);
      }
    }
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_ShowerShapeTool";         
      m_timingProfile->chronoStop(chronoName);
    }
  }
}

// =====================================================================
void EMShowerBuilder::CalcFromTracker(xAOD::Egamma* eg,const xAOD::TrackParticleContainer* aTrackParticleContainer /*= 0*/)
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
    eg->setIsolationValue(value, xAOD::EgammaParameters::ptcone20);
    value= static_cast<float>(m_emTrackIsolationTool->ptcone30());
    eg->setIsolationValue(value, xAOD::EgammaParameters::ptcone30);
    value= static_cast<float>(m_emTrackIsolationTool->ptcone40());
    eg->setIsolationValue(value, xAOD::EgammaParameters::ptcone40);
    value= static_cast<float>(m_emTrackIsolationTool->nucone20());
    eg->setIsolationValue(value, xAOD::EgammaParameters::nucone20);
    value= static_cast<float>(m_emTrackIsolationTool->nucone30());
    eg->setIsolationValue(value, xAOD::EgammaParameters::nucone30);
    value= static_cast<float>(m_emTrackIsolationTool->nucone40());
    eg->setIsolationValue(value, xAOD::EgammaParameters::nucone40);
  }
    
  
  if (m_timingProfile) {
    std::string chronoName = this->name()+"_CalcFromTracker";         
    m_timingProfile->chronoStop(chronoName);
  }
  return;
}

// ==========================================================================
void EMShowerBuilder::CalcCaloIsolation(xAOD::Egamma* eg)
{
  //
  // Call calorimeter isolation tool
  //

  ATH_MSG_DEBUG("Executing CalcCaloIsolation");

  // protection in case tool does not exist
  if (m_emCaloIsolationTool.empty()) {
    return;
  }
  if (m_doTopoIso) {


    // get topo calo container
    const xAOD::CaloClusterContainer* calocontainer = 0;
    StatusCode sc = evtStore()->retrieve(calocontainer, m_topoCaloClusterInputName);
    if ( sc.isFailure() || calocontainer==0 ) {
      ATH_MSG_WARNING("CaloClusters not found" );
      return;
    }
    
    // topo cluster isolation
    sc = m_emTopoCaloIsolationTool->execute(eg, calocontainer, m_topoIsoTypes);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("call to TopoIso returns failure for execute");
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
  
  float value=0;
  value=static_cast<float>(m_emCaloIsolationTool->ethad1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ethad1);

  value=static_cast<float>(m_emCaloIsolationTool->ethad());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ethad);

  value=static_cast<float>(m_emCaloIsolationTool->ehad1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ehad1);

  
  // pt correction for topoetcone40 
  m_dotopoptcor = false;

  try {
    for (unsigned int i = 0; i < m_isoTypes.size(); i++) {
      float isovalue=static_cast<float>(m_emCaloIsolationTool->etcone(i));
      eg->setIsolationValue(isovalue,static_cast<xAOD::EgammaParameters::IsolationType>(m_isoTypes.at(i)));
    }
    if (m_doTopoIso) {
      for (unsigned int i = 0; i < m_topoIsoTypes.size(); i++) {
        if (m_topoIsoTypes.at(i) == xAOD::EgammaParameters::topoetcone40) {m_dotopoptcor = true;}
	float isotopovalue=static_cast<float>(m_emTopoCaloIsolationTool->topoetcone(i));
	eg->setIsolationValue(isotopovalue,static_cast<xAOD::EgammaParameters::IsolationType>(m_topoIsoTypes.at(i)));	
      }
    }
  }
  catch(std::out_of_range) {
    ATH_MSG_WARNING("out of range exception caught");
  }

  // delete ccls
  delete EMccl;
  delete HADccl;
}

// ==========================================================================
void EMShowerBuilder::CalcCaloIsolationPtCorrection(xAOD::Egamma* eg)
{ 
  //
  // Call calorimeter pt corrected  isolation tool
  //

    float value=0;
    float initialvalue=0;
    if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::etcone15)){
    value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.15));
    eg->setIsolationValue(value, xAOD::EgammaParameters::etcone15_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::etcone20)){
    value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.20));
    eg->setIsolationValue(value, xAOD::EgammaParameters::etcone20_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::etcone25)){
    value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.25));
    eg->setIsolationValue(value, xAOD::EgammaParameters::etcone25_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::etcone30)){
    value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.30));
    eg->setIsolationValue(value, xAOD::EgammaParameters::etcone30_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::etcone35)){
    value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.35));
    eg->setIsolationValue(value, xAOD::EgammaParameters::etcone35_ptcorrected);
    }
    if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::etcone40)){
    value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.40));
    eg->setIsolationValue(value, xAOD::EgammaParameters::etcone40_ptcorrected);
    }

    if ( m_dotopoptcor ){
      if (eg->isolationValue(initialvalue,xAOD::EgammaParameters::topoetcone40)){
	value= static_cast<float>(initialvalue - m_emCaloIsoPtCorrectionTool->CalcPtCorrection(eg,  0.40, "topo"));
	eg->setIsolationValue(value, xAOD::EgammaParameters::topoetcone40_ptcorrected);

	// SL+JBdV
	float toSub = 0.;
	double rho  = 0.;
	if (m_evtShape) {
	  xAOD::EventShape::EventDensityID densityType = xAOD::EventShape::EventDensityID::DensityForEgammaCentral;
	  ATH_MSG_DEBUG("Will get density");
	  if (fabs(eg->eta())>1.5) 
	    densityType = xAOD::EventShape::EventDensityID::DensityForEgammaForward;
	  bool status = m_evtShape->getDensity(densityType,rho);
	  if (!status)
	    ATH_MSG_WARNING("EventDensity not OK");
	}
	toSub = rho*M_PI*(0.16-0.875/128.);
	ATH_MSG_DEBUG("Density  = " << rho << ", to sub = " << toSub);
	float edcor = static_cast<float>(value - toSub);
	eg->setIsolationValue(edcor, xAOD::EgammaParameters::topoetcone40_corrected);

      }	
    }
}

// ==========================================================================
void EMShowerBuilder::FillEMShowerShape(xAOD::Egamma* eg)
{ 
  //
  // retrieve information from shower shape calculation at ESD level
  //

  // protection in case Tool does not exist
  if (m_ShowerShapeTool.empty()) return;

  //
  // information in the presampler
  //
  // E in 1x1 cells in pre sampler

  float value=0;
  // E in 1x1 cells in pre sampler
  value=static_cast<float>(m_ShowerShapeTool->e011());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e011);
  // E in 3x3 cells in pre sampler
  value=static_cast<float>(m_ShowerShapeTool->e033());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e033);
  // E in 3x2 cells in S1
  value=static_cast<float>(m_ShowerShapeTool->e132());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e132);
  // E in 15x2 cells in S1 
  value=static_cast<float>(m_ShowerShapeTool->e1152());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e1152);
  // fraction of E in S1
  value=static_cast<float>(m_ShowerShapeTool->f1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::f1);
  // fraction of E in the core(e132) in S1
  value=static_cast<float>(m_ShowerShapeTool->f1core());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::f1core);
  // corr width with 3 strips 
  value=static_cast<float>(m_ShowerShapeTool->width3c());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::weta1);
  // energy in second max
  value=static_cast<float>(m_ShowerShapeTool->esec());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e2ts1);
  // energy strip of second max
  value=static_cast<float>(m_ShowerShapeTool->esec1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e2tsts1);
  // fraction of E outside core in S1
  value=static_cast<float>(m_ShowerShapeTool->fracm());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::fracs1);
  // width with 5 strips
  value=static_cast<float>(m_ShowerShapeTool->width5());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::widths1);
  //eta pos within cell in S1
  value=static_cast<float>(m_ShowerShapeTool->poscs1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::poscs1);
  // asymmetry with 3 strips
  value=static_cast<float>(m_ShowerShapeTool->asy3());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::asy1);
  // diff position +/- 1 cells
  value=static_cast<float>(m_ShowerShapeTool->pos());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::pos);
  // diff position +/- 7 cells
  value=static_cast<float>(m_ShowerShapeTool->pos7());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::pos7);
  // E of strip with min E 
  value=static_cast<float>(m_ShowerShapeTool->emin());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::emins1);
  // E of strip with max E
  value=static_cast<float>(m_ShowerShapeTool->emax());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::emaxs1);
  // barycentre in eta in S1
  value=static_cast<float>(m_ShowerShapeTool->eta1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::barys1);
  // total width in strips
  value=static_cast<float>(m_ShowerShapeTool->wtot());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::wtots1);

  //
  // information in the 2nd sampling
  //
  // E in 3x3 cells in S2
  value=static_cast<float>(m_ShowerShapeTool->e233());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e233);
  // E in 3x5 cells in S2
  value=static_cast<float>(m_ShowerShapeTool->e235());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e235);
  // E in 3x7 cells in S2
  value=static_cast<float>(m_ShowerShapeTool->e237());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e237);
  // E in 5x5 cells in S2
  value=static_cast<float>(m_ShowerShapeTool->e255());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e255);
  // E in 7x7 cells in S2
  value=static_cast<float>(m_ShowerShapeTool->e277());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e277);
  // corr width in S2
  value=static_cast<float>(m_ShowerShapeTool->etaw());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::weta2);
  // uncorr width in S2
  value=static_cast<float>(m_ShowerShapeTool->width());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::widths2);
  // position in eta within cell in S2
  value=static_cast<float>(m_ShowerShapeTool->poscs2());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::poscs2);
  //
  // information in the 3rd sampling
  //
  // fraction of E in S3 
  value=static_cast<float>(m_ShowerShapeTool->f3());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::f3);
  // fraction of E in the core (e333) in S3 
  value=static_cast<float>(m_ShowerShapeTool->f3core());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::f3core);
  // E in 3x3 cells in S3
  value=static_cast<float>(m_ShowerShapeTool->e333());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e333);
  // E in 3x5 cells in S3
  value=static_cast<float>(m_ShowerShapeTool->e335());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e335);
  // E in 3x7 cells in S3
  value=static_cast<float>(m_ShowerShapeTool->e337());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e337);
  // E in 7x7 cells in S3
  value=static_cast<float>(m_ShowerShapeTool->e377());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e377);

  //
  // information combining all samplings
  //
  // ratio of energy in 3x3/3x7 cells
  value=static_cast<float>(m_ShowerShapeTool->reta3337_allcalo());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::r33over37allcalo);
  // core energy
  value=static_cast<float>(m_ShowerShapeTool->ecore());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ecore);

  return;
}

// ==========================================================================
IegammaIso::IsoSpecifier EMShowerBuilder::getSpecifier(xAOD::EgammaParameters::IsolationType par) const
{
  switch (par) {
  case xAOD::EgammaParameters::etcone15:
    return IegammaIso::IsoSpecifier(0, 0.15, -1);
  case xAOD::EgammaParameters::etcone20:
    return IegammaIso::IsoSpecifier(0, 0.20, -1);
  case xAOD::EgammaParameters::etcone25:
    return IegammaIso::IsoSpecifier(0, 0.25, -1);
  case xAOD::EgammaParameters::etcone30:
    return IegammaIso::IsoSpecifier(0, 0.30, -1);
  case xAOD::EgammaParameters::etcone35:
    return IegammaIso::IsoSpecifier(0, 0.35, -1);
  case xAOD::EgammaParameters::etcone40:
    return IegammaIso::IsoSpecifier(0, 0.40, -1);
  case xAOD::EgammaParameters::etcone:
    return IegammaIso::IsoSpecifier(0, 0.45, -1);

  default:
    ATH_MSG_WARNING("found an unrecognized isolation parameter: " << par);
    return IegammaIso::IsoSpecifier();
  }
}

