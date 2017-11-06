/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterROI_Selector.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaRec

AUTHORS:  A.Morley,C.A
CREATED:  Nov 2011

PURPOSE:   For each cluster create a new CaloClusterROI object and fills it then in the CaloClusterROI_Collection.
   

********************************************************************/

// INCLUDE HEADER FILES:

#include "InDetCaloClusterROISelector/CaloClusterROI_Selector.h"

#include "InDetRecToolInterfaces/ICaloClusterROI_Builder.h"
//
#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloCellList.h"

#include "xAODCaloEvent/CaloClusterContainer.h"


//Cluster cuts
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IegammaIso.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"



// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
    
InDet::CaloClusterROI_Selector::CaloClusterROI_Selector(const std::string& name, 
             ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_AllClusters(0),
  m_SelectedClusters(0),
  m_timingProfile(0)
{
  // The following properties are specified at run-time
  // (declared in jobOptions file)
  // input cluster for egamma objects
  declareProperty("InputClusterContainerName",
                  m_inputClusterContainerName="LArClusterEM",
                  "Input cluster for egamma objects");
  
  //Cell container
  declareProperty("CellsName",                              
                  m_cellsName="AllCalo",      
                  "Names of containers which contain cells ");

  // input cluster for egamma objects
  declareProperty("OutputClusterContainerName",
                  m_outputClusterContainerName="CaloClusterROIs",
                  "Output cluster for egamma objects");
  //
  // Handles of tools
  //Hadronic Isolation
  declareProperty("EMCaloIsolationTool",                    m_emCaloIsolationTool, "Handle of the EMCaloIsolationTool");
  //Check Fraction 
  declareProperty("egammaCheckEnergyDepositTool",           m_egammaCheckEnergyDepositTool, "Handle of the egammaCheckEnergyDepositTool");
  //
  declareProperty("CaloClusterROIBuilder",                  m_caloClusterROI_Builder,"Handle of the CaloClusterROI_Builder Tool");
  //
  // Other properties.
  //
  declareProperty("CheckEMSamples",                         m_CheckEMsamples =true);
  declareProperty("CheckHadronicEnergy",                    m_CheckHadronicEnergy=true);
  declareProperty("CheckReta",                              m_CheckReta=true);
  //
  declareProperty("HadRatioCut",                            m_HadRatioCut  =0.12,    " Cut on Hadronic Leakage");
  declareProperty("RetaCut",                                m_RetaCut      =0.65,   " Cut on Reta");
  declareProperty("ClusterEtCut",                           m_ClusterEtCut = 0000,  " Cut On Cluster Et");
  

}

// ================================================================
InDet::CaloClusterROI_Selector::~CaloClusterROI_Selector()
{  
  //
  // destructor
  //
}

// =================================================================
StatusCode InDet::CaloClusterROI_Selector::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG("Initializing CaloClusterROI_Selector");

  /*Get the check Energy Deposit tool*/
  if(m_egammaCheckEnergyDepositTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_egammaCheckEnergyDepositTool);
    return StatusCode::FAILURE;
  }

  /*Get the Hadronic iso tool*/
  if(m_CheckHadronicEnergy){
    if(m_emCaloIsolationTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_emCaloIsolationTool);
      return StatusCode::FAILURE;
    }
  }
  /* Get the middle shapes Tool*/
  if(m_CheckReta){
    // Create egammaMiddleShape Tool
    std::string egammaMiddleShapeTool_name="egammaMiddleShape/Roiegammamiddleshape";
    m_egammaMiddleShape=ToolHandle<IegammaMiddleShape>(egammaMiddleShapeTool_name);
    // a priori this is not useful
    if(m_egammaMiddleShape.retrieve().isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve "<<m_egammaMiddleShape);
      return StatusCode::SUCCESS;
    } 
    else ATH_MSG_DEBUG("Tool " << m_egammaMiddleShape << " retrieved");  
  }
  
  if(m_caloClusterROI_Builder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<< m_caloClusterROI_Builder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<< m_caloClusterROI_Builder); 


  m_timingProfile = 0;
  StatusCode sc = service("ChronoStatSvc",m_timingProfile);
  if(sc.isFailure() || m_timingProfile == 0) {
    ATH_MSG_ERROR("Cannot find the ChronoStatSvc " << m_timingProfile);
  }

  m_AllClusters=0;
  m_SelectedClusters=0;

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}



