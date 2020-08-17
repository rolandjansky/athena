/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
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
#include "CaloTopoTowerAlgorithm.h"

#include <string>
#include <vector>
#include <iomanip>

CaloTopoTowerAlgorithm::CaloTopoTowerAlgorithm(const std::string& name, 
				       ISvcLocator* pSvcLocator) 
  : AthReentrantAlgorithm(name,pSvcLocator)
  , m_genericLink(true) 
  , m_ptools( this )
  , m_cellContainerKey("AllCalo")
  , m_clusterKey("CaloTopoCluster")
  , m_cellToClusterMapKey("CaloCell2TopoCluster")
  , m_towerContainerKey("CmbTower")
  , m_newTowerContainerKey("TopoTower")
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
  declareProperty("Cell2ClusterMapName",    m_cellToClusterMapKey);
  declareProperty("CellContainerName"  ,    m_cellContainerKey);
  declareProperty("ClusterContainerName",   m_clusterKey);
  declareProperty("InputTowerContainerName" ,    m_towerContainerKey);
  declareProperty("OutputTowerContainerName",    m_newTowerContainerKey);

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

  // tool service
  IToolSvc* myToolSvc = nullptr;
  ATH_CHECK( service("ToolSvc",myToolSvc) );

  ATH_CHECK(m_cellContainerKey.initialize());
  ATH_CHECK(m_clusterKey.initialize());
  ATH_CHECK(m_cellToClusterMapKey.initialize());
  ATH_CHECK(m_towerContainerKey.initialize());
  ATH_CHECK(m_newTowerContainerKey.initialize());

  // Report some information regarding the noise tool
  if ( m_useNoiseTool && m_usePileUpNoise) {
    ATH_MSG_DEBUG( "Pile-Up Noise from Noise Tool "
         << " is selected! The noise sigma will be the"
                   << " quadratic sum of the electronics noise and the pile up!"  );
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

  ATH_MSG_INFO( " Calo selection applied ? " << m_caloSelection  );
  if (m_caloSelection) {
    msg() << MSG::INFO << "   subcalo selected ";
    for (unsigned int iCalos=0;iCalos< m_includedCalos.size(); iCalos++ )
      msg() << MSG::INFO << " " << m_includedCalos[iCalos];
    msg() << MSG::INFO << " " << endmsg;
  }


  ////////////////////
  // Allocate Tools //
  ////////////////////

  // check tool names
  if ( m_ptools.size() == 0 )
    {
      ATH_MSG_ERROR( "no tools given for this algorithm." );
      return StatusCode::FAILURE;
    }

  // find tools
  
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator firstITool = m_ptools.begin();
  ToolHandleArray<ICaloTopoTowerBuilderToolBase>::const_iterator lastITool  = m_ptools.end();

  unsigned int toolCtr = 0;
  ATH_MSG_INFO( " "  );
  ATH_MSG_INFO( "List of tools in execution sequence:" );
  ATH_MSG_INFO( "------------------------------------" );

  StatusCode checkOut = m_ptools.retrieve();
  if ( checkOut.isFailure() ) 
    {
      ATH_MSG_WARNING( "Cannot retrieve tool array " << m_ptools  );
      return StatusCode::FAILURE;
    }

  for ( ; firstITool != lastITool; firstITool++ )
    {
      toolCtr++;
      /*      ATH_MSG_INFO( "retrieving tool"  );

      if ( checkOut.isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve tool at ToolArray[" << toolCtr-1 << "]"  );
      ATH_MSG_WARNING( "This tool won't be used"  );
      }
      else {
      ATH_MSG_INFO( "retrieved tool"  );
	// print the list of tools
	*/


      ATH_MSG_INFO( std::setw(2) << toolCtr << ".) "
	  << (*firstITool)->type()
	  << "::name() = \042"
	  << (*firstITool)->name()
	  << "\042" );

      ATH_MSG_INFO( "------------------------------------" );
      ATH_MSG_INFO( " "  );

	/*if ( (*firstITool)->initializeTool().isFailure() ) {

	  ATH_MSG_WARNING( " Tool failed to initialize"  );
	  }*/
      
    } //close iteration over tools
  return StatusCode::SUCCESS;
}

/////////////
// Execute //
/////////////

