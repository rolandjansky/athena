/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoClusterMaker.cxx,v 1.44 2009-05-19 10:04:47 gunal Exp $
//
// Description: see CaloTopoClusterMaker.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloRec/CaloTopoClusterMaker.h"
#include "CaloRec/CaloTopoTmpClusterCell.h"
#include "CaloRec/CaloTopoTmpHashCluster.h"
#include "CaloRec/CaloTopoTmpHashCell.h"
#include "CaloRec/CaloTopoTmpHashCellSort.h"
#include "CaloRec/CaloBadCellHelper.h"
#include "CaloUtils/CaloClusterEtSort.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloPrefetch.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>

#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "CaloRec/CaloProtoCluster.h"

using CLHEP::MeV;
using CLHEP::ns;

namespace {

  /*
struct CaloClusterSort
{
  CaloClusterSort (xAOD::CaloCluster* cl, float et)
    : m_et (et), m_cl (cl) {}
  bool operator< (const CaloClusterSort& other) const
  { return m_et < other.m_et; }
  operator xAOD::CaloCluster* () const { return m_cl; }

  float m_et;
  xAOD::CaloCluster* m_cl;
};
  */

} // anonymous namespace

//#############################################################################

CaloTopoClusterMaker::CaloTopoClusterMaker(const std::string& type, 
					   const std::string& name,
					   const IInterface* parent)
  
  : AthAlgTool(type, name, parent),
    m_calo_dd_man(0),
    m_calo_id(0),
    m_subcaloUsed(),
    m_cellThresholdOnEorAbsEinSigma    (    0.),
    m_neighborThresholdOnEorAbsEinSigma(    3.),
    m_seedThresholdOnEorAbsEinSigma    (    6.),
    m_cellThresholdOnEtorAbsEt         (    0.*MeV),   
    m_neighborThresholdOnEtorAbsEt     (  100.*MeV), 
    m_seedThresholdOnEtorAbsEt         (  200.*MeV),
    m_seedThresholdOnTAbs              (  12.5*ns),
    m_useNoiseTool                     (false),
    m_usePileUpNoise                   (false),
    m_noiseTool                        ("CaloNoiseTool"),
    m_noiseSigma                       (  100.*MeV),                      
    m_neighborOption                   ("super3D"),
    m_nOption                          (LArNeighbours::super3D),
    m_restrictHECIWandFCalNeighbors    (false),
    m_restrictPSNeighbors              (false),
    m_seedCutsInAbsE                   (false),
    m_neighborCutsInAbsE               (true),
    m_cellCutsInAbsE                   (true),
    m_clusterEtorAbsEtCut              (    0.*MeV),
    m_seedCutsInT                      (false),
    m_cutOOTseed                       (false),
    m_twogaussiannoise                 (false),
    m_treatL1PredictedCellsAsGood      (true),
    m_minSampling                      (0),
    m_maxSampling                      (0),
    m_doALotOfPrintoutInFirstEvent     (false),
    m_hashMin                          (999999),
    m_hashMax                          (0),
    m_clusterSize                      ()
{
  declareInterface<CaloClusterCollectionProcessor> (this);
  // Name(s) of Cell Containers
  declareProperty("CellsName",m_cellsName);
  
  // Name(s) of Calorimeters to consider
  declareProperty("CalorimeterNames",m_caloNames);

  // Name(s) of Calorimeter Samplings to consider for seeds
  declareProperty("SeedSamplingNames",m_samplingNames);

  // Energy thresholds (in units of noise Sigma) 
  declareProperty("SeedThresholdOnEorAbsEinSigma",
		  m_seedThresholdOnEorAbsEinSigma);
  declareProperty("NeighborThresholdOnEorAbsEinSigma",
		  m_neighborThresholdOnEorAbsEinSigma);
  declareProperty("CellThresholdOnEorAbsEinSigma",
		  m_cellThresholdOnEorAbsEinSigma);
  // Additional E_t cuts in MeV in case usePileUpNoise is set to false
  declareProperty("SeedThresholdOnEtorAbsEt",
		  m_seedThresholdOnEtorAbsEt);
  declareProperty("NeighborThresholdOnEtorAbsEt",
		  m_neighborThresholdOnEtorAbsEt);
  declareProperty("CellThresholdOnEtorAbsEt",
		  m_cellThresholdOnEtorAbsEt);

  // Time thresholds (in abs. val.)
  declareProperty("SeedThresholdOnTAbs",m_seedThresholdOnTAbs);

  // Seed and cluster cuts are in E or Abs E
  declareProperty("SeedCutsInAbsE",m_seedCutsInAbsE);

  // Neighbor cuts are in E or Abs E
  declareProperty("NeighborCutsInAbsE",m_neighborCutsInAbsE);

  // Cell cuts are in E or Abs E
  declareProperty("CellCutsInAbsE",m_cellCutsInAbsE);

  //do Seed cuts on Time              
  declareProperty("SeedCutsInT",m_seedCutsInT);
  //exclude out-of-time seeds from neighbouring and cell stage              
  declareProperty("CutOOTseed",m_cutOOTseed);

  // Noise Sigma
  declareProperty("NoiseSigma",m_noiseSigma);

  // NoiseTool
  declareProperty("UseCaloNoiseTool",m_useNoiseTool);
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");

  // PileUpNoise
  declareProperty("UsePileUpNoise",m_usePileUpNoise);

  // Neighbor Option
  declareProperty("NeighborOption",m_neighborOption);

  // Restrict HEC IW and FCal Neighbors
  declareProperty("RestrictHECIWandFCalNeighbors",m_restrictHECIWandFCalNeighbors);

  // Restrict PS Neighbors
  declareProperty("RestrictPSNeighbors",m_restrictPSNeighbors);

  // Cluster E_t or Abs E_t cut
  declareProperty("ClusterEtorAbsEtCut",m_clusterEtorAbsEtCut);

  // use 2-gaussian noise for Tile
  declareProperty("TwoGaussianNoise",m_twogaussiannoise);

  // Treat bad cells with dead OTX if predicted from L1 as good
  declareProperty("TreatL1PredictedCellsAsGood",m_treatL1PredictedCellsAsGood);
}