// ====================================================================
StatusCode InDet::CaloClusterROI_Selector::finalize()
{
  //
  // finalize method
  //
  ATH_MSG_INFO ("AllClusters " << m_AllClusters);
  ATH_MSG_INFO ("SelectedClusters " << m_SelectedClusters);

  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode InDet::CaloClusterROI_Selector::execute()
{
  //
  // athena execute method
  //
  
  //bool do_trackMatch = true;
  ATH_MSG_DEBUG("Executing CaloClusterROI_Selector");

  StatusCode sc;
  // Chrono name for each Tool
  std::string chronoName;

  // Record output CaloClusterROICollection:
  CaloClusterROI_Collection* ccROI_Collection =  new CaloClusterROI_Collection();
  
  sc = evtStore()->record( ccROI_Collection, m_outputClusterContainerName );
  if (sc.isFailure()) 
  {
    ATH_MSG_ERROR("Could not record "<< m_outputClusterContainerName <<" object.");
    return StatusCode::FAILURE;
  }
    
  // retrieve cluster containers, return `failure' if not existing
  const xAOD::CaloClusterContainer* inputClusterContainer;
  if(  evtStore()->contains<xAOD::CaloClusterContainer>(m_inputClusterContainerName)) {  
    sc = evtStore()->retrieve(inputClusterContainer,  m_inputClusterContainerName);
    if( sc.isFailure() ) {
      ATH_MSG_ERROR("Input Cluster not retrived but found " << m_inputClusterContainerName);
      ATH_MSG_DEBUG("Locking ROI container  and returning");
      evtStore()->setConst(ccROI_Collection).ignore();
      return StatusCode::SUCCESS;
    }
  } else { 
    ATH_MSG_INFO("No input Cluster container found " << m_inputClusterContainerName);
    ATH_MSG_DEBUG("Locking ROI container  and returning");
    evtStore()->setConst(ccROI_Collection).ignore();
    return StatusCode::SUCCESS;
  }
  
  
  // retrieve Calo Cell Container
  const CaloCellContainer* cellcoll(0);
  if(m_CheckHadronicEnergy|| m_CheckReta){
    if(  evtStore()->contains<CaloCellContainer>(m_cellsName) ){  
      StatusCode sc = evtStore()->retrieve(cellcoll, m_cellsName) ; 
      if(sc.isFailure() || !cellcoll) {
        ATH_MSG_WARNING("no Calo Cell Container " << m_cellsName << " found");
        return sc;
      } 
    } else {
      ATH_MSG_DEBUG("No input Cell container found " << m_cellsName << ". Will not apply shape cuts");
      ATH_MSG_DEBUG("Locking ROI container  and returning");
      //    evtStore()->setConst(ccROI_Collection).ignore();
      //    return StatusCode::SUCCESS;
      m_CheckHadronicEnergy =  false;
      m_CheckReta = false;
      
    }
  }

  // loop over clusters in the default inputClusterContainer:
  typedef xAOD::CaloClusterContainer::const_iterator clus_iterator;
  // loop over clusters 
  for(clus_iterator iter = inputClusterContainer->begin();
                    iter != inputClusterContainer->end(); 
                    ++iter) 
  {
    m_AllClusters++;
    const xAOD::CaloCluster* cluster = *iter;    
    if (PassClusterSelection(cluster , cellcoll))
    {
      m_SelectedClusters++;
      ATH_MSG_DEBUG("Pass cluster selection");
      Trk::CaloClusterROI* ccROI = m_caloClusterROI_Builder->buildClusterROI( cluster );  
      ccROI_Collection->push_back(ccROI);   
    } else {
     ATH_MSG_DEBUG("Fail cluster selection");
    }
    // reset sc to success for each cluster
    sc = StatusCode::SUCCESS;
  } // end loop over all Calorimeter clusters 
 
  // May be left unchecked if we exited the loop via a continue.
  sc.ignore();
  // lock the egamma collection
  evtStore()->setConst(ccROI_Collection).ignore();
  ATH_MSG_DEBUG("execute completed successfully");
  return StatusCode::SUCCESS;
}
// ======================================================================
bool InDet::CaloClusterROI_Selector::PassClusterSelection(const xAOD::CaloCluster* cluster ,  const CaloCellContainer* cellcoll)
{
 
  if( m_CheckEMsamples && !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster( cluster ) ) {
    ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
    return false;
  }
  
  if((m_CheckHadronicEnergy || m_CheckReta) && cellcoll==0) {
    ATH_MSG_DEBUG("No cell collection: dont make ROI");
    return false;
  }
    
  // transverse energy in calorimeter (using eta position in second sampling)
  double eta2   = fabs(cluster->etaBE(2));  
  if (eta2>2.47 || eta2<0.) {
    ATH_MSG_DEBUG("Cluster failed acceptance test: dont make ROI");
    return false;
  }

  double et = cosh(eta2)!=0. ? cluster->e()/cosh(eta2) : 0.;

  if ( et < m_ClusterEtCut ){
    ATH_MSG_DEBUG("Cluster failed Energy Cut: dont make ROI");
    return false;
  }
    
  
  if(m_CheckReta || m_CheckHadronicEnergy){

    if(m_CheckReta){
      StatusCode sc = m_egammaMiddleShape->execute(cluster,cellcoll);
      if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Middle shape returns failure for execute");
        return false;
      }
      double e237   = m_egammaMiddleShape->e237(); 
      // E(7*7) in 2nd sampling
      double e277   = m_egammaMiddleShape->e277(); 
      if (e277 != 0. && e237/e277<=m_RetaCut){
        ATH_MSG_DEBUG("Cluster failed Reta test: dont make ROI");
        return false;
      }
    }
    if(m_CheckHadronicEnergy){
      // define a new Calo Cell list corresponding to HAD Calo
      CaloCell_ID::SUBCALO theCalo1 =  static_cast<CaloCell_ID::SUBCALO>(CaloCell_ID::LARHEC) ;
      CaloCell_ID::SUBCALO theCalo2 =  static_cast<CaloCell_ID::SUBCALO>(CaloCell_ID::TILE) ;
      std::vector<CaloCell_ID::SUBCALO> theVecCalo;
      theVecCalo.push_back(theCalo1);
      theVecCalo.push_back(theCalo2);
      // define a new Calo Cell list
      CaloCellList* HADccl = new CaloCellList(cellcoll,theVecCalo); 
      StatusCode sc = m_emCaloIsolationTool->execute(cluster,HADccl); 
      if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Iso returns failure for execute");
        delete HADccl;
        return false;
      }
      delete HADccl;
      double ethad1 = m_emCaloIsolationTool->ethad1();
      double ethad  = m_emCaloIsolationTool->ethad();
      double raphad1 = fabs(et) > 0. ? ethad1/et : 0.;
      double raphad = fabs(et) > 0. ? ethad/et : 0.;
      if (eta2 >= 0.8 && eta2 < 1.37){
        if (raphad >m_HadRatioCut){
          ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
          return false;
        }
			}
      else if(raphad1 >m_HadRatioCut){
        ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
        return false;
      }
    }
  }
  return true;
}
