/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCaloTopoTowerAlgorithm.cxx
//
// 
//
// ********************************************************************
//
#include <sstream>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "GaudiKernel/Property.h"

//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
//
// from CaloTopoTowerAlgorithm
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloEvent/CaloTopoTowerContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "TrigCaloRec/TrigCaloTopoTowerAlgorithm.h"

#include "CaloEvent/CaloCell2ClusterMap.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigCaloTopoTowerAlgorithm::TrigCaloTopoTowerAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
  , m_caloSelection(false)
  , m_ptools( this )
{
  
  // Tools
  declareProperty("TowerBuilderTools",m_ptools);

  declareProperty("OutputTowerContainerName",    m_newTowerContainerName="TrigCaloTopoTower");

  // Declare configurable properties of the algorithm      
  declareProperty("MinimumCellEnergy",      m_minimumCellEnergy    = -1000000000.0);
  declareProperty("MinimumClusterEnergy",   m_minimumClusterEnergy = -1000000000.0);

  // Noise Tool stuff    

  declareProperty("DefaultNoiseSigma",      m_noiseSigma           = 10.0);
  declareProperty("UseCaloNoiseTool",       m_useNoiseTool         = false);
  declareProperty("UsePileUpNoise",         m_usePileUpNoise       = true);
  declareProperty("CellEnergySignificance", m_cellESignificanceThreshold = -1);

  // Calo from which to use cells

  declareProperty("IncludedCalos",          m_includedCalos);
  declareProperty("useCellWeight",          m_useCellWeights=true);


}

/////////////////////////////////////////////////////////////////////
    // DESTRUCTOR:
    /////////////////////////////////////////////////////////////////////
    //
    TrigCaloTopoTowerAlgorithm::~TrigCaloTopoTowerAlgorithm()
{  }

