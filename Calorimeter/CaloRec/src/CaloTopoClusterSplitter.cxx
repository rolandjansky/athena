/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoClusterSplitter.cxx,v 1.25 2009-04-18 02:56:18 ssnyder Exp $
//
// Description: see CaloTopoClusterSplitter.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------


#include "CaloProtoCluster.h"
#include "CaloTopoClusterSplitter.h"
#include "CaloTopoSplitterClusterCell.h"
#include "CaloTopoSplitterHashCluster.h"
#include "CaloTopoTmpHashCell.h"
#include "CaloUtils/CaloClusterEtSort.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloTopoTmpHashCellSort.h"
#include "CaloRec/CaloBadCellHelper.h"
#include "CaloEvent/CaloCell.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaHandle.h"
#include "CxxUtils/prefetch.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <memory>


using HepGeom::Vector3D;
using CLHEP::MeV;
using CLHEP::cm;

//###############################################################################

CaloTopoClusterSplitter::CaloTopoClusterSplitter(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  
  : AthAlgTool(type, name, parent),
    m_calo_id(0),
    m_neighborOption("super3D"),
    m_nOption(LArNeighbours::super3D),
    m_restrictHECIWandFCalNeighbors(false),
    m_nCells(4),
    m_minEnergy(500*MeV),
    m_shareBorderCells(false),
    m_emShowerScale(5*cm),
    m_minSampling (0),
    m_maxSampling (0),
    m_minSecondarySampling (0),
    m_maxSecondarySampling (0),
    m_treatL1PredictedCellsAsGood      (true),
    m_absOpt      (false)
{
  declareInterface<CaloClusterCollectionProcessor> (this);
  // Neighbor Option
  declareProperty("NeighborOption",m_neighborOption);
  // Restrict HEC IW and FCal Neighbors
  declareProperty("RestrictHECIWandFCalNeighbors",m_restrictHECIWandFCalNeighbors);
  // minimal number of cells around a local max
  declareProperty("NumberOfCellsCut",m_nCells);
  // minimal energy for a local max
  declareProperty("EnergyCut",m_minEnergy);
  // Name(s) of Calorimeter Samplings to consider for local maxima
  declareProperty("SamplingNames",m_samplingNames);
  // Name(s) of secondary Calorimeter Samplings to consider for local maxima
  // if no maximum in a primary sampling is overlapping
  declareProperty("SecondarySamplingNames",m_secondarySamplingNames);
  // Whether or not to share cells at the boundary between two clusters
  declareProperty("ShareBorderCells",m_shareBorderCells);
  // Typical em shower distance for which the energy density should drop to 1/e
  declareProperty("EMShowerScale",m_emShowerScale);
  
  // Treat bad cells with dead OTX if predicted from L1 as good
  declareProperty("TreatL1PredictedCellsAsGood",m_treatL1PredictedCellsAsGood);
  // Use weighting of neg. clusters option?
  declareProperty("WeightingOfNegClusters",m_absOpt);  
}

//###############################################################################

StatusCode CaloTopoClusterSplitter::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;
  msg(MSG::INFO) << "Treat L1 Predicted Bad Cells as Good set to" << ((m_treatL1PredictedCellsAsGood) ? "true" : "false") << endmsg;

  ATH_CHECK( detStore()->retrieve (m_calo_id, "CaloCell_ID") );

  //--- set Neighbor Option

  if ( m_neighborOption == "all2D" ) 
    m_nOption = LArNeighbours::all2D;
  else if ( m_neighborOption == "all3D" ) 
    m_nOption = LArNeighbours::all3D;
  else if ( m_neighborOption == "super3D" ) 
    m_nOption = LArNeighbours::super3D;
  else {
    msg(MSG::ERROR) <<"Invalid Neighbor Option "
	<< m_neighborOption << ", exiting ..." << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Neighbor Option "
		 << m_neighborOption << " is selected!" << endmsg;

  //--- check sampling names to use
  std::vector<std::string>::iterator samplingIter = m_samplingNames.begin(); 
  std::vector<std::string>::iterator samplingIterEnd = m_samplingNames.end(); 
  for(; samplingIter!=samplingIterEnd; ++samplingIter) { 
    if ( *samplingIter == "PreSamplerB" ) 
      m_validSamplings.insert(CaloCell_ID::PreSamplerB);
    else if ( *samplingIter == "EMB1" ) 
      m_validSamplings.insert(CaloCell_ID::EMB1);
    else if ( *samplingIter == "EMB2" ) 
      m_validSamplings.insert(CaloCell_ID::EMB2);
    else if ( *samplingIter == "EMB3" ) 
      m_validSamplings.insert(CaloCell_ID::EMB3);
    else if ( *samplingIter == "PreSamplerE" ) 
      m_validSamplings.insert(CaloCell_ID::PreSamplerE);
    else if ( *samplingIter == "EME1" ) 
      m_validSamplings.insert(CaloCell_ID::EME1);
    else if ( *samplingIter == "EME2" ) 
      m_validSamplings.insert(CaloCell_ID::EME2);
    else if ( *samplingIter == "EME3" ) 
      m_validSamplings.insert(CaloCell_ID::EME3);
    else if ( *samplingIter == "HEC0" ) 
      m_validSamplings.insert(CaloCell_ID::HEC0);
    else if ( *samplingIter == "HEC1" ) 
      m_validSamplings.insert(CaloCell_ID::HEC1);
    else if ( *samplingIter == "HEC2" ) 
      m_validSamplings.insert(CaloCell_ID::HEC2);
    else if ( *samplingIter == "HEC3" ) 
      m_validSamplings.insert(CaloCell_ID::HEC3);
    else if ( *samplingIter == "TileBar0" ) 
      m_validSamplings.insert(CaloCell_ID::TileBar0);
    else if ( *samplingIter == "TileBar1" ) 
      m_validSamplings.insert(CaloCell_ID::TileBar1);
    else if ( *samplingIter == "TileBar2" ) 
      m_validSamplings.insert(CaloCell_ID::TileBar2);
    else if ( *samplingIter == "TileGap1" ) 
      m_validSamplings.insert(CaloCell_ID::TileGap1);
    else if ( *samplingIter == "TileGap2" ) 
      m_validSamplings.insert(CaloCell_ID::TileGap2);
    else if ( *samplingIter == "TileGap3" ) 
      m_validSamplings.insert(CaloCell_ID::TileGap3);
    else if ( *samplingIter == "TileExt0" ) 
      m_validSamplings.insert(CaloCell_ID::TileExt0);
    else if ( *samplingIter == "TileExt1" ) 
      m_validSamplings.insert(CaloCell_ID::TileExt1);
    else if ( *samplingIter == "TileExt2" ) 
      m_validSamplings.insert(CaloCell_ID::TileExt2);
    else if ( *samplingIter == "FCAL0" ) 
      m_validSamplings.insert(CaloCell_ID::FCAL0);
    else if ( *samplingIter == "FCAL1" ) 
      m_validSamplings.insert(CaloCell_ID::FCAL1);
    else if ( *samplingIter == "FCAL2" ) 
      m_validSamplings.insert(CaloCell_ID::FCAL2);
    else 
      msg(MSG::ERROR) <<"Calorimeter sampling" << *samplingIter 
	  << " is not a valid Calorimeter sampling name and will be ignored! "
	  << "Valid names are: "
	  << "PreSamplerB, EMB1, EMB2, EMB3, "
	  << "PreSamplerE, EME1, EME2, EME3, "
          << "HEC0, HEC1, HEC2, HEC3, "
          << "TileBar0, TileBar1, TileBar2, "
          << "TileGap1, TileGap2, TileGap3, "
          << "TileExt0, TileExt1, TileExt2, "
          << "FCAL0, FCAL1, FCAL2." << endmsg;
  }

  msg(MSG::INFO) << "Samplings to consider for local maxima:";
  samplingIter = m_samplingNames.begin(); 
  for(; samplingIter!=samplingIterEnd; ++samplingIter)  
    msg() << " " << *samplingIter;
  msg() << endmsg;

  m_minSampling=0;
  m_maxSampling=0;
  std::set<int>::const_iterator vSamplingIter = m_validSamplings.begin(); 
  std::set<int>::const_iterator vSamplingIterEnd = m_validSamplings.end(); 
  for(; vSamplingIter!=vSamplingIterEnd; ++vSamplingIter) {
    if ( (*vSamplingIter) > m_maxSampling ) 
      m_maxSampling = (*vSamplingIter);
    if ( (*vSamplingIter) < m_minSampling ) 
      m_minSampling = (*vSamplingIter);
  }

  m_useSampling.resize(m_maxSampling-m_minSampling+1,false);

  for(vSamplingIter = m_validSamplings.begin(); vSamplingIter!=vSamplingIterEnd; ++vSamplingIter) {
    m_useSampling[(*vSamplingIter)-m_minSampling] = true;
  }

  //--- check sampling names to use
  samplingIter = m_secondarySamplingNames.begin(); 
  samplingIterEnd = m_secondarySamplingNames.end(); 
  for(; samplingIter!=samplingIterEnd; ++samplingIter) { 
    if ( *samplingIter == "PreSamplerB" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::PreSamplerB);
    else if ( *samplingIter == "EMB1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::EMB1);
    else if ( *samplingIter == "EMB2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::EMB2);
    else if ( *samplingIter == "EMB3" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::EMB3);
    else if ( *samplingIter == "PreSamplerE" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::PreSamplerE);
    else if ( *samplingIter == "EME1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::EME1);
    else if ( *samplingIter == "EME2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::EME2);
    else if ( *samplingIter == "EME3" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::EME3);
    else if ( *samplingIter == "HEC0" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::HEC0);
    else if ( *samplingIter == "HEC1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::HEC1);
    else if ( *samplingIter == "HEC2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::HEC2);
    else if ( *samplingIter == "HEC3" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::HEC3);
    else if ( *samplingIter == "TileBar0" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileBar0);
    else if ( *samplingIter == "TileBar1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileBar1);
    else if ( *samplingIter == "TileBar2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileBar2);
    else if ( *samplingIter == "TileGap1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileGap1);
    else if ( *samplingIter == "TileGap2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileGap2);
    else if ( *samplingIter == "TileGap3" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileGap3);
    else if ( *samplingIter == "TileExt0" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileExt0);
    else if ( *samplingIter == "TileExt1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileExt1);
    else if ( *samplingIter == "TileExt2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::TileExt2);
    else if ( *samplingIter == "FCAL0" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::FCAL0);
    else if ( *samplingIter == "FCAL1" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::FCAL1);
    else if ( *samplingIter == "FCAL2" ) 
      m_validSecondarySamplings.insert(CaloCell_ID::FCAL2);
    else 
      msg(MSG::ERROR) <<"Calorimeter sampling" << *samplingIter 
	  << " is not a valid Calorimeter sampling name and will be ignored! "
	  << "Valid names are: "
	  << "PreSamplerB, EMB1, EMB2, EMB3, "
	  << "PreSamplerE, EME1, EME2, EME3, "
          << "HEC0, HEC1, HEC2, HEC3, "
          << "TileBar0, TileBar1, TileBar2, "
          << "TileGap1, TileGap2, TileGap3, "
          << "TileExt0, TileExt1, TileExt2, "
          << "FCAL0, FCAL1, FCAL2." << endmsg;
  }

  msg(MSG::INFO) << "Secondary samplings to consider for local maxima:";
  samplingIter = m_secondarySamplingNames.begin(); 
  for(; samplingIter!=samplingIterEnd; ++samplingIter)  
    msg() << " " << *samplingIter;
  msg() << endmsg;

  m_minSecondarySampling=0;
  m_maxSecondarySampling=0;
  vSamplingIter = m_validSecondarySamplings.begin(); 
  vSamplingIterEnd = m_validSecondarySamplings.end(); 
  for(; vSamplingIter!=vSamplingIterEnd; ++vSamplingIter) {
    if ( (*vSamplingIter) > m_maxSecondarySampling ) 
      m_maxSecondarySampling = (*vSamplingIter);
    if ( (*vSamplingIter) < m_minSecondarySampling ) 
      m_minSecondarySampling = (*vSamplingIter);
  }

  m_useSecondarySampling.resize(m_maxSecondarySampling-m_minSecondarySampling+1,false);

  for(vSamplingIter = m_validSecondarySamplings.begin(); vSamplingIter!=vSamplingIterEnd; ++vSamplingIter) {
    m_useSecondarySampling[(*vSamplingIter)-m_minSecondarySampling] = true;
  }

  m_hashMin = 999999;
  m_hashMax = 0;
  for(unsigned int iCalo=0;iCalo<CaloCell_ID::NSUBCALO; iCalo++) {
    IdentifierHash thismin, thismax;
    m_calo_id->calo_cell_hash_range (iCalo, thismin, thismax);
    m_hashMin = std::min (m_hashMin, thismin);
    m_hashMax = std::max (m_hashMax, thismax);
  }

  return StatusCode::SUCCESS;
  
}

//###############################################################################

StatusCode CaloTopoClusterSplitter::execute(const EventContext& ctx,
                                            xAOD::CaloClusterContainer* clusColl) const
{
  ATH_MSG_DEBUG("Executing " << name());

  typedef CaloTopoTmpHashCell<CaloTopoSplitterClusterCell> HashCell;
  typedef CaloTopoSplitterHashCluster HashCluster;

  SG::ArenaHandle<CaloTopoSplitterClusterCell, SG::ArenaPoolAllocator>
    tmpcell_pool;
  SG::ArenaHandle<HashCluster,                 SG::ArenaPoolAllocator>
    tmpclus_pool;
  CaloTopoSplitterHashCluster::pool_type                      tmplist_pool;

  // create cell list for cells above seed  cut (for seed growing algo)
  std::vector<HashCell> mySeedCells;
  mySeedCells.reserve (200);
  // create initial cluster list (one cell per cluster)
  std::vector<HashCluster *> myHashClusters;
  myHashClusters.reserve (20000);
  // create cell list in order to clean up in the end
  std::vector<CaloTopoSplitterClusterCell *> allCellList;
  allCellList.reserve (20000);
  // create vector to hold all cells for a given subsystem (the IdentifierHash is
  // used as index - therefore the vector has always maximal size)
  std::vector<HashCell> cellVector (m_hashMax - m_hashMin);

  //---- loop over the initial set of Clusters

  std::vector<int> hasLocalMaxVector;
  hasLocalMaxVector.resize(clusColl->size(),0);
  int iClusterNumber = 0;
  float eTotOrig(0.);
  int nTotOrig(0);
  xAOD::CaloClusterContainer::iterator clusCollIter    = clusColl->begin();
  xAOD::CaloClusterContainer::iterator clusCollIterEnd = clusColl->end();

  // get cluster size and underlying cell container (assume it's identical for the whole collection)
  xAOD::CaloCluster::ClusterSize clusterSize = xAOD::CaloCluster::CSize_Unknown;
  const CaloCellContainer* myCellColl=0;
  if (clusCollIter != clusCollIterEnd) {
    clusterSize = (*clusCollIter)->clusterSize();
    ATH_MSG_DEBUG("cluster size = " <<clusterSize);
    const CaloClusterCellLink* lnk=(*clusCollIter)->getCellLinks();
    if (!lnk) {
      msg(MSG::ERROR) << "Can't get valid links to CaloCells (CaloClusterCellLink)!" << endmsg;
      return StatusCode::FAILURE;
    }
    myCellColl = lnk->getCellContainer();    
  }
  else {
    ATH_MSG_DEBUG("Got an empty input collection. Do notihing");
    return StatusCode::SUCCESS;
  }


  for (; clusCollIter != clusCollIterEnd; ++clusCollIter, ++iClusterNumber ){
    xAOD::CaloCluster* parentCluster = (*clusCollIter);
    CaloClusterCellLink* cellLinks=parentCluster->getOwnCellLinks();
    if (!cellLinks) {
      msg(MSG::ERROR) << "Can't get valid links to CaloCells (CaloClusterCellLink)!" << endmsg;
      return StatusCode::FAILURE;
    }

    eTotOrig+=parentCluster->e();
    nTotOrig+=cellLinks->size();
    // use the number of the parent cluster to identify 
    // cells from the same parent cluster
    // only cells belonging to the same parent cluster are allowed
    // to be merged into one split cluster

    //---- Get the CaloCells from this cluster
    xAOD::CaloCluster::cell_iterator cellIter    = parentCluster->cell_begin();
    xAOD::CaloCluster::cell_iterator cellIterEnd = parentCluster->cell_end();
    
    for(; cellIter != cellIterEnd; cellIter++ ){
      CxxUtils::prefetchNext (cellIter, cellIterEnd); //FIXME Does this work with the new cell-iterator?

      const CaloCell* pCell = (*cellIter);
      Identifier myId = pCell->ID();
      
      // store energy here to search for local maxima
      int caloSample = m_calo_id->calo_sample(myId);
      float signedRatio = 0;
      bool is_secondary = false;         //reintroduced  is_secondary for negative cluster option
      // in case the cell is not a bad cell
      // check if the current cell belongs to a sampling
      // that should be considered for local maxima.
      // in case the cell does not belong to such a sampling its signedRatio
      // is set to 0. The cell is still counted as neighbor cell but
      // will not make (or prevent) a local maximum 
      if ( !CaloBadCellHelper::isBad(pCell,m_treatL1PredictedCellsAsGood) ) { 
	if ( (m_absOpt || pCell->e() > 0) 
	     && caloSample >= m_minSampling 
	     && caloSample <= m_maxSampling 
	     && m_useSampling[caloSample-m_minSampling] )
	  signedRatio = pCell->e();
	// check also if the current cell belongs to a sampling
	// that should be considered for secondary local maxima.
	// in case the cell does belong to such a sampling its signedRatio
	// will be set to -e(). The is still counted as neighbor cell but
	// will not make (or prevent) a local maximum 
	else if ( (m_absOpt || pCell->e() > 0)  
		  && caloSample >= m_minSecondarySampling 
		  && caloSample <= m_maxSecondarySampling 
		  && m_useSecondarySampling[caloSample-m_minSecondarySampling] ){
	  signedRatio = -pCell->e();
          is_secondary = true;  
        } 
      }
            
      
      IdentifierHash hashid = m_calo_id->calo_cell_hash(myId);
      CaloCell_ID::SUBCALO subdet = (CaloCell_ID::SUBCALO)m_calo_id->sub_calo (hashid);
      // use iterator and not cell pointer in lookup of cell container for speed
      //int myIndex = myCellColl->findIndex(hashid);
      size_t myIndex=cellIter.index();
      CaloTopoSplitterClusterCell *tmpClusterCell = 
        new (tmpcell_pool.allocate())
	CaloTopoSplitterClusterCell(hashid, subdet,
                                    cellIter,myIndex,
                                    signedRatio,parentCluster,
                                    iClusterNumber,is_secondary);  
      // some debug printout - can also be used to construct neighbor
      // tables offline ...
      if ( ctx.evt() == 0 &&  msgLvl(MSG::DEBUG)) {
	msg(MSG::INFO) << " [ExtId|Id|SubDet|HashId|eta|phi|iParent|E]: "
		       << "[" << m_calo_id->show_to_string(myId,0,'/')
		       << "|" << myId.getString() 
		       << "|" << subdet
		       << "|" << (unsigned int)hashid
		       << "|" << pCell->eta()
		       << "|" << pCell->phi()
		       << "|" << iClusterNumber
		       << "|" << signedRatio
		       << "]" << endmsg;
      }
      HashCell hashCell(tmpClusterCell);
      HashCluster *tmpCluster =
        new (tmpclus_pool.allocate()) HashCluster (tmplist_pool);
      tmpClusterCell->setCaloTopoTmpHashCluster(tmpCluster);
      tmpCluster->add(hashCell);
      myHashClusters.push_back(tmpCluster);
      allCellList.push_back(tmpClusterCell);
      cellVector[(unsigned int)hashid - m_hashMin] = hashCell;
    }
  }

  // Vectors to hold the results of get_neighbors().
  // Create them here, at the top level, so we don't need
  // to reallocate the vectors each trip through the inner loops.
  std::vector<IdentifierHash> theNeighbors;
  theNeighbors.reserve(22);
  std::vector<IdentifierHash> theSuperNeighbors;
  theSuperNeighbors.reserve(22);
  std::vector<IdentifierHash> theNNeighbors;
  theNNeighbors.reserve(22);
  std::vector<IdentifierHash> theCurrentNeighbors;
  theCurrentNeighbors.reserve(88);
  std::vector<IdentifierHash> theNextNeighbors;
  theNextNeighbors.reserve(88);
   
  // look for local maxima
  std::vector<CaloTopoSplitterClusterCell*>::iterator allCellIter=allCellList.begin();
  std::vector<CaloTopoSplitterClusterCell*>::iterator allCellIterEnd=allCellList.end();
  for(;allCellIter != allCellIterEnd;++allCellIter) { 
    CaloTopoSplitterClusterCell* pClusCell = (*allCellIter);
    // only check cells for which we don't know if a neighbor with larger
    // energy was found before
    if (! pClusCell->getUsed() ) {
      float myEnergy = pClusCell->getSignedRatio();
      if(m_absOpt) myEnergy=fabs(myEnergy);
      if ( myEnergy >= m_minEnergy &&  !pClusCell->getSecondary() ) {
        int nCells=0; 
        bool isLocalMax = true;
        size_t iParent = pClusCell->getParentClusterIndex();
        IdentifierHash hashid = pClusCell->getID();
        theNeighbors.clear();
        m_calo_id->get_neighbours(hashid,m_nOption,theNeighbors);
        for (unsigned int iN=0;iN<theNeighbors.size();iN++) {
          IdentifierHash nId = theNeighbors[iN];
          HashCell neighborCell = cellVector[(unsigned int)nId - m_hashMin];
          CaloTopoSplitterClusterCell *pNeighCell = neighborCell.getCaloTopoTmpClusterCell();
          if ( pNeighCell && pNeighCell->getParentClusterIndex() == iParent) {
            nCells++;
            if ( ((myEnergy > pNeighCell->getSignedRatio() ) && !m_absOpt)   ||
               (m_absOpt && myEnergy > fabs(pNeighCell->getSignedRatio() ) ) || 
                pNeighCell->getSecondary()  ) { 
              // in case the neighbor cell is a 2nd local max candidate
              // it has negative energy and we set it to used only if also
              // its abs value is smaller than myEnergy
              if (fabs(pNeighCell->getSignedRatio()) <  myEnergy ) 
                pNeighCell->setUsed(); 
            } 
            else { 
              isLocalMax=false;
            }
          }
        }
        if ( nCells < m_nCells )
          isLocalMax = false;
        if ( isLocalMax ) {
          mySeedCells.push_back(cellVector[(unsigned int)hashid - m_hashMin]);
          hasLocalMaxVector[iParent]++; 
        }
      }
    }
  } 
  // look for secondary local maxima
  if ( m_validSecondarySamplings.size() > 0 ) {
    allCellIter=allCellList.begin();
    for(;allCellIter != allCellIterEnd;++allCellIter) {
      CaloTopoSplitterClusterCell* pClusCell = (*allCellIter);
      // only check cells for which we don't know if a neighbor with larger
      // energy was found before 
      if (! pClusCell->getUsed()  && pClusCell->getSecondary()) {  
	float myEnergy = pClusCell->getSignedRatio();
        if(m_absOpt) myEnergy=fabs(myEnergy);
	if ( (!m_absOpt && myEnergy <= -m_minEnergy) || (m_absOpt && myEnergy >= m_minEnergy) ) {
	  int nCells=0;
	  bool isLocalMax = true;
	  size_t iParent = pClusCell->getParentClusterIndex();
	  IdentifierHash hashid = pClusCell->getID();
          //CaloCell_ID::SUBCALO mySubDet = pClusCell->getSubDet();
          theNeighbors.clear();
	  m_calo_id->get_neighbours(hashid,m_nOption,theNeighbors);
	  for (unsigned int iN=0;iN<theNeighbors.size();iN++) {
	    IdentifierHash nId = theNeighbors[iN];
	    HashCell neighborCell = cellVector[(unsigned int)nId - m_hashMin];
	    CaloTopoSplitterClusterCell *pNeighCell = neighborCell.getCaloTopoTmpClusterCell();
	    if ( pNeighCell && pNeighCell->getParentClusterIndex() == iParent) {
	      nCells++;
	      if ( fabs(myEnergy) > fabs(pNeighCell->getSignedRatio()) ) {
		pNeighCell->setUsed();
	      } 
	      else {
		isLocalMax=false;
	      }
	    }
	  }
	  if ( nCells < m_nCells ) 
	    isLocalMax = false;
	  // check the neighbors in all previous and all next samplings
	  // for overlapping cells in the primary local maximum list
	  // in case such cells exist do not consider this cell as 
	  // local maximum
	  if ( isLocalMax ) {
	    // first check previous samplings
	    if ( m_nOption & (LArNeighbours::prevInSamp|LArNeighbours::prevSuperCalo)) {
	      // start with current cell
              theCurrentNeighbors.clear();
	      theCurrentNeighbors.push_back(hashid);
	      while ( isLocalMax && theCurrentNeighbors.size() > 0 ) {
		// loop over the current neighbors and add all found cells in 
		// previous samplings to the next neighbor list
		theNextNeighbors.clear();
		for (unsigned int iN=0;iN<theCurrentNeighbors.size() 
		       && isLocalMax;iN++) {
                  theNeighbors.clear();
		  theSuperNeighbors.clear();
		  if ( m_nOption & LArNeighbours::prevInSamp ) 
		    m_calo_id->get_neighbours(theCurrentNeighbors[iN],LArNeighbours::prevInSamp,theNeighbors);
		  if ( m_nOption & LArNeighbours::prevSuperCalo )
		    m_calo_id->get_neighbours(theCurrentNeighbors[iN],LArNeighbours::prevSuperCalo,theSuperNeighbors);
		  theNeighbors.insert(theNeighbors.end(),theSuperNeighbors.begin(),theSuperNeighbors.end());
		  for(unsigned int iNN=0;iNN<theNeighbors.size() && isLocalMax;iNN++) {
		    IdentifierHash nId = theNeighbors[iNN];
		    std::vector<HashCell>::iterator hashCellIter;
		    std::vector<HashCell>::iterator hashCellIterEnd;
		    
		    hashCellIter= mySeedCells.begin();
		    hashCellIterEnd=mySeedCells.end();
		    
		    // loop over all seed cells and check if cells match
		    for(;hashCellIter!=hashCellIterEnd 
			  && isLocalMax;++hashCellIter) {
		      if ( cellVector[(unsigned int)nId - m_hashMin] 
			   == (*hashCellIter) )
                      {
			isLocalMax = false;
		      }
		    }
		    if ( isLocalMax ) { 
		      // no matching seed cell was found for this
		      // neighbor - so add it to the next list in case it's
		      // not yet included
		      bool doInclude(true);
		      for(unsigned int iNNN=0;iNNN<theNextNeighbors.size();iNNN++) {
			if ( theNextNeighbors[iNNN] == theNeighbors[iNN] ) {
			  doInclude = false;
			  break;
			}
		      }
		      if ( doInclude ) 
			theNextNeighbors.push_back(theNeighbors[iNN]);
		    }
		  }
		}
		theCurrentNeighbors.swap (theNextNeighbors);
	      }
	    }
	  }
	  if ( isLocalMax ) {
	    // now check next samplings
	    if ( m_nOption & (LArNeighbours::nextInSamp|LArNeighbours::nextSuperCalo) ) {
	      std::vector<IdentifierHash> theCurrentNeighbors;
	      std::vector<IdentifierHash> theNextNeighbors;
	      // start with current cell
	      theCurrentNeighbors.push_back(hashid);
	      while ( isLocalMax && theCurrentNeighbors.size() > 0 ) {
		// loop over the current neighbors and add all found cells in 
		// next samplings to the next neighbor list
		theNextNeighbors.clear();
		for (unsigned int iN=0;iN<theCurrentNeighbors.size() 
		       && isLocalMax;iN++) {
		  theNeighbors.clear();
		  theSuperNeighbors.clear();
		  if ( m_nOption & LArNeighbours::nextInSamp ) 
		    m_calo_id->get_neighbours(theCurrentNeighbors[iN],LArNeighbours::nextInSamp,theNeighbors);
		  if ( m_nOption & LArNeighbours::nextSuperCalo )
		    m_calo_id->get_neighbours(theCurrentNeighbors[iN],LArNeighbours::nextSuperCalo,theSuperNeighbors);
		  theNeighbors.insert(theNeighbors.end(),theSuperNeighbors.begin(),theSuperNeighbors.end());
		  for(unsigned int iNN=0;iNN<theNeighbors.size() && isLocalMax;iNN++) {
		    IdentifierHash nId = theNeighbors[iNN];
		    std::vector<HashCell>::iterator hashCellIter;
		    std::vector<HashCell>::iterator hashCellIterEnd;
		    
		    hashCellIter= mySeedCells.begin();
		    hashCellIterEnd=mySeedCells.end();
		    
		    // loop over all seed cells and check if cells match
		    for(;hashCellIter!=hashCellIterEnd 
			  && isLocalMax;++hashCellIter) {
		      if ( cellVector[(unsigned int)nId - m_hashMin] 
			   == (*hashCellIter) )
                      {
			isLocalMax = false;
		      }
		    }
		    if ( isLocalMax ) { 
		      // no matching seed cell was found for this
		      // neighbor - so add it to the next list in case it's
		      // not yet included
		      bool doInclude(true);
		      for(unsigned int iNNN=0;iNNN<theNextNeighbors.size();iNNN++) {
			if ( theNextNeighbors[iNNN] == theNeighbors[iNN] ) {
			  doInclude = false;
			  break;
			}
		      }
		      if ( doInclude ) 
			theNextNeighbors.push_back(theNeighbors[iNN]);
		    }
		  }
		}
		theCurrentNeighbors.swap (theNextNeighbors);
	      }
	    }
	  }
	  // did the cell survive until here make it a local maximum
	  if ( isLocalMax ) {
	    mySeedCells.push_back(cellVector[(unsigned int)hashid - m_hashMin]);
	    hasLocalMaxVector[iParent]++;
	  }
	}
      }
    }
  }
  allCellIter=allCellList.begin();
  for(;allCellIter != allCellIterEnd;++allCellIter) {
    (*allCellIter)->setUnused();
    const xAOD::CaloCluster::cell_iterator itrCell = (*allCellIter)->getCellIterator();
    (*allCellIter)->setSignedRatio(itrCell->e());
  }

  // create shared cell list for border cells between two split clusters
  std::vector<HashCell> sharedCellList;
  std::vector<HashCell> nextSharedCellList;

  std::vector<HashCell>::iterator hashCellIter;
  std::vector<HashCell>::iterator hashCellIterEnd;
  
  hashCellIter= mySeedCells.begin();
  hashCellIterEnd=mySeedCells.end();
  
  // loop over all seed cells and set them Used
  for(;hashCellIter!=hashCellIterEnd;++hashCellIter) {
    hashCellIter->getCaloTopoTmpClusterCell()->setUsed();
    HashCluster *myCluster = hashCellIter->getCaloTopoTmpClusterCell()->getCaloTopoTmpHashCluster();
    myCluster->setContainsLocalMax();
  }

  // sort initial seed cells to start with the cell of largest E
  // this makes the resulting clusters independent of the initial
  // ordering of the cells 

  CaloTopoTmpHashCellSort::compare<CaloTopoSplitterClusterCell> compareE;
  std::sort(mySeedCells.begin(),mySeedCells.end(),compareE); 

  if ( msgLvl(MSG::DEBUG)) {
    hashCellIter= mySeedCells.begin();
    hashCellIterEnd=mySeedCells.end();

    // loop over all current neighbor cells (for Seed Growing Algo)
    for(;hashCellIter!=hashCellIterEnd;++hashCellIter) {
      msg(MSG::DEBUG) << " SeedCell [" 
		      << hashCellIter->getCaloTopoTmpClusterCell()->getSubDet() 
		      << "|" 
		      << (unsigned int)hashCellIter->getCaloTopoTmpClusterCell()->getID() 
		      << "] has E = " 
		      << hashCellIter->getCaloTopoTmpClusterCell()->getSignedRatio() 
		      << endmsg;
    }
  }

  std::vector<HashCell> myNextCells;
  myNextCells.reserve (4096);
  while ( !mySeedCells.empty() ) {
    // create cell list for next neighbor cells to consider
    myNextCells.clear();
    hashCellIter= mySeedCells.begin();
    hashCellIterEnd=mySeedCells.end();
    
    // loop over all current neighbor cells (for Seed Growing Algo)
    for(;hashCellIter!=hashCellIterEnd;++hashCellIter) {
      CaloTopoSplitterClusterCell* pClusCell = hashCellIter->getCaloTopoTmpClusterCell();
      IdentifierHash hashid = pClusCell->getID();
      HashCluster *myCluster = pClusCell->getCaloTopoTmpHashCluster();
      size_t iParent = pClusCell->getParentClusterIndex();
      CaloCell_ID::SUBCALO mySubDet = pClusCell->getSubDet();
      // in case we use all3d or super3D and the current cell is in the 
      // HEC IW or FCal2 & 3 and we want to restrict their neighbors, 
      // use only next in sampling neighbors 
      theNeighbors.clear();
      if ( m_restrictHECIWandFCalNeighbors 
	   && (m_nOption & LArNeighbours::nextInSamp)
	   && ( ( mySubDet == CaloCell_ID::LARHEC 
		  && m_calo_id->region(m_calo_id->cell_id(hashid)) == 1 ) 
		|| ( mySubDet == CaloCell_ID::LARFCAL 
		     && m_calo_id->sampling(m_calo_id->cell_id(hashid)) > 1 ) ) ) {
	m_calo_id->get_neighbours(hashid,LArNeighbours::nextInSamp,theNeighbors);
      }
      else {
	m_calo_id->get_neighbours(hashid,m_nOption,theNeighbors);
      }
      // loop over all neighbors of that cell (Seed Growing Algo)
      if ( ctx.evt() == 0 && msgLvl(MSG::DEBUG)) {
	Identifier myId;
	myId = m_calo_id->cell_id(hashid);
	msg(MSG::DEBUG)  << " Cell [" << mySubDet << "|" 
			 << (unsigned int)hashid << "|"
			 << m_calo_id->show_to_string(myId,0,'/') 
			 << "] has " << theNeighbors.size() << " neighbors:" 
			 << endmsg; 
      }
      int otherSubDet;
      for (unsigned int iN=0;iN<theNeighbors.size();iN++) {
	otherSubDet = m_calo_id->sub_calo(theNeighbors[iN]);
	IdentifierHash nId = theNeighbors[iN];
	if ( ctx.evt() == 0 && msgLvl(MSG::DEBUG)) {
	  Identifier myId;
	  myId = m_calo_id->cell_id(nId);
	  msg(MSG::DEBUG) << "  NeighborCell [" << otherSubDet << "|" 
			  << (unsigned int) nId << "|" 
			  << m_calo_id->show_to_string(myId,0,'/') << "]" << endmsg;
	  theNNeighbors.clear();
	  m_calo_id->get_neighbours(nId,m_nOption,theNNeighbors);
	  bool foundId (false);
	  int nOtherSubDet;
	  for (unsigned int iNN=0;iNN<theNNeighbors.size();iNN++) {
	    nOtherSubDet = m_calo_id->sub_calo(theNNeighbors[iNN]);
	    if (nOtherSubDet == ((int)(mySubDet)) && 
		theNNeighbors[iNN] == hashid) {
	      foundId = true;
	      break;
	    }
	  }
	  if ( ! foundId ) {
	    myId = m_calo_id->cell_id(hashid);
	    msg(MSG::ERROR) <<" Cell [" << mySubDet << "|" 
		<< (unsigned int)hashid << "|"
		<< m_calo_id->show_to_string(myId,0,'/') 
		<< "] has bad neighbor cell[";
	    myId = m_calo_id->cell_id(nId);
	    
	    msg() << otherSubDet << "|" << nId << "|" 
		  << m_calo_id->show_to_string(myId,0,'/') 
		  << "]" << endmsg;
	  }
	}//end if printout
	HashCell neighborCell = cellVector[(unsigned int)nId - m_hashMin];
	CaloTopoSplitterClusterCell *pNCell = neighborCell.getCaloTopoTmpClusterCell();
	if ( pNCell && pNCell->getParentClusterIndex() == iParent && !pNCell->getShared() ) {
	  // checking the neighbors
	  HashCluster *otherCluster = pNCell->getCaloTopoTmpHashCluster();
	  if ( !pNCell->getUsed() ) {
	    pNCell->setUsed();
	    myNextCells.push_back(neighborCell);
	  }
	  else {
	    // in case the cell is used already it might have been
	    // included in the same iteration from another cluster. In
	    // this case it is a shared cell and needs to be removed
	    // from the myNextCells list and from the other cluster
	    if ( m_shareBorderCells && myCluster != otherCluster ) {
	      std::vector<HashCell>::iterator nextCellIter = myNextCells.begin();
	      std::vector<HashCell>::iterator nextCellIterEnd = myNextCells.end();
	      bool isRemoved(false);
	      // try to remove the neighborCell - if it belongs to the
	      // myNextCell list it is a shared cell, added to the
	      // list of shared cells and removed from the cluster it
	      // first was added to
              // cppcheck-suppress invalidContainer; false positive
	      while ( !isRemoved && nextCellIter != nextCellIterEnd ) {
		if ( (*nextCellIter) == neighborCell ) {
		  nextCellIter = myNextCells.erase(nextCellIter);
		  isRemoved=true;
		}
		else
		  ++nextCellIter;
	      }
	      if ( isRemoved ) {
		pNCell->setShared();
		pNCell->setSecondCaloTopoTmpHashCluster(myCluster);
		nextSharedCellList.push_back(neighborCell);
		otherCluster->remove(neighborCell);
	      }
	    }
	  }
	  if ( myCluster != otherCluster ) {
	    HashCluster *toKill = otherCluster;
	    HashCluster *toKeep = myCluster;
	    if ( toKill ) {
	      if ( !toKill->getContainsLocalMax() && toKill->size() == 1) {
		// note that the other cluster can only be merged if
		// it has size 1 and does not contain local
		// maxima. Both conditions need to be tested as in
		// some rare cases with sharing of border cells a
		// cluster including a local max might temporarily be
		// of size 1 if its direct neighbors are all shared
		// with other local maxima
		HashCluster::iterator clusCellIter=toKill->begin();
		HashCluster::iterator clusCellIterEnd = toKill->end();
		for(;clusCellIter!=clusCellIterEnd;++clusCellIter) {
		  clusCellIter->setCaloTopoTmpHashCluster(toKeep);
		}
		toKeep->add(*toKill);
		toKill->removeAll();
		myCluster = toKeep;
	      }
	    }
	    else {
	      toKeep->add(neighborCell);
	      pNCell->setCaloTopoTmpHashCluster(toKeep);
	    }
	  }
	}
      }
    }
    mySeedCells.swap (myNextCells);

    // sort next seed cells to start again with the cell of largest E
    std::sort(mySeedCells.begin(),mySeedCells.end(),compareE); 
    // sort next shared cells to start again with the cell of largest E
    if ( m_shareBorderCells) {
      std::sort(nextSharedCellList.begin(),nextSharedCellList.end(),compareE); 
      if (sharedCellList.empty())
        sharedCellList.swap (nextSharedCellList);
      else {
        sharedCellList.insert(sharedCellList.end(),
                              nextSharedCellList.begin(),
                              nextSharedCellList.end());
      }
      nextSharedCellList.clear();
    }
  }

  // cluster the remaining cells around the shared cells and give each
  // newly clustered cell the same cluster pointers as its seeding
  // neighbor. Newly included cells are used as seeds in the next
  // iteration and sorted in E.
  int nShared(0);
  if ( m_shareBorderCells ) {
    mySeedCells = sharedCellList;
    while ( !mySeedCells.empty() ) {
    
      // create cell list for next neighbor cells to consider
      myNextCells.clear();
      hashCellIter= mySeedCells.begin();
      hashCellIterEnd=mySeedCells.end();
    
      // loop over all current neighbor cells (for Seed Growing Algo)
      for(;hashCellIter!=hashCellIterEnd;++hashCellIter) {
	CaloTopoSplitterClusterCell* pClusCell = hashCellIter->getCaloTopoTmpClusterCell();
	IdentifierHash hashid = pClusCell->getID();
	HashCluster *myCluster = pClusCell->getCaloTopoTmpHashCluster();
	HashCluster *mySecondCluster = pClusCell->getSecondCaloTopoTmpHashCluster();
	size_t iParent = pClusCell->getParentClusterIndex();
        CaloCell_ID::SUBCALO mySubDet = pClusCell->getSubDet();
	// in case we use all3d or super3D and the current cell is in the 
	// HEC IW or FCal2 & 3 and we want to restrict their neighbors, 
	// use only next in sampling neighbors 
        theNeighbors.clear();
	if ( m_restrictHECIWandFCalNeighbors 
	     && (m_nOption & LArNeighbours::nextInSamp) 
	     && ( ( mySubDet == CaloCell_ID::LARHEC 
		    && m_calo_id->region(m_calo_id->cell_id(hashid)) == 1 ) 
		  || ( mySubDet == CaloCell_ID::LARFCAL 
		       && m_calo_id->sampling(m_calo_id->cell_id(hashid)) > 1 ) ) ) {
	  m_calo_id->get_neighbours(hashid,LArNeighbours::nextInSamp,theNeighbors);
	}
	else {
	  m_calo_id->get_neighbours(hashid,m_nOption,theNeighbors);
	}
	// loop over all neighbors of that cell (Seed Growing Algo)
	if ( ctx.evt() == 0 && msgLvl(MSG::DEBUG)) {
	  Identifier myId;
	  myId = m_calo_id->cell_id(hashid);
	  msg(MSG::DEBUG) << " Shared Cell [" << mySubDet << "|" 
			  << (unsigned int)hashid << "|"
			  << m_calo_id->show_to_string(myId,0,'/') 
			  << "] has " << theNeighbors.size() << " neighbors:" 
			  << endmsg; 
	}//end if printout
	int otherSubDet;
	for (unsigned int iN=0;iN<theNeighbors.size();iN++) {
	  otherSubDet = m_calo_id->sub_calo(theNeighbors[iN]);
	  IdentifierHash nId = theNeighbors[iN];
	  if (ctx.evt() == 0 && msgLvl(MSG::DEBUG)) {
	    Identifier myId;
	    myId = m_calo_id->cell_id(nId);
	    msg(MSG::DEBUG) << "  NeighborCell [" << otherSubDet << "|" 
			    << (unsigned int) nId << "|" 
			    << m_calo_id->show_to_string(myId,0,'/') << "]"
			    << endmsg;
	    theNNeighbors.clear();
	    m_calo_id->get_neighbours(nId,m_nOption,theNNeighbors);
	    bool foundId (false);
	    int nOtherSubDet;
	    for (unsigned int iNN=0;iNN<theNNeighbors.size();iNN++) {
	      nOtherSubDet = m_calo_id->sub_calo(theNNeighbors[iNN]);
	      if (nOtherSubDet == ((int)(mySubDet)) && 
		  theNNeighbors[iNN] == hashid)
              {
		foundId = true;
		break;
	      }
	    }
	    if ( ! foundId ) {
	      myId = m_calo_id->cell_id(hashid);
	      msg(MSG::ERROR) <<" Shared Cell [" << mySubDet << "|" 
		  << (unsigned int)hashid << "|"
		  << m_calo_id->show_to_string(myId,0,'/') 
		  << "] has bad neighbor cell[";
	      myId = m_calo_id->cell_id(nId);
	      
	      msg() << otherSubDet << "|" << nId << "|" 
		    << m_calo_id->show_to_string(myId,0,'/') 
		    << "]" << endmsg;
	    }
	  }
	  HashCell neighborCell = cellVector[(unsigned int)nId - m_hashMin];
	  CaloTopoSplitterClusterCell *pNCell = neighborCell.getCaloTopoTmpClusterCell();
	  if ( pNCell && pNCell->getParentClusterIndex() == iParent && !pNCell->getShared() && !pNCell->getUsed() ) {
	    // checking the neighbors
	    HashCluster *otherCluster = pNCell->getCaloTopoTmpHashCluster();
	    // the neighbors cell cluster should have just one member and be
	    // different from the 2 clusters the seed cells belongs to
	    if ( myCluster != otherCluster && mySecondCluster != otherCluster) {
	      pNCell->setUsed();
	      pNCell->setShared();
	      myNextCells.push_back(neighborCell);
	      sharedCellList.push_back(neighborCell);
	      if ( otherCluster ) 
		otherCluster->removeAll();
	      pNCell->setCaloTopoTmpHashCluster(myCluster);
	      pNCell->setSecondCaloTopoTmpHashCluster(mySecondCluster);
	    }
	  }
	}
      }
      mySeedCells.swap (myNextCells);
      
      // sort next seed cells to start again with the cell of largest E
      std::sort(mySeedCells.begin(),mySeedCells.end(),compareE); 
    }

    // add the shared cells to the clusters. For this the weights have
    // to be known first. This means that all clusters referenced in
    // the shared cell list need to compute their energy and
    // centroid. Once the energies and centroids are calculated the
    // shared cells are added with the weights w_1 = E_1/(E_1+r*E_2),
    // w2 = 1-w1, with r = exp(d1-d2), and d_i is the distance of the
    // shared cell to cluster i in units of a typical em shower
    // scale. In case E_i is negative or 0 the minimum value of 1 MeV
    // is assumed.

    // loop over all shared cells and calculate the weights. Note that
    // we don't add the shared cells to the clusters in this loop in
    // order to keep the energy calculation free of shared cells for
    // all weights
    hashCellIter= sharedCellList.begin();
    hashCellIterEnd=sharedCellList.end();
    for(;hashCellIter!=hashCellIterEnd;++hashCellIter) {
      CaloTopoSplitterClusterCell* pClusCell = hashCellIter->getCaloTopoTmpClusterCell();
      float e1 = (pClusCell->getCaloTopoTmpHashCluster())->getEnergy();
      float e2 = (pClusCell->getSecondCaloTopoTmpHashCluster())->getEnergy();
      if(m_absOpt) e1 = fabs(e1);
      if(m_absOpt) e2 = fabs(e2);
      if ( e1 <= 0 ) e1 = 1*MeV;
      if ( e2 <= 0 ) e2 = 1*MeV;    
      const xAOD::CaloCluster::cell_iterator itrCell = pClusCell->getCellIterator();
      Vector3D<double> thisPos(itrCell->x(),itrCell->y(),itrCell->z());
      const Vector3D<double> * c1 = (pClusCell->getCaloTopoTmpHashCluster())->getCentroid();
      const Vector3D<double> * c2 = (pClusCell->getSecondCaloTopoTmpHashCluster())->getCentroid();
      double d1 = (thisPos-(*c1)).mag();
      double d2 = (thisPos-(*c2)).mag();
      double r = (d1-d2)/m_emShowerScale;
      if ( r > 10 ) 
	r = exp(10);
      else if ( r < -10 ) 
	r = exp(-10);
      else
	r = exp(r);
      pClusCell->setSharedWeight(e1/(e1+e2*r));
    }

    // loop again over all shared cells and add them to their
    // respective clusters
    hashCellIter= sharedCellList.begin();
    hashCellIterEnd=sharedCellList.end();
    for(;hashCellIter!=hashCellIterEnd;++hashCellIter) {
      CaloTopoSplitterClusterCell* pClusCell = hashCellIter->getCaloTopoTmpClusterCell();
      HashCluster *firstCluster = pClusCell->getCaloTopoTmpHashCluster();
      HashCluster *secondCluster = pClusCell->getSecondCaloTopoTmpHashCluster();
      firstCluster->add(*hashCellIter);
      secondCluster->add(*hashCellIter);
    }
    nShared = sharedCellList.size();
  }

  const DataLink<CaloCellContainer> myCellCollLink (myCellColl);
  
  // create cluster list for the purpose of sorting in E_t before storing 
  // in the cluster collection
  std::vector<std::unique_ptr<CaloProtoCluster> > myCaloClusters;
  myCaloClusters.reserve (500);
  std::vector<std::unique_ptr<CaloProtoCluster> > myRestClusters;
  myRestClusters.resize(clusColl->size());  // this has a 0 pointer as default!
  std::vector<HashCluster *>::iterator hashClusIter = myHashClusters.begin();
  std::vector<HashCluster *>::iterator hashClusIterEnd=myHashClusters.end();
  for (;hashClusIter!=hashClusIterEnd;++hashClusIter) {
    HashCluster * tmpCluster = (*hashClusIter);
    if ( tmpCluster->size() > 1 ) {
      // local maximum implies at least 2 cells are in the cluster ...
      std::unique_ptr<CaloProtoCluster> myCluster = std::make_unique<CaloProtoCluster>(myCellCollLink);
      ATH_MSG_DEBUG("[CaloCluster@" << myCluster.get() << "] created in <myCaloClusters>.");
      HashCluster::iterator clusCellIter=tmpCluster->begin();
      HashCluster::iterator clusCellIterEnd=tmpCluster->end();
      myCluster->getCellLinks()->reserve(tmpCluster->size());
      for(;clusCellIter!=clusCellIterEnd;++clusCellIter) {
	CaloTopoSplitterClusterCell *pClusCell =  *clusCellIter;
	xAOD::CaloCluster::cell_iterator itrCell = pClusCell->getCellIterator();
	double myWeight = itrCell.weight();//pClusCell->getParentCluster()->getCellWeight(itrCell);
	if ( pClusCell->getShared() ) {
	  if ( pClusCell->getCaloTopoTmpHashCluster() == tmpCluster ) 
	  myWeight *= pClusCell->getSharedWeight();
	else  
	  myWeight *= (1.-pClusCell->getSharedWeight());
	}	
	myCluster->addCell(itrCell.index(),myWeight);
      }
      //CaloClusterKineHelper::calculateKine(myCluster);
      ATH_MSG_DEBUG("[CaloCluster@" << myCluster.get() << "] size: " << myCluster->size());
      //myCluster->setClusterSize(clusterSize);
      myCaloClusters.push_back(std::move(myCluster));
    }
    else if ( tmpCluster->size() == 1 ) {
      // either cells belonging to a cluster with no local maximum
      // or in case there was a local maximum a non-connected part 
      // of the cluster (different cluster algo for parent cluster, or
      // in case of TopoClustering different neighbor option)
      if ( hasLocalMaxVector[tmpCluster->getParentClusterIndex()]) {
	// need to take care only of those with local max, as clusters without
	// any local max are copied anyways
	
	if (!myRestClusters[tmpCluster->getParentClusterIndex()]) {
	  myRestClusters[tmpCluster->getParentClusterIndex()] = std::make_unique<CaloProtoCluster>(myCellColl);
	}
	ATH_MSG_DEBUG("[CaloCluster@" << myRestClusters[tmpCluster->getParentClusterIndex()].get()  
		      << "] created in <myRestClusters>");
	myRestClusters[tmpCluster->getParentClusterIndex()]->getCellLinks()->reserve(tmpCluster->size());
	HashCluster::iterator clusCellIter=tmpCluster->begin();
	HashCluster::iterator clusCellIterEnd=tmpCluster->end();
	for(;clusCellIter!=clusCellIterEnd;++clusCellIter) {
	  CaloTopoSplitterClusterCell *pClusCell =  *clusCellIter;
	  xAOD::CaloCluster::cell_iterator itrCell = pClusCell->getCellIterator();
	  const double myWeight = itrCell.weight();
	  myRestClusters[tmpCluster->getParentClusterIndex()]->addCell(itrCell.index(),myWeight);
	}
	//CaloClusterKineHelper::calculateKine(myRestClusters[tmpCluster->getParentClusterIndex()]);
	ATH_MSG_DEBUG("[CaloCluster@" << myRestClusters[tmpCluster->getParentClusterIndex()].get()
		      << "] size: " << myRestClusters[tmpCluster->getParentClusterIndex()]->size());
	//myRestClusters[tmpCluster->getParentClusterIndex()]->setClusterSize(clusterSize);
      }
    }
  }

  // add the clusters which do not have any local max and the rest clusters 
  // to the list
  iClusterNumber = 0;
  clusCollIter    = clusColl->begin();
  for (; clusCollIter != clusCollIterEnd; ++clusCollIter,++iClusterNumber){
    const xAOD::CaloCluster* parentCluster = (*clusCollIter);
    if ( !hasLocalMaxVector[iClusterNumber] ) {
      //xAOD::CaloCluster *myClone = new xAOD::CaloCluster(*parentCluster);
      myCaloClusters.push_back(std::make_unique<CaloProtoCluster>(parentCluster->getCellLinks()));
      ATH_MSG_DEBUG("[CaloProtoCluster@" << myCaloClusters.back().get() << "] with " 
		    << myCaloClusters.back()->size() << "cells cloned from " 
		    << parentCluster << " with " << parentCluster->size() <<" cells");
    }
    else if (myRestClusters[iClusterNumber]) {
      ATH_MSG_DEBUG("[CaloCluster@" << myRestClusters[iClusterNumber].get()
		    << "] pushed into <myCaloClusters> with "
		    << myRestClusters[iClusterNumber]->size() << " cells");
      myCaloClusters.push_back(std::move(myRestClusters[iClusterNumber]));
    }
  }

  //CaloClusterEtSort::compare compareEt;
  std::sort(myCaloClusters.begin(),myCaloClusters.end(),[](const std::unique_ptr<CaloProtoCluster>& pc1, 
							   const std::unique_ptr<CaloProtoCluster>& pc2) {
      //As in CaloUtils/CaloClusterEtSort. 
      //assign to volatile to avoid excess precison on in FP unit on x386 machines
      volatile double et1(pc1->et());
      volatile double et2(pc2->et());
      return (et1 > et2);
    }
    );
  // remove all original clusters from the cluster container
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "erase " << clusColl->size() << " clusters";
  clusColl->clear();
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << ", new size " << clusColl->size() << endmsg;
  clusColl->reserve (myCaloClusters.size());

  float eTot(0.);
  int nTot(0);
  float eMax(0.);
  // add to cluster container.
  for(const auto& protoCluster : myCaloClusters) {
    xAOD::CaloCluster* xAODCluster=new xAOD::CaloCluster();
    clusColl->push_back(xAODCluster);
    xAODCluster->addCellLink(protoCluster->releaseCellLinks());//Hand over ownership to xAOD::CaloCluster
    xAODCluster->setClusterSize(clusterSize);
    CaloClusterKineHelper::calculateKine(xAODCluster);
    ATH_MSG_DEBUG("CaloCluster@" << xAODCluster << " pushed into "
		  << "CaloClusterContainer@" << clusColl);
  
    ATH_MSG_DEBUG("CaloClusterContainer@" << clusColl 
		  << "->size() = " << clusColl->size());
    ATH_MSG_DEBUG("CaloCluster E = " << xAODCluster->e() 
		  << " MeV, Et = " << xAODCluster->et() 
		  << " MeV, NCells = " << xAODCluster->size());
    eTot+=xAODCluster->e();
    nTot+=xAODCluster->size();
    
    if ( fabs(xAODCluster->e()) > eMax )
      eMax = fabs(xAODCluster->e());
  }
  ATH_MSG_DEBUG("Sum of all CaloClusters E = " << eTot
		<< " MeV, NCells = " << nTot 
		<< " (including NShared = " << nShared << " twice)");

  if ( fabs(eTot) > eMax ) 
    eMax = fabs(eTot);
  if ( fabs(eTot-eTotOrig)>0.001*eMax ){
    msg(MSG::WARNING) << "Energy sum for split Clusters = " << eTot << " MeV does not equal original sum = " << eTotOrig << " MeV !" << endmsg;
  } 
  if ( abs(nTot-nShared-nTotOrig) > 0 ) {
    msg(MSG::ERROR) <<"Cell sum for split Clusters does not equal original sum!" << endmsg;
  } 

  tmpcell_pool.erase();
  tmpclus_pool.erase();

  return StatusCode::SUCCESS;

}
