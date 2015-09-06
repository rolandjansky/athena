/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloUtils/CaloCollectionHelper.h"

#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloTopoTowerContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloRec/CaloTopoTowerAlgorithm.h"

#include <string>
#include <vector>
#include <iomanip>

CaloTopoTowerAlgorithm::CaloTopoTowerAlgorithm(const std::string& name, 
				       ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name,pSvcLocator)
  , m_genericLink(true) 
  , m_ptools( this )
  , m_caloSelection(false)
{
  // tool names
  //declareProperty("TowerBuilderTools",m_toolNames);
  declareProperty("TowerBuilderTools",m_ptools);
  // output data
  //declareProperty("TowerContainerName",m_towerContainerName);
  // linkable
  declareProperty("GenericLinked",m_genericLink);

  /////////////////////////////////////////////
  //Item From CaloTopoTowerAlg
  declareProperty("Cell2ClusterMapName",    m_cellToClusterMapName = "CaloCell2TopoCluster");
  declareProperty("CellContainerName"  ,    m_cellContainerName = "AllCalo");
  declareProperty("ClusterContainerName",    m_clusterName = "CaloTopoCluster");
  declareProperty("InputTowerContainerName" ,    m_towerContainerName = "CmbTower");
  declareProperty("OutputTowerContainerName",    m_newTowerContainerName = "TopoTower");

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
  
  //END Item From CaloTopoTowerAlg 
  ////////////////////////////////////////////////////

}

CaloTopoTowerAlgorithm::~CaloTopoTowerAlgorithm()
{ 

}

////////////////
// Initialize //
////////////////

StatusCode CaloTopoTowerAlgorithm::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

  // message service
  MsgStream log(messageService(),name());


  // tool service
  IToolSvc* myToolSvc;
  StatusCode checkOut = service("ToolSvc",myToolSvc);

  if ( checkOut.isFailure() )
    {
      log << MSG::FATAL
          << "Tool Service not found"
          << endreq;
      return StatusCode::FAILURE;
    }


  // Report some information regarding the noise tool
  if ( m_useNoiseTool && m_usePileUpNoise && (log.level() <= MSG::DEBUG)) {
    log << MSG::DEBUG
         << "Pile-Up Noise from Noise Tool "
         << " is selected! The noise sigma will be the"
         << " quadratic sum of the electronics noise and the pile up!" << endreq;
  }

  m_caloIndices.clear();
  for ( unsigned int iCalos=0; iCalos< m_includedCalos.size(); iCalos++ )
    {
      if  ( m_includedCalos[iCalos] == "LAREM" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LAREM);
	}
      else if ( m_includedCalos[iCalos] == "LARHEC")
	{
	  m_caloIndices.push_back(CaloCell_ID::LARHEC);
	}
      else if ( m_includedCalos[iCalos] == "LARFCAL" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LARFCAL);
	}
      else if ( m_includedCalos[iCalos] == "TILE" )
	{
	  m_caloIndices.push_back(CaloCell_ID::TILE);
	}
    }

  m_caloSelection=false;
  unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO) ;
  if (m_caloIndices.size()>0 && m_caloIndices.size()<nSubCalo) m_caloSelection=true;

  log << MSG::INFO << " Calo selection applied ? " << m_caloSelection << endreq;
  if (m_caloSelection) {
    log << MSG::INFO << "   subcalo selected ";
    for (unsigned int iCalos=0;iCalos< m_includedCalos.size(); iCalos++ ) log << MSG::INFO << " " << m_includedCalos[iCalos];
    log << MSG::INFO << " " << endreq;
  }


  ////////////////////
  // Allocate Tools //
  ////////////////////

  // check tool names
  if ( m_ptools.size() == 0 )
    {
      log << MSG::ERROR
	  << "no tools given for this algorithm."
	  << endreq;
      return StatusCode::FAILURE;
    }

  // find tools
  
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator firstITool = m_ptools.begin();
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator lastITool  = m_ptools.end();

  unsigned int toolCtr = 0;
  log << MSG::INFO << " " << endreq; 
  log << MSG::INFO << "List of tools in execution sequence:" 
      << endreq;
  log << MSG::INFO << "------------------------------------"
      << endreq;

  checkOut = m_ptools.retrieve();
  if ( checkOut.isFailure() ) 
    {
      log << MSG::WARNING << "Cannot retrieve tool array " << m_ptools << endreq;
      return StatusCode::FAILURE;
    }

  for ( ; firstITool != lastITool; firstITool++ )
    {
      toolCtr++;
      /*      log <<  MSG::INFO << "retrieving tool" << endreq;

      if ( checkOut.isFailure() ) {
	log <<  MSG::WARNING << "Cannot retrieve tool at ToolArray[" << toolCtr-1 << "]" << endreq;
	log <<  MSG::WARNING << "This tool won't be used" << endreq;
      }
      else {
	log <<  MSG::INFO << "retrieved tool" << endreq;
	// print the list of tools
	*/


      log << MSG::INFO 
	  << std::setw(2) << toolCtr << ".) "
	  << (*firstITool)->type()
	  << "::name() = \042"
	  << (*firstITool)->name()
	  << "\042"
	  << endreq;
      // reset statistics
      m_toolInvoke[(*firstITool)->name()] = 0;
      m_toolReject[(*firstITool)->name()] = 0;
      m_toolAccept[(*firstITool)->name()] = 0;

      log << MSG::INFO << "------------------------------------"
	  << endreq;
      log << MSG::INFO << " " << endreq;

	/*if ( (*firstITool)->initializeTool().isFailure() ) {

	  log << MSG::WARNING << " Tool failed to initialize" << endreq;
	  }*/
      
    } //close iteration over tools
  return StatusCode::SUCCESS;
}