/////////////////////////////////////////////////////////////////////
    // INITIALIZE:
    // The initialize method will create all the required algorithm objects
    // Note that it is NOT NECESSARY to run the initialize of individual
    // sub-algorithms.  The framework takes care of it.
    /////////////////////////////////////////////////////////////////////
    //

    HLT::ErrorCode TrigCaloTopoTowerAlgorithm::hltInitialize()
{
  ATH_MSG_DEBUG("in initialize()   TrigCaloTopoTowerAlgorithm");

  ///////////////////////

  m_caloIndices.clear();
  for ( unsigned int iCalos=0; iCalos< m_includedCalos.size(); iCalos++ )
    {
      if  ( m_includedCalos[iCalos] == "LAREM" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LAREM);
	  if (msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG<< "LAREM"<< endmsg;
	}
      else if ( m_includedCalos[iCalos] == "LARHEC")
	{
	  m_caloIndices.push_back(CaloCell_ID::LARHEC);
	  if (msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG<< "LARHEC"<< endmsg;
	}
      else if ( m_includedCalos[iCalos] == "LARFCAL" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LARFCAL);
	  if (msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG<< "LARFCAL"<< endmsg;
	}
      else if ( m_includedCalos[iCalos] == "TILE" )
	{
	  m_caloIndices.push_back(CaloCell_ID::TILE);
	  if (msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG<< "TILE"<< endmsg;
	}
    }

  m_caloSelection=false;
  unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO) ;

  if (msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG<< "caloIndices size cut "<<m_caloIndices.size()<<"  "<<nSubCalo<< endmsg;

  if (m_caloIndices.size()>0 && m_caloIndices.size()<nSubCalo) m_caloSelection=true;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Calo selection applied ? " << m_caloSelection << endmsg;

  if (m_caloSelection) {
    if (msgLvl() <= MSG::DEBUG)      msg() << MSG::DEBUG<< "   subcalo selected "<<endmsg;
    for (unsigned int iCalos=0;iCalos< m_includedCalos.size(); iCalos++ ) 
      if (msgLvl() <= MSG::DEBUG)    msg() << MSG::DEBUG << " " << m_includedCalos[iCalos]<<endmsg;
  }



  ////////////////////                                                                                                            
  // Allocate Tools //                                                                                                            
  ////////////////////                                                                                                            

  // check tool names                                                                                                             
  if ( m_ptools.size() == 0 )
    {
      msg() << MSG::ERROR
	    << "no tools given for this algorithm."
	    << endmsg;
      return HLT::BAD_JOB_SETUP;
    }

  // find tools                                                                                                                   

  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator firstITool = m_ptools.begin();
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator lastITool  = m_ptools.end();

  unsigned int toolCtr = 0;
  msg() << MSG::INFO << " " << endmsg;
  msg() << MSG::INFO << "List of tools in execution sequence:"
      << endmsg;
  msg() << MSG::INFO << "------------------------------------"
      << endmsg;

  StatusCode checkOut;
  checkOut = m_ptools.retrieve();
  if ( checkOut.isFailure() )
    {
      msg() << MSG::WARNING << "Cannot retrieve tool array " << m_ptools << endmsg;
      return HLT::BAD_JOB_SETUP;
    }

  for ( ; firstITool != lastITool; firstITool++ )
    {
      toolCtr++;
      msg() << MSG::INFO
          << std::setw(2) << toolCtr << ".) "
          << (*firstITool)->type()
          << "::name() = \042"
          << (*firstITool)->name()
          << "\042"
          << endmsg;
      // reset statistics                                                                                                         
      //m_toolInvoke[(*firstITool)->name()] = 0;
      //m_toolReject[(*firstITool)->name()] = 0;
      //m_toolAccept[(*firstITool)->name()] = 0;

      msg() << MSG::INFO << "------------------------------------"
          << endmsg;
      msg() << MSG::INFO << " " << endmsg;

    } //close iteration over tools  

  return HLT::OK;

}

HLT::ErrorCode TrigCaloTopoTowerAlgorithm::hltFinalize()
{
  
    ATH_MSG_DEBUG("in finalize()");

  return HLT::OK;
}


HLT::ErrorCode TrigCaloTopoTowerAlgorithm::hltExecute(const HLT::TriggerElement* /*inputTE*/,
						HLT::TriggerElement* outputTE)
{

  ATH_MSG_DEBUG("in execute(): outputTE->getId() ="<<outputTE->getId());

  //////////////////////
 //////CaloCluster Container
  const CaloClusterContainer * thisCaloCluster = 0;
  HLT::ErrorCode sc = getFeature(outputTE, thisCaloCluster);
  if(sc!=HLT::OK || !thisCaloCluster) { ATH_MSG_ERROR("Failed to retrieved TopoClusterContainer"); return HLT::NAV_ERROR; }

  
  const CaloTowerContainer* thisTower = 0;
  sc = getFeature(outputTE, thisTower);
  if(sc!=HLT::OK || !thisTower) { ATH_MSG_ERROR("Failed to retrieved TowerContainer"); return HLT::NAV_ERROR; }  
  
  const CaloCellContainer* thisCell = 0;
  sc = getFeature(outputTE, thisCell);
  if(sc!=HLT::OK || !thisCell) { ATH_MSG_ERROR("Failed to retrieved CellContainer"); return HLT::NAV_ERROR; }

  const CaloCell2ClusterMap*  cellToClusterMap = 0;
  sc = getFeature(outputTE, cellToClusterMap);
  if(sc!=HLT::OK || !cellToClusterMap) { ATH_MSG_ERROR("Failed to retrieved Cell2ClusterMap"); return HLT::NAV_ERROR; }

  ///////////////////////////////////////////////////////////////////////////////

  //make new TopoTowerContainer 
  CaloTopoTowerContainer* newTowers = new CaloTopoTowerContainer(thisTower->towerseg()); 

  ////////////////////////////////////////////////////////////////////////////                                      
  //Starting to save variable to CaloTopoTowerContainer                                                             
  //need to sort out .h                                                                                                                

  newTowers->SetTowers(thisTower);
  newTowers->SetClusters(thisCaloCluster);
  newTowers->SetCells(thisCell);
  newTowers->SetCellToClusterMap(cellToClusterMap);

  newTowers->SetMinimumCellEnergy(m_minimumCellEnergy);
  newTowers->SetMinimumClusterEnergy(m_minimumClusterEnergy);
  newTowers->SetUseCellWeights(m_useCellWeights);

  // Noise tool stuff                                                                                               
  newTowers->SetUseNoiseTool(m_useNoiseTool);
  newTowers->SetUsePileUpNoise(m_usePileUpNoise);
  newTowers->SetNoiseSigma(m_noiseSigma);
  newTowers->SetCellESignificanceThreshold(m_cellESignificanceThreshold);

  // List of calorimeters from which to use cells                                                                   
  newTowers->SetCaloIndices(m_caloIndices);
  newTowers->SetCaloSelection(m_caloSelection);

  //                                                                                                                
  //Finished saving variable to CaloTopoTowerContainer                                                              
  
  ///////////////////////////////////////////////////////////////
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::iterator firstITool  = m_ptools.begin();
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::iterator lastITool   = m_ptools.end();
  StatusCode processStatus = StatusCode::SUCCESS;
  //
  // loop stops only when Failure indicated by one of the tools
  //
  
  while ( ! processStatus.isFailure() && firstITool != lastITool )
    {
      //      m_toolInvoke[(*firstITool)->name()]++;
      
      processStatus = (*firstITool)->execute(newTowers);
      
      // if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << << MSG::INFO << "processStatus is: " << processStatus << endmsg;
      
      if ( ! processStatus.isFailure() )
	{
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
					   << (*firstITool)->name()
					   << ": CaloTopoTowerContainer::size() = "
					   << newTowers->size()
					   << endmsg;	  
	  //m_toolAccept[(*firstITool)->name()]++;
	  firstITool++;
	}
      else
	{
	  // some problem - but do not skip event loop!
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
					   <<"problems while or after processing tool \042"
					   << (*firstITool)->name()
					   << "\042 - cross-check CaloTopoTowerContainer::size() = "
					   << newTowers->size()
					   << endmsg;
	  //m_toolReject[(*firstITool)->name()]++;
	  firstITool++;
	}
    }






  ///////////////////////////////////////////////////////////////

  HLT::ErrorCode stat = attachFeature(outputTE, newTowers , m_newTowerContainerName);             

  

  if (stat != HLT::OK) { 
    ATH_MSG_DEBUG("Could not attachFeature ");
    return HLT::NAV_ERROR;
  }  
  else{
    ATH_MSG_DEBUG("Successful attachFeature ");  
  }
  
  StatusCode sc_const = store()->setConst(newTowers);
  if (sc_const.isFailure()) {  ATH_MSG_WARNING("Can not lock the TopoTowerContainer"); }

  return HLT::OK;
}
  