//#############################################################################

StatusCode CaloTopoClusterMaker::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endreq;
  msg(MSG::INFO) << "Treat L1 Predicted Bad Cells as Good set to" << ((m_treatL1PredictedCellsAsGood) ? "true" : "false") << endreq;
  msg(MSG::INFO) << "Two-Gaussian noise for Tile set to " << ((m_twogaussiannoise) ? "true" : "false") << endreq;

  getClusterSize();

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &CaloTopoClusterMaker::geoInit,this);
    if(sc.isFailure())
    {
      msg(MSG::FATAL) << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }

  return sc;
}

StatusCode
CaloTopoClusterMaker::geoInit(IOVSVC_CALLBACK_ARGS)
{

  //MsgStream log(msgSvc(), name());
  m_doALotOfPrintoutInFirstEvent = false;

  // pointer to detector manager:
  m_calo_dd_man  = CaloDetDescrManager::instance(); 

  m_calo_id   = m_calo_dd_man->getCaloCell_ID();

  msg(MSG::INFO) << "Threshold choices:"
      << (m_seedCutsInAbsE?" SeedThresholdOnAbsEinSigma=":
	  " SeedThresholdOnEinSigma=")
      << m_seedThresholdOnEorAbsEinSigma
      << (m_neighborCutsInAbsE?", NeighborThresholdOnAbsEinSigma=":
	  ", NeighborThresholdOnEinSigma=")
      << m_neighborThresholdOnEorAbsEinSigma
      << (m_cellCutsInAbsE?", CellThresholdOnAbsEinSigma=":
	  ", CellThresholdOnEinSigma=")
      << m_cellThresholdOnEorAbsEinSigma
      << endreq;

  //--- set Neighbor Option

  if ( m_neighborOption == "all2D" ) 
    m_nOption = LArNeighbours::all2D;
  else if ( m_neighborOption == "all3D" ) 
    m_nOption = LArNeighbours::all3D;
  else if ( m_neighborOption == "super3D" ) 
    m_nOption = LArNeighbours::super3D;
  else {
    msg(MSG::ERROR) << "Invalid Neighbor Option "
	<< m_neighborOption << ", exiting ..." << endreq;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Neighbor Option "
      << m_neighborOption << " is selected!" << endreq;

  //--- check calorimeter names to use
  std::vector<std::string>::iterator caloIter = m_caloNames.begin(); 
  std::vector<std::string>::iterator caloIterEnd = m_caloNames.end(); 
  for(; caloIter!=caloIterEnd; caloIter++) { 
    if ( *caloIter == "LAREM" ) 
      m_subcaloUsed[CaloCell_ID::LAREM] = true;
    else if ( *caloIter == "LARHEC" )
      m_subcaloUsed[CaloCell_ID::LARHEC] = true;
    else if ( *caloIter == "LARFCAL" )
      m_subcaloUsed[CaloCell_ID::LARFCAL] = true;
    else if ( *caloIter == "TILE" )
      m_subcaloUsed[CaloCell_ID::TILE] = true;
    else 
      msg(MSG::ERROR) << "Calorimeter " << *caloIter 
	  << " is not a valid Calorimeter name and will be ignored! "
	  << "Valid names are: LAREM, LARHEC, LARFCAL, and TILE." << endreq;
  }

  //--- check sampling names to use for seeds
  std::vector<std::string>::iterator samplingIter = m_samplingNames.begin(); 
  std::vector<std::string>::iterator samplingIterEnd = m_samplingNames.end(); 
  for(; samplingIter!=samplingIterEnd; samplingIter++) { 
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
      msg(MSG::ERROR) << "Calorimeter sampling" << *samplingIter 
	  << " is not a valid Calorimeter sampling name and will be ignored! "
	  << "Valid names are: "
	  << "PreSamplerB, EMB1, EMB2, EMB3, "
	  << "PreSamplerE, EME1, EME2, EME3, "
          << "HEC0, HEC1, HEC2, HEC3, "
          << "TileBar0, TileBar1, TileBar2, "
          << "TileGap1, TileGap2, TileGap3, "
          << "TileExt0, TileExt1, TileExt2, "
          << "FCAL0, FCAL1, FCAL2." << endreq;
  }

  msg(MSG::INFO) << "Samplings to consider for seeds:";
  samplingIter = m_samplingNames.begin(); 
  for(; samplingIter!=samplingIterEnd; samplingIter++)  
    msg() << " " << *samplingIter;
  msg() << endreq;

  m_minSampling=0;
  m_maxSampling=0;
  std::set<int>::const_iterator vSamplingIter = m_validSamplings.begin(); 
  std::set<int>::const_iterator vSamplingIterEnd = m_validSamplings.end(); 
  for(; vSamplingIter!=vSamplingIterEnd; vSamplingIter++) {
    if ( (*vSamplingIter) > m_maxSampling ) 
      m_maxSampling = (*vSamplingIter);
    if ( (*vSamplingIter) < m_minSampling ) 
      m_minSampling = (*vSamplingIter);
  }

  m_useSampling.resize(m_maxSampling-m_minSampling+1,false);

  for(vSamplingIter = m_validSamplings.begin(); vSamplingIter!=vSamplingIterEnd; vSamplingIter++) {
    m_useSampling[(*vSamplingIter)-m_minSampling] = true;
  }

  msg(MSG::INFO) << "CellCollection to use: " << m_cellsName << endreq;

  msg(MSG::INFO) << "Calorimeters to consider:";
  caloIter = m_caloNames.begin(); 
  for(; caloIter!=caloIterEnd; caloIter++)  
    msg() << " " << *caloIter;
  msg() << endreq;

  //---- retrieve the noise tool ----------------
  
  if (m_useNoiseTool) {
    
    if(m_noiseTool.retrieve().isFailure()){
      msg(MSG::WARNING)
	  << "Unable to find Noise Tool" << endreq;
    }  
    else {
      msg(MSG::INFO) << "Noise Tool retrieved" << endreq;
    }
  }
  else  {
    msg(MSG::INFO) << "Noise Sigma "
	<< m_noiseSigma << " MeV is selected!" 
	<< (!m_usePileUpNoise?
	    " The noise sigma will just be the electronics noise!":"") 
	<< endreq;
  }

  if ( m_useNoiseTool && m_usePileUpNoise ) {
    msg(MSG::INFO) << "Pile-Up Noise from Noise Tool" 
	<< " is selected! The noise sigma will be the"
        << " quadratic sum of the electronics noise and the pile up!" << endreq;
  }
  else {
    msg(MSG::INFO) << "Additional E_t cuts (instead of Pile-Up Noise):" 
	<< (m_seedCutsInAbsE?" SeedThresholdOnAbsEt=":" SeedThresholdOnEt=")
	<< m_seedThresholdOnEtorAbsEt 
	<< " MeV, " 
	<< (m_neighborCutsInAbsE?"NeighborThresholdOnAbsEt=":
	    "NeighborThresholdOnEt=")
	<< m_neighborThresholdOnEtorAbsEt 
	<< " MeV, "
	<< (m_cellCutsInAbsE?"CellThresholdOnAbsEt=":
	    "CellThresholdOnEt=")
	<< m_cellThresholdOnEtorAbsEt 
	<< " MeV" << endreq;
  }

  msg(MSG::INFO) << (m_seedCutsInAbsE?"ClusterAbsEtCut= ":"ClusterEtCut= ")
      << m_clusterEtorAbsEtCut << " MeV" <<endreq;  

  m_hashMin = 999999;
  m_hashMax = 0;
  for (int subdet = 0; subdet < CaloCell_ID::NSUBCALO; ++subdet) {
    if (m_subcaloUsed[subdet]) {
      IdentifierHash thismin, thismax;
      m_calo_id->calo_cell_hash_range ((CaloCell_ID::SUBCALO)subdet,
                                       thismin, thismax);
      m_hashMin = std::min (m_hashMin, thismin);
      m_hashMax = std::max (m_hashMax, thismax);
    }
  }

  return StatusCode::SUCCESS;
  
}

//#############################################################################

StatusCode CaloTopoClusterMaker::execute(xAOD::CaloClusterContainer* clusColl)
{
  // minimal significance - should be > 0 in order to avoid 
  // throwing away of bad cells
  const float epsilon = 0.00001;

  StatusCode sc;
  //ATH_MSG_DEBUG( "Executing " << name());

  typedef CaloTopoTmpHashCell<CaloTopoTmpClusterCell> HashCell;
  typedef CaloTopoTmpHashCluster HashCluster;

  CaloPrefetch::ArenaHandlePrefetch<CaloTopoTmpClusterCell, SG::ArenaPoolAllocator, 2> tmpcell_pool;
  CaloPrefetch::ArenaHandlePrefetch<HashCluster,            SG::ArenaPoolAllocator, 2> tmpclus_pool;
  CaloTopoTmpHashCluster::pool_type                               tmplist_pool;

  // create cell list for cells above seed  cut (for seed growing algo)
  std::vector<HashCell> mySeedCells;
  mySeedCells.reserve (2000);
  // create initial cluster list (one cell per cluster)
  std::vector<HashCluster *> myHashClusters;
  myHashClusters.reserve (10000);

  // create vector to hold all cells.
  std::vector<HashCell> cellVector (m_hashMax - m_hashMin);
  HashCell* hashCells = cellVector.data() - m_hashMin;

  //---- Get the CellContainers ----------------

  //  for (const std::string& cellsName : m_cellsNames) {
  const CaloCellContainer * cellColl = nullptr;
  sc = evtStore()->retrieve(cellColl,m_cellsName); 

  if( !sc.isSuccess() || !cellColl){ 
    msg(MSG::ERROR) << " Can not retrieve CaloCellContainer: "
		    << m_cellsName << endreq; 
    return StatusCode::RECOVERABLE;      
  }    

  const DataLink<CaloCellContainer> cellCollLink (m_cellsName);

  //ATH_MSG_DEBUG("CaloCell container: "<< cellsName 
  //		  <<" contains " << cellColl->size() << " cells");

  for (int isubdet = 0; isubdet < CaloCell_ID::NSUBCALO; ++isubdet) {
    CaloCell_ID::SUBCALO subdet = (CaloCell_ID::SUBCALO)isubdet;
    if (m_subcaloUsed[subdet] && cellColl->hasCalo(subdet)) {
      auto cellIter = cellColl->beginConstCalo (subdet);
      auto cellIterEnd = cellColl->endConstCalo (subdet);
      for (int iCell = cellColl->indexFirstCellCalo(subdet);
	   cellIter != cellIterEnd;
	   ++iCell, ++cellIter)
        {
          CaloPrefetch::nextDDE(cellIter, cellIterEnd, 2);
	  // noise() member does not exist for CaloCell - need to use noise tool
	  const CaloCell* pCell = *cellIter;
	  // take the noise for the current gain of the cell
	  // the highest gain would be nice to use, but this is not possible 
	  // in runs with fixed gain because it might not be defined ...
	  float noiseSigma;
	  if ( m_useNoiseTool ) {
	    if ( m_usePileUpNoise ) {
	      if(m_twogaussiannoise) noiseSigma = m_noiseTool->getEffectiveSigma(pCell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::TOTALNOISE);
	      else noiseSigma = m_noiseTool->getNoise(pCell,ICalorimeterNoiseTool::TOTALNOISE);
	    }
	    else
	      if(m_twogaussiannoise) noiseSigma = m_noiseTool->getEffectiveSigma(pCell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::ELECTRONICNOISE);
	      else noiseSigma = m_noiseTool->getNoise(pCell,ICalorimeterNoiseTool::ELECTRONICNOISE);
	  }
	  else 
	    noiseSigma = m_noiseSigma;
	  float signedE = pCell->energy();
	  float signedEt = pCell->et();
	  float signedRatio = epsilon; // not 0 in order to keep bad cells 
	  if ( finite(noiseSigma) && noiseSigma > 0 && !CaloBadCellHelper::isBad(pCell,m_treatL1PredictedCellsAsGood) ) 
	    signedRatio = signedE/noiseSigma;

	  //apply energy cuts
	  bool passedCellCut = (m_cellCutsInAbsE?std::abs(signedRatio):signedRatio)
	    > m_cellThresholdOnEorAbsEinSigma 
	    && (m_usePileUpNoise || ((m_cellCutsInAbsE?std::abs(signedEt):signedEt)
				     > m_cellThresholdOnEtorAbsEt));
	  bool passedNeighborCut = (m_neighborCutsInAbsE?std::abs(signedRatio):signedRatio) > m_neighborThresholdOnEorAbsEinSigma
	    && ( m_usePileUpNoise || ((m_neighborCutsInAbsE?std::abs(signedEt):signedEt) > m_neighborThresholdOnEtorAbsEt ));
	  bool passedSeedCut = (m_seedCutsInAbsE?std::abs(signedRatio):signedRatio) > m_seedThresholdOnEorAbsEinSigma
	    && ( m_usePileUpNoise || ((m_seedCutsInAbsE?std::abs(signedEt):signedEt) > m_seedThresholdOnEtorAbsEt ));



	  bool passTimeCut_seedCell = (!m_seedCutsInT || passCellTimeCut(pCell,m_seedThresholdOnTAbs));
	  bool passedSeedAndTimeCut = (passedSeedCut && passTimeCut_seedCell); //time cut is applied here

	  bool passedNeighborAndTimeCut = passedNeighborCut;
	  if(m_cutOOTseed && passedSeedCut && !passTimeCut_seedCell) passedNeighborAndTimeCut=false; //exclude Out-Of-Time seeds from neighbouring stage as well (if required)

	  bool passedCellAndTimeCut = passedCellCut;
	  if(m_cutOOTseed && passedSeedCut && !passTimeCut_seedCell) passedCellAndTimeCut=false; //exclude Out-Of-Time seeds from cluster (if required)
				       

	  if ( passedCellAndTimeCut || passedNeighborAndTimeCut || passedSeedAndTimeCut ) {
	    const CaloDetDescrElement* dde = pCell->caloDDE();
	    IdentifierHash hashid = dde ? dde->calo_hash() : m_calo_id->calo_cell_hash(pCell->ID());
	    CaloTopoTmpClusterCell *tmpClusterCell =
              new (tmpcell_pool.allocate())
              CaloTopoTmpClusterCell(hashid,subdet,iCell,signedRatio,signedEt);
#if 0
	    // some debug printout - can also be used to construct neighbor
	    // tables offline ...
	    if ( m_doALotOfPrintoutInFirstEvent ) {
	      ATH_MSG_DEBUG( " [ExtId|Id|SubDet|HashId|eta|phi|E/noise|Et]: "
			     << "[" << m_calo_id->show_to_string(pCell->ID(),0,'/')
			     << "|" << mypCell->ID().getString()
			     << "|" << subdet
			     << "|" << (unsigned int)hashid
			     << "|" << pCell->eta()
			     << "|" << pCell->phi()
			     << "|" << signedRatio
			     << "|" << signedEt
			     << "]");
	  
	    }
#endif
	    HashCell hashCell(tmpClusterCell);
	    if ( passedNeighborAndTimeCut || passedSeedAndTimeCut ) {
	      HashCluster *tmpCluster =
                new (tmpclus_pool.allocate()) HashCluster (tmplist_pool);
	      tmpClusterCell->setCaloTopoTmpHashCluster(tmpCluster);
	      tmpCluster->add(hashCell);
	      myHashClusters.push_back(tmpCluster);
	      int caloSample = dde ? dde->getSampling() : m_calo_id->calo_sample(pCell->ID());
	      if ( passedSeedAndTimeCut
		   && caloSample >= m_minSampling 
		   && caloSample <= m_maxSampling 
		   && m_useSampling[caloSample-m_minSampling]) {
		tmpClusterCell->setUsed();
		mySeedCells.push_back(hashCell);
	      }
	    }
	    hashCells[hashid] = hashCell;
	  }
	}//end loop over cells
    }//end if use subcalo
  }//end loop over subcalos


  // sort initial seed cells to start with the cell of largest S/N
  // this makes the resulting clusters independent of the initial
  // ordering of the cells 
  if ( m_seedCutsInAbsE) {
    CaloTopoTmpHashCellSort::compareAbs<CaloTopoTmpClusterCell> compareSoverN;
    std::sort(mySeedCells.begin(),mySeedCells.end(),compareSoverN); 
  }
  else {
    CaloTopoTmpHashCellSort::compare<CaloTopoTmpClusterCell> compareSoverN;
    std::sort(mySeedCells.begin(),mySeedCells.end(),compareSoverN); 
  }

#if 1
  if (msgLvl(MSG::DEBUG)) {
    for (const HashCell& hc : mySeedCells) {
      msg(MSG::DEBUG) << " SeedCell [" 
		      << hc.getCaloTopoTmpClusterCell()->getSubDet() 
		      << "|" 
		      << (unsigned int)hc.getCaloTopoTmpClusterCell()->getID() 
		      << "] has S/N = " 
		      << hc.getCaloTopoTmpClusterCell()->getSignedRatio() 
		      << endreq;
    }
  }
#endif

  std::vector<HashCell> myNextCells;
  myNextCells.reserve (1000);

  std::vector<IdentifierHash> theNeighbors;
  theNeighbors.reserve(22);
#if 0
  std::vector<IdentifierHash> theNNeighbors;
  theNNeighbors.reserve(22);
#endif

  bool doRestrictHECIWandFCal = m_restrictHECIWandFCalNeighbors &&
    (m_nOption & LArNeighbours::nextInSamp);

  bool doRestrictPS = m_restrictPSNeighbors && 
    (m_nOption & LArNeighbours::nextInSamp);

  while ( !mySeedCells.empty() ) {
    // create cell list for next neighbor cells to consider
    myNextCells.clear();

    // loop over all current neighbor cells (for Seed Growing Algo)
    for (HashCell& hc : mySeedCells) {
      CaloTopoTmpClusterCell* pCell= hc.getCaloTopoTmpClusterCell();
      IdentifierHash hashid = pCell->getID();
      HashCluster *myCluster = pCell->getCaloTopoTmpHashCluster();
      CaloCell_ID::SUBCALO mySubDet = pCell->getSubDet();
      // in case we use all3d or super3D and the current cell is in the 
      // HEC IW or FCal2 & 3 or PS and we want to restrict their neighbors, 
      // use only next in sampling neighbors 
      LArNeighbours::neighbourOption opt = m_nOption;
      if (( mySubDet != CaloCell_ID::LAREM &&
	    doRestrictHECIWandFCal &&
	    ( ( mySubDet == CaloCell_ID::LARHEC  &&
		m_calo_id->region(m_calo_id->cell_id(hashid)) == 1 )  ||
	      ( mySubDet == CaloCell_ID::LARFCAL &&
		m_calo_id->sampling(m_calo_id->cell_id(hashid)) > 1 ) ) ) ||
	  ( doRestrictPS && 
	    ( ( mySubDet == CaloCell_ID::LAREM && 
		m_calo_id->sampling(m_calo_id->cell_id(hashid)) == 0 ) ) ) ) {
	opt = LArNeighbours::nextInSamp;
      }
      m_calo_id->get_neighbours(hashid,opt,theNeighbors);
#if 0
      if ( m_doALotOfPrintoutInFirstEvent && msgLvl(MSG::DEBUG)) {
	Identifier myId;
	myId = m_calo_id->cell_id((int)(mySubDet),hashid);
	msg(MSG::DEBUG) << " Cell [" << mySubDet << "|" 
			<< (unsigned int)hashid << "|"
			<< m_calo_id->show_to_string(myId,0,'/') 
			<< "] has " << theNeighbors.size() 
			<< " neighbors:" << endreq; 
      }
#endif
      // loop over all neighbors of that cell (Seed Growing Algo)
      for (IdentifierHash nId : theNeighbors) {
        CaloCell_ID::SUBCALO otherSubDet =
          (CaloCell_ID::SUBCALO)m_calo_id->sub_calo(nId);
	if ( m_subcaloUsed[otherSubDet] ) {
#if 0
	  if ( m_doALotOfPrintoutInFirstEvent && msgLvl(MSG::DEBUG)) {
	    Identifier myId = m_calo_id->cell_id(nId);
	    msg(MSG::DEBUG) <<  "  NeighborCell [" << otherSubDet << "|" 
			    << (unsigned int) nId << "|" 
			    << m_calo_id->show_to_string(myId,0,'/') << "]" 
			    << endreq;

	    m_calo_id->get_neighbours(nId,m_nOption,theNNeighbors);
	    if ( std::find (theNNeighbors.begin(),
                            theNNeighbors.end(), hashid) ==theNNeighbors.end() )
            {
	      myId = m_calo_id->cell_id(hashid);
	      msg(MSG::ERROR) << " Cell [" << mySubDet << "|" 
			      << (unsigned int)hashid << "|"
			      << m_calo_id->show_to_string(myId,0,'/') 
			      << "] has bad neighbor cell[";
	      myId = m_calo_id->cell_id(nId);
	      
	      msg() << otherSubDet << "|" << nId << "|" 
		    << m_calo_id->show_to_string(myId,0,'/') 
		    << "]" << endreq;
	    }
	  }
#endif
	  HashCell neighborCell = hashCells[nId];
	  if ( neighborCell.getCaloTopoTmpClusterCell() ) {
	    CaloTopoTmpClusterCell* pNCell =
              neighborCell.getCaloTopoTmpClusterCell();
	    // check neighbor threshold only since seed cells are already in
	    // the original list 
	    bool isAboveNeighborThreshold = 
	      (m_neighborCutsInAbsE?std::abs(pNCell->getSignedRatio()):pNCell->getSignedRatio()) > m_neighborThresholdOnEorAbsEinSigma
	      && ( m_usePileUpNoise || ((m_neighborCutsInAbsE?std::abs(pNCell->getSignedEt()):pNCell->getSignedEt()) > m_neighborThresholdOnEtorAbsEt ));
	    // checking the neighbors
	    if ( isAboveNeighborThreshold && !pNCell->getUsed() ) {
	      pNCell->setUsed();
	      myNextCells.push_back(neighborCell);
	    }
	    HashCluster *otherCluster = pNCell->getCaloTopoTmpHashCluster();
	    if ( myCluster != otherCluster ) {
	      HashCluster *toKill = 0;
	      HashCluster *toKeep = 0;
	      if ( !otherCluster || isAboveNeighborThreshold ) {
		if ( !otherCluster || otherCluster->size() < myCluster->size() ) {
		  toKill = otherCluster;
		  toKeep = myCluster;
		}
		else {
		  toKill = myCluster;
		  toKeep = otherCluster;
		}
		if ( toKill ) {
                  for (auto hc : *toKill)
                    hc->setCaloTopoTmpHashCluster(toKeep);
		  toKeep->add(*toKill);
		  toKill->removeAll();
		}
		else {
		  toKeep->add(neighborCell);
		  pNCell->setCaloTopoTmpHashCluster(toKeep);
		}
		myCluster = toKeep;
	      }
	    }
	  }
	}
      }
    }
    mySeedCells.swap (myNextCells);
  }


  //Create temporary list of proto-clusters 
  //Clusters below Et cut will be dropped. 
  //The remaining clusters will be sorted in E_t before storing 
  std::vector<CaloProtoCluster*> sortClusters;
  sortClusters.reserve (myHashClusters.size());

  for (HashCluster* tmpCluster : myHashClusters) {
    bool addCluster(false);
    if ( tmpCluster->size() > 1 )
      addCluster = true;
    else if ( tmpCluster->size() == 1 ) {
      // need to check if seed cell was good
      HashCluster::iterator clusCellIter=tmpCluster->begin();
      if ( clusCellIter->getUsed() )
	addCluster = true;
    }
    if ( addCluster) {
      CaloProtoCluster* myCluster = new CaloProtoCluster(cellCollLink);
      myCluster->getCellLinks()->reserve(tmpCluster->size());

      for (CaloTopoTmpClusterCell* cell : *tmpCluster) {
	size_t iCell = cell->getCaloCell();
	myCluster->addCell(iCell,1.);
      }
      float cl_et = myCluster->et();
      if ( (m_seedCutsInAbsE ? std::abs(cl_et) : cl_et) > m_clusterEtorAbsEtCut ) {
	sortClusters.push_back(myCluster);
      } 
      else {
	delete myCluster;
      }
    }
  }

  // Sort the clusters according to Et 
  std::sort(sortClusters.begin(),sortClusters.end(),[](CaloProtoCluster* pc1, CaloProtoCluster* pc2) {
      //As in CaloUtils/CaloClusterEtSort. 
      //assign to volatile to avoid excess precison on in FP unit on x386 machines
      volatile double et1(pc1->et());
      volatile double et2(pc2->et());
      //return (et1 < et2);  //This is the order we had from CaloRec-02-13-11 to  CaloRec-03-00-31
      return (et1 > et2); //This is the order we should have
    }
    );
 // add to cluster container
  clusColl->reserve(sortClusters.size());

  for (CaloProtoCluster* protoCluster: sortClusters) {
    xAOD::CaloCluster* xAODCluster=new xAOD::CaloCluster();
    clusColl->push_back(xAODCluster);
    xAODCluster->addCellLink(protoCluster->releaseCellLinks());//Hand over ownership to xAOD::CaloCluster
    delete protoCluster;
    xAODCluster->setClusterSize(m_clusterSize);
    CaloClusterKineHelper::calculateKine(xAODCluster,false,true); //No weight at this point! 
  }
  
  if ( m_doALotOfPrintoutInFirstEvent ) 
    m_doALotOfPrintoutInFirstEvent = false;

  tmpclus_pool.erase();
  tmpcell_pool.erase();

  return StatusCode::SUCCESS;
}

void CaloTopoClusterMaker::getClusterSize(){
  m_clusterSize = xAOD::CaloCluster::CSize_Unknown;

  if(m_seedThresholdOnEorAbsEinSigma==6. 
     && m_neighborThresholdOnEorAbsEinSigma==3. 
     && m_cellThresholdOnEorAbsEinSigma==3.){
    
    m_clusterSize = xAOD::CaloCluster::Topo_633;
    
  }else if(m_seedThresholdOnEorAbsEinSigma==4. 
	   && m_neighborThresholdOnEorAbsEinSigma==2. 
	   && m_cellThresholdOnEorAbsEinSigma==0.){
    
    m_clusterSize = xAOD::CaloCluster::Topo_420;    
  }
  ATH_MSG_DEBUG( "Cluster size = " << m_clusterSize);  
  return;
}



inline bool CaloTopoClusterMaker::passCellTimeCut(const CaloCell* pCell,float threshold) const 
{
  // get the cell time to cut on (the same as in CaloEvent/CaloCluster.h)                             

  // need sampling number already for time                                                        
  CaloSampling::CaloSample sam = pCell->caloDDE()->getSampling();
  // check for unknown sampling                                                                   
  if ( sam != CaloSampling::Unknown )
    {
      unsigned pmask = 0;
      if (sam != CaloSampling::PreSamplerB && sam != CaloSampling::PreSamplerE)
	{
	  if ( pCell->caloDDE()->is_tile() )
	    pmask = 0x8080;
	  else
	    pmask = 0x2000; //0x2000 is used to tell that time and quality information are available for this channel  (from TWiki: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/CaloEventDataModel#The_Raw_Data_Model)                                                                                         
	    
	  // Is time defined?                                                                         
	  if(pCell->provenance() & pmask)
	    {
	      return std::abs(pCell->time())<threshold;
	    }
	}
    }
  return true;
}
