/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IChronoStatSvc.h"
#include "StoreGate/ReadHandle.h"

#include "EMShowerBuilder.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaInterfaces/IegammaIso.h"

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
    m_cellcoll(0),
    m_clus(0),
    m_caloSelection(false),
    m_timingProfile(0)
{
  //
  // constructor
  //

  // declare interface
  declareInterface<IEMShowerBuilder>(this);
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

  ATH_MSG_DEBUG(" Initializing EMShowerBuilder, m_cellKey = " << m_cellsKey.key());
 
  ATH_CHECK(m_cellsKey.initialize((m_UseShowerShapeTool || m_UseCaloIsoTool) && 
				  m_cellsKey.key() != ""));

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

  StatusCode sc = StatusCode::SUCCESS;
  //Retrieve shower shape Tool 
  if (m_UseShowerShapeTool) { 
    if ((sc = RetrieveShowerShapeTool()).isFailure()) { 
      return sc; 
    } else {
      m_ShowerShapeTool.disable();
    }
  }
  // 
  // call calorimeter isolation tool only if needed
  //
  if (m_UseCaloIsoTool) {
    if ((sc = RetrieveHadronicLeakageTool()).isFailure()) {
      return sc;
    } else {
      m_HadronicLeakageTool.disable();
    }
  }
  // for measuring the timing
  if (m_timing) {
    sc = service("ChronoStatSvc",m_timingProfile);
    if(sc.isFailure() || m_timingProfile == 0) {
      ATH_MSG_ERROR("Cannot find the ChronoStatSvc " << m_timingProfile);
      return sc;
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
StatusCode EMShowerBuilder::RetrieveHadronicLeakageTool()
{
  //
  // retrieve EMCaloIsolationTool
  //
  
  if (m_HadronicLeakageTool.empty()) {
    ATH_MSG_INFO("HadronicLeakageTool is empty");
    return StatusCode::SUCCESS;
  } 

  if(m_HadronicLeakageTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_HadronicLeakageTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_HadronicLeakageTool);
  
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMShowerBuilder::finalize(){
  //
  // finalize method
  //
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode EMShowerBuilder::execute(xAOD::Egamma* eg){ 
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

StatusCode EMShowerBuilder::recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll)
{ 
  // 
  // execute method as used by online reconstruction
  // 
  
  ATH_MSG_DEBUG("Executing recoExecute");

  // put the CaloCellContainer into the global variable
  m_cellcoll = cellcoll;

  // calculate shower shapes
  CHECK(CalcShowerShape(eg));
  
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
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey);

    // check is only used for serial running; remove when MT scheduler used
    if(!cellcoll.isValid()) {
      ATH_MSG_ERROR("Failed to retrieve cell container: "<< m_cellsKey.key());
      return StatusCode::FAILURE;
    }

    m_cellcoll = cellcoll.cptr();
  } else {
    m_cellcoll = nullptr;
  }

  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode EMShowerBuilder::CalcShowerShape(xAOD::Egamma* eg)
{ 
  // 
  // Estimate shower shapes and fill the EMShower object associated to eg
  // 

  ATH_MSG_DEBUG("Executing CalcShowerShape");

  // protection against bad pointers
  if (eg==0) {return StatusCode::SUCCESS;}

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve the cluster 
  m_clus = eg->caloCluster(); 
  // Protect against non-existent structures.
  if (m_clus == 0) {return StatusCode::SUCCESS; }

  // call calorimeter isolation tool only if needed
  if (m_UseCaloIsoTool){
  
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_CalcHadronicLeakage";         
      m_timingProfile->chronoStart(chronoName);
      CHECK(CalcHadronicLeakage(eg));
      m_timingProfile->chronoStop(chronoName);
    } else {
      CHECK(CalcHadronicLeakage(eg));
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
	CHECK(FillEMShowerShape(eg));
      }
    }
    if (m_timingProfile) {
      std::string chronoName = this->name()+"_ShowerShapeTool";         
      m_timingProfile->chronoStop(chronoName);
    }
  }
  return sc;
}

// ==========================================================================
StatusCode EMShowerBuilder::CalcHadronicLeakage(xAOD::Egamma* eg){
  //
  // Call calorimeter isolation tool
  //
  ATH_MSG_DEBUG("Executing CalcHadronicLeakage");

  // protection in case tool does not exist
  if (m_HadronicLeakageTool.empty()) {
    return StatusCode::SUCCESS;
  }

  if ( m_caloNums.size() < 3 ) {
    ATH_MSG_WARNING("Less than 3 subCalos, skipping");
    return StatusCode::SUCCESS;
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
  
  // calculate information concerning just the hadronic leakage
  StatusCode sc =  m_HadronicLeakageTool->execute(m_clus,HADccl);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("call to Iso returns failure for execute");
    // delete ccls
    delete EMccl;
    delete HADccl;
    return sc;
  }
  
  float value=0;
  /// @brief ethad/et
  const double et = eg->caloCluster()->et();
  
  value=static_cast<float>(m_HadronicLeakageTool->ethad1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ethad1);
  eg->setShowerShapeValue( et != 0. ? value/et : 0., xAOD::EgammaParameters::Rhad1);
  
  value=static_cast<float>(m_HadronicLeakageTool->ethad());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ethad);
  eg->setShowerShapeValue( et != 0. ? value/et : 0., xAOD::EgammaParameters::Rhad);

  value=static_cast<float>(m_HadronicLeakageTool->ehad1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::ehad1);
    
  // delete ccls
  delete EMccl;
  delete HADccl;

  return StatusCode::SUCCESS;

}


// ==========================================================================
StatusCode EMShowerBuilder::FillEMShowerShape(xAOD::Egamma* eg) const { 
  //
  // retrieve information from shower shape calculation at ESD level
  //
  // protection in case Tool does not exist
  if (m_ShowerShapeTool.empty()) {
    return StatusCode::SUCCESS;
  }
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
  //
  // information combining different shower shape
  //
  float valueSecond=0;
  /// @brief  e237/e277
  value=static_cast<float>(m_ShowerShapeTool->e277());
  valueSecond=static_cast<float>(m_ShowerShapeTool->e237());
  eg->setShowerShapeValue( value != 0 ? valueSecond/value : 0., xAOD::EgammaParameters::Reta);
  /// @brief  e233/e237
  value=static_cast<float>(m_ShowerShapeTool->e233());
  valueSecond=static_cast<float>(m_ShowerShapeTool->e237());
  eg->setShowerShapeValue( valueSecond != 0 ? value/valueSecond : 0., xAOD::EgammaParameters::Rphi);
  /// @brief (emaxs1-e2tsts1)/(emaxs1+e2tsts1)
  value=static_cast<float>(m_ShowerShapeTool->emax());
  valueSecond=static_cast<float>(m_ShowerShapeTool->esec1());
  eg->setShowerShapeValue(fabs(value+valueSecond)>0. ? (value-valueSecond)/(value+valueSecond) : 0., xAOD::EgammaParameters::Eratio);

  value=static_cast<float>(m_ShowerShapeTool->emin());
  eg->setShowerShapeValue( (valueSecond-value), xAOD::EgammaParameters::DeltaE);
  return StatusCode::SUCCESS;
}