StatusCode CaloTopoTowerAlgorithm::execute (const EventContext& ctx) const
{

  //////////////////////////
  // Re-allocate Services //
  //////////////////////////

  //timing
  IChronoStatSvc* theTicker = chronoSvc();

  /////////////////////
  // Tool Processing //
  /////////////////////
  /// retrieve existing Tower container
  SG::ReadHandle<CaloTowerContainer> towerContainer(m_towerContainerKey, ctx);
  if ( !towerContainer.isValid() ) {
    ATH_MSG_WARNING( " cannot retrieve tower container with key " << towerContainer.name()  );
    return StatusCode::SUCCESS;
  }

  /// get CaloCell container from StoreGate
  SG::ReadHandle<CaloCellContainer> theCells(m_cellContainerKey, ctx);
  if ( !theCells.isValid()) {
    ATH_MSG_WARNING( " cannot retrieve cell container with key " << theCells.name()  );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "CaloTopoTowerAlgorithm::execute " << theCells.name() << " size= " << theCells->size()  );
      
  ///+++ pick up TopoCluster from StoreGate
  SG::ReadHandle<CaloClusterContainer> clusters(m_clusterKey, ctx);

  if ( !clusters.isValid() )
    {
      ATH_MSG_WARNING( "cannot retrieve CaloClusterContainer with key <"
            << clusters.name() << ">" );
      return StatusCode::SUCCESS;
    }

  ///+++ pick up CaloCell2ClusterMap from StoreGate
  SG::ReadHandle<CaloCell2ClusterMap>  cellToClusterMap(m_cellToClusterMapKey, ctx);
   if ( !cellToClusterMap.isValid() ){
    ATH_MSG_WARNING( "cannot retrieve CaloCell2ClusterMap with key <"
	  << cellToClusterMap.name() << ">"  );
    return StatusCode::SUCCESS;
  } 
  
  ATH_MSG_DEBUG( "Successfully retrieved CaloCell2ClusterMap <"<< cellToClusterMap.name() << ">" );

  SG::WriteHandle<CaloTopoTowerContainer> theTowers( m_newTowerContainerKey, ctx);
  ATH_CHECK( theTowers.record (std::make_unique<CaloTopoTowerContainer>(towerContainer->towerseg())) );

  ////////////////////////////////////////////////////////////////////////////
  //Starting to save variable to CaloTopoTowerContainer
  //
  
  theTowers->SetTowers(towerContainer.cptr());
  theTowers->SetClusters(clusters.ptr());
  theTowers->SetCells(theCells.cptr());
  theTowers->SetCellToClusterMap(cellToClusterMap.cptr());

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

  ATH_MSG_DEBUG( "In execute() "  );
  
  while ( ! processStatus.isFailure() && firstITool != lastITool )
    {
      //      CaloTopoTowerBuilderToolBase* pTool = *firstTool;
      //sc = (*firstITool).retrieve();
      //if ( sc.isFailure() ) { log << MSG::INFO << "error retrieving tool " << endmsg; }

      //if ( (*firstITool).empty() ) { log << MSG::INFO << "tool is empty " << endmsg; }

      //ATH_MSG_INFO( "tool retrieved, going to start "  );
      if ( theTicker != 0 )
	{
	  //	  ATH_MSG_INFO( "Chrono start "  );
	  theTicker->chronoStart((*firstITool)->name());
	}
      /*      ATH_MSG_INFO( "executing tool: " << (*firstITool)->name()  );
              ATH_MSG_INFO( "this is &(firstITool): " << &(firstITool)  );
              ATH_MSG_INFO( "this is (*firstITool): " << (*firstITool)  );
              ATH_MSG_INFO( "this is &(*firstITool): " << &(*firstITool)  );
              ATH_MSG_INFO( "this is &(*(*firstITool)): " << &(*(*firstITool))  );
              ATH_MSG_INFO( "this is theTowers: " << theTowers  );
      */
      
      processStatus = (*firstITool)->execute(theTowers.ptr());
      
      // ATH_MSG_INFO( "processStatus is: " << processStatus  );
      

      if ( theTicker != 0 )
	{
	  //  ATH_MSG_INFO( "Chrono stop "  );
	  theTicker->chronoStop((*firstITool)->name());
	}
      if ( ! processStatus.isFailure() )
	{
	  ATH_MSG_DEBUG( (*firstITool)->name()
	      << ": CaloTopoTowerContainer::size() = "
	      << theTowers->size() );
	  firstITool++;
	}
      else
	{
	  // some problem - but do not skip event loop!
	  ATH_MSG_ERROR( "problems while or after processing tool \042"
	      << (*firstITool)->name()
	      << "\042 - cross-check CaloTopoTowerContainer::size() = "
	      << theTowers->size() );
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

  return StatusCode::SUCCESS;
}

//////////////
// Finalize //
//////////////

StatusCode CaloTopoTowerAlgorithm::finalize()
{
  return StatusCode::SUCCESS;
}