/////////////
// Execute //
/////////////

StatusCode CaloTopoTowerAlgorithm::execute()
{

  //////////////////////////
  // Re-allocate Services //
  //////////////////////////

  // messaging
  MsgStream log(messageService(),name());

  //timing
  IChronoStatSvc* theTicker = chronoSvc();

  /////////////////////
  // Tool Processing //
  /////////////////////
  /// retrieve existing Tower container
  const CaloTowerContainer* towerContainer=0;
  if ( (evtStore()->retrieve(towerContainer,m_towerContainerName)).isFailure()) {
    log << MSG::WARNING << " cannot retrieve tower container with key " << m_towerContainerName << endreq;
    return StatusCode::SUCCESS;
  }

  /// get CaloCell container from StoreGate
  const CaloCellContainer* theCells =0;
  if ( (evtStore()->retrieve(theCells,m_cellContainerName)).isFailure()) {
    log << MSG::WARNING << " cannot retrieve cell container with key " << m_cellContainerName << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::DEBUG << "CaloTopoTowerAlgorithm::execute " << m_cellContainerName << " size= " << theCells->size() << endreq; 
    
  ///+++ pick up TopoCluster from StoreGate
  const CaloClusterContainer*  clusters = 0;
  if ( (evtStore()->retrieve(clusters,m_clusterName)).isFailure() )
    {
      log  << MSG::WARNING
            << "cannot retrieve CaloClusterContainer with key <"
            << m_clusterName << ">"
            << endreq;
      return StatusCode::SUCCESS;
    }

  ///+++ pick up CaloCell2ClusterMap from StoreGate
  const CaloCell2ClusterMap*  cellToClusterMap = 0;
  if ( (evtStore()->retrieve(cellToClusterMap,m_cellToClusterMapName)).isFailure() ){
    log  << MSG::WARNING
	  << "cannot retrieve CaloCell2ClusterMap with key <"
	  << m_cellToClusterMapName << ">" 
	  << endreq;
    return StatusCode::SUCCESS;
  }
  if (log.level() <= MSG::DEBUG) log  << MSG::DEBUG << "Successfully retrieved CaloCell2ClusterMap <"<< m_cellToClusterMapName << ">"<< endreq;

  CaloTopoTowerContainer* theTowers = new CaloTopoTowerContainer(towerContainer->towerseg());

  ////////////////////////////////////////////////////////////////////////////
  //Starting to save variable to CaloTopoTowerContainer
  //
  
  theTowers->SetTowers(towerContainer);
  theTowers->SetClusters(clusters);
  theTowers->SetCells(theCells);
  theTowers->SetCellToClusterMap(cellToClusterMap);

  theTowers->SetMinimumCellEnergy(m_minimumCellEnergy);
  theTowers->SetMinimumClusterEnergy(m_minimumClusterEnergy);
  theTowers->SetUseCellWeights(m_useCellWeights);

  // Noise tool stuff
  theTowers->SetUseNoiseTool(m_useNoiseTool);
  theTowers->SetUsePileUpNoise(m_usePileUpNoise);
  theTowers->SetNoiseSigma(m_noiseSigma);
  theTowers->SetCellESignificanceThreshold(m_cellESignificanceThreshold);

  // List of calorimeters from which to use cells
  theTowers->SetCaloIndices(m_caloIndices);
  theTowers->SetCaloSelection(m_caloSelection);

  //
  //Finished saving variable to CaloTopoTowerContainer
  ////////////////////////////////////////////////////////////////////////////
  
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator firstITool  = m_ptools.begin();
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator lastITool   = m_ptools.end();
  StatusCode processStatus = StatusCode::SUCCESS;
  //
  // loop stops only when Failure indicated by one of the tools
  //

  log << MSG::DEBUG << "In execute() " << endreq;
  
  while ( ! processStatus.isFailure() && firstITool != lastITool )
    {
      //      CaloTopoTowerBuilderToolBase* pTool = *firstTool;
      //sc = (*firstITool).retrieve();
      //if ( sc.isFailure() ) { log << MSG::INFO << "error retrieving tool " << endreq; }

      //if ( (*firstITool).empty() ) { log << MSG::INFO << "tool is empty " << endreq; }

      //log << MSG::INFO << "tool retrieved, going to start " << endreq;
      m_toolInvoke[(*firstITool)->name()]++;
      if ( theTicker != 0 )
	{
	  //	  log << MSG::INFO << "Chrono start " << endreq;
	  theTicker->chronoStart((*firstITool)->name());
	}
      /*      log << MSG::INFO << "executing tool: " << (*firstITool)->name() << endreq;
      log << MSG::INFO << "this is &(firstITool): " << &(firstITool) << endreq;
      log << MSG::INFO << "this is (*firstITool): " << (*firstITool) << endreq;
      log << MSG::INFO << "this is &(*firstITool): " << &(*firstITool) << endreq;
      log << MSG::INFO << "this is &(*(*firstITool)): " << &(*(*firstITool)) << endreq;
      log << MSG::INFO << "this is theTowers: " << theTowers << endreq;
      */
      
      processStatus = (*firstITool)->execute(theTowers);
      
      // log << MSG::INFO << "processStatus is: " << processStatus << endreq;
      

      if ( theTicker != 0 )
	{
	  //  log << MSG::INFO << "Chrono stop " << endreq;
	  theTicker->chronoStop((*firstITool)->name());
	}
      if ( ! processStatus.isFailure() )
	{
	  log << MSG::DEBUG
	      << (*firstITool)->name()
	      << ": CaloTopoTowerContainer::size() = "
	      << theTowers->size()
	      << endreq;	  
	  m_toolAccept[(*firstITool)->name()]++;
	  firstITool++;
	}
      else
	{
	  // some problem - but do not skip event loop!
	  log << MSG::ERROR
	      << "problems while or after processing tool \042"
	      << (*firstITool)->name()
	      << "\042 - cross-check CaloTopoTowerContainer::size() = "
	      << theTowers->size()
	      << endreq;
	  m_toolReject[(*firstITool)->name()]++;
	  firstITool++;
	}
    }




  ///////////////////
  // Record Output //
  ///////////////////
     
  // record towers
    //  if ( m_genericLink )
    //    { 
    //      INavigable4MomentumCollection* theNav4Coll = 
    //	new INavigable4MomentumCollection(SG::VIEW_ELEMENTS);
    //      return 
    //	CaloCollectionHelper::
    //	recordStorable<CaloTopoTowerContainer,
    //	INavigable4MomentumCollection>(evtStore(),theTowers,
    //				       m_newTowerContainerName,theNav4Coll);
    //    }

  if ( (evtStore()->record(theTowers,m_newTowerContainerName)).isSuccess() )
    {
      const INavigable4MomentumCollection* theNav4Coll = 0;
      return (evtStore()->setConst(theTowers)).isSuccess()
	? evtStore()->symLink(theTowers,theNav4Coll)
	: StatusCode::FAILURE;
    }
  else
    {
      return StatusCode::FAILURE;
    }
  //  return 
  //    CaloCollectionHelper::
  //    recordStorable<CaloTopoTowerContainer>(evtStore(),theTowers,
  //				       m_newTowerContainerName);
}

//////////////
// Finalize //
//////////////

StatusCode CaloTopoTowerAlgorithm::finalize()
{
  //////////////////////////
  // Re-allocate Services //
  //////////////////////////

  MsgStream log(messageService(),name());

  ////////////////
  // Tool Stats //
  ////////////////
  
  log << MSG::INFO << " " << endreq;
  log << MSG::INFO
      << "Summary of Tool invocation: (invoked/success/failure)"
      << endreq;
  log << "---------------------------"
      << endreq;

  tool_stats_iterator firstName = m_toolInvoke.begin();
  tool_stats_iterator lastName  = m_toolInvoke.end();
  unsigned int toolCtr = 0;
  for ( ; firstName != lastName; firstName++ )
    {
      toolCtr++;
      log << MSG::INFO
	  << std::setw(2) << toolCtr << ".) "
	  << std::setw(36) //<< std::setfill(".") 
	  << (*firstName).first
	  << " ("
	  << (*firstName).second
	  << "/"
	  << m_toolAccept[(*firstName).first]
	  << "/"
	  << m_toolReject[(*firstName).first]
	  << ")"
	  << endreq;
    }
  log << MSG::INFO << "---------------------------"
      << endreq;
  log << MSG::INFO << " " << endreq;
 
  return StatusCode::SUCCESS;
}
