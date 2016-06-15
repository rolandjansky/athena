/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "egammaOQFlagsBuilder.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <algorithm> 
#include <math.h> 

#include "StoreGate/StoreGateSvc.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "LArTools/LArCablingService.h"
#include "Identifier/HWIdentifier.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "FourMomUtils/P4Helpers.h"
//  END OF HEADER FILES INCLUDE

using CLHEP::GeV;


// ===========================================================================
egammaOQFlagsBuilder::egammaOQFlagsBuilder(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
  : egammaBaseTool(type, name, parent),
    m_badChannelTool("LArBadChanTool"),
    m_larCablingSvc("LArCablingService"),
    m_affectedTool("CaloAffectedTool"),
    m_emHelper(0),
    m_cellcoll(0),
    m_cellCentrId(0),
    m_detStore(NULL)
{
  //
  // constructor
  //

  // declare interface
  declareInterface<IegammaBaseTool>(this);


  // The following properties are specified at run-time
  // (declared in jobOptions file)
  declareProperty("LArBadChannelTool",m_badChannelTool,"This is the larBadChannelTool");
  declareProperty("affectedTool"    , m_affectedTool); 
  declareProperty("CellsName",m_cellsName="AllCalo","Names of container which contain cells ");
  declareProperty("QCellCut", m_QCellCut = 4000.);
  declareProperty("QCellHECCut", m_QCellHECCut = 60000.);
  declareProperty("QCellSporCut", m_QCellSporCut = 4000.);
  declareProperty("LArQCut", m_LArQCut = 0.8);
  declareProperty("TCut", m_TCut = 10.0);
  declareProperty("TCutVsE", m_TCutVsE = 2.0);
  m_affRegVec = 0;
  m_calocellId = 0;
}

// ===============================================================
egammaOQFlagsBuilder::~egammaOQFlagsBuilder()
{ 
  //
  // destructor
  //
}

// ==================================================================
StatusCode egammaOQFlagsBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing egammaOQFlagsBuilder");
 
  StatusCode sc;
  
  // get DetectorStore service
  //StoreGateSvc* detStore;
  sc=service("DetectorStore",m_detStore);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } else {
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found DetectorStore" << endreq;
  }

  // Get BadChannelTool
  sc=m_badChannelTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve LArBadChannelTool " << m_badChannelTool << endreq;
    return StatusCode::FAILURE;
  } else {
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "LArBadChannelTool" << m_badChannelTool << " retrieved" << endreq;
  }
  

  //Get CaloAffectedTool
  sc = m_affectedTool.retrieve();
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve CaloAffectedTool " << m_affectedTool << endreq;
    return StatusCode::FAILURE;
  } else {
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CaloAffectedTool" << m_affectedTool << " retrieved" << endreq;
  }


  //Get LArCablingService
  sc=m_larCablingSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve LArCablingService " << m_larCablingSvc << endreq;
    return StatusCode::FAILURE;
  } else {
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "LArCablingService" << m_larCablingSvc << " retrieved" << endreq;
  }


  sc=m_detStore->retrieve(m_calocellId, "CaloCell_ID"); 
  if(sc.isFailure()){
    msg(MSG::WARNING) <<  "Cannot retrieve online_id" << endreq;
    return StatusCode::FAILURE;
  } else {
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CaloCell_ID" << m_calocellId << " retrieved" << endreq;
  }
  
  sc=m_detStore->retrieve(m_emHelper); 
  if(sc.isFailure()){
    msg(MSG::WARNING) <<   "Cannot retrieve online_id" << endreq;
    return StatusCode::FAILURE;
  } else {
    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CaloCell_ID  retrieved" << endreq;
  }


  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaOQFlagsBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}



// ===============================================================
bool egammaOQFlagsBuilder::findCentralCell(const xAOD::CaloCluster* cluster)
{

  bool thereIsACentrCell = false;

  //LOOP OVER CLUSTER TO FIND THE CENTRAL CELL IN S2
  xAOD::CaloCluster::const_cell_iterator  cellIter    = cluster->cell_begin();
  xAOD::CaloCluster::const_cell_iterator  cellIterEnd = cluster->cell_end();
  float clusEta = cluster->eta();
  float clusPhi = cluster->phi();
  float energymax = -999999.;

  for( ;cellIter!=cellIterEnd;cellIter++) {
    const CaloCell* cell = (*cellIter);
    if (!cell) continue;
    float eta = cell->eta();
    float phi = cell->phi();
    float energy = cell->energy();
    CaloSampling::CaloSample layer = cell->caloDDE()->getSampling() ;
    if(fabs(eta - clusEta)<0.025 && fabs(P4Helpers::deltaPhi(phi, clusPhi))<0.025 && (layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) && (energy>energymax)) { 
      energymax=energy;
      m_cellCentrId = cellIter->ID();
      thereIsACentrCell = true;
    }
  }
  return thereIsACentrCell;
}


// ===============================================================
bool egammaOQFlagsBuilder::isCore(Identifier Id, const std::vector<IdentifierHash>& neighbourList) const {
  const IdentifierHash hashId = m_calocellId->calo_cell_hash(Id);
  std::vector<IdentifierHash>::const_iterator it=std::find(neighbourList.begin(),neighbourList.end(),hashId);
  return (it!=neighbourList.end());
}


// ===============================================================
std::vector<IdentifierHash> egammaOQFlagsBuilder::findNeighbours(Identifier cellCentrId) const{
  std::vector<IdentifierHash> neighbourList;
  const IdentifierHash hashId = m_calocellId->calo_cell_hash(cellCentrId);
  m_emHelper->get_neighbours(hashId, LArNeighbours::all2D, neighbourList); 
  return  neighbourList;
}

// =====================================================================
StatusCode egammaOQFlagsBuilder::execute(xAOD::Egamma* eg)
{ 
  // Protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;
  const xAOD::CaloCluster* cluster = eg->caloCluster(); 
  if (!cluster) return StatusCode::SUCCESS; 
  if (cluster->size()<=0) return StatusCode::SUCCESS; 
  
  unsigned int iflag = 0;
  const float clusterEta = cluster->eta();
  const float clusterPhi = cluster->phi();

  //Find the central cell in the middle layer 
  bool foundCentralCell = egammaOQFlagsBuilder::findCentralCell(cluster);
  
  //=================== Set timing bit ===================================================//
  const double absEnergyGeV = fabs(cluster->e()*(1./GeV));
  if(absEnergyGeV != 0 &&  fabs(cluster->time()) > m_TCut + m_TCutVsE/absEnergyGeV) {
    iflag |= ( 0x1 << xAOD::EgammaParameters::OutTime);
  }
  //======================================================================================//

  //Declare totE and badE for LarQ cleaning
  double totE=0;
  double badE=0;

  if (foundCentralCell) {
    //Find the list of neighbours cells, to define the 3x3 cluster core
    std::vector<IdentifierHash> neighbourList = findNeighbours(m_cellCentrId);
    
    //Loop over all the cluster cells
    xAOD::CaloCluster::const_cell_iterator cellIter    = cluster->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = cluster->cell_end();
    for( ;cellIter!=cellIterEnd;cellIter++) {
      //Find cell parameters and properties
      const CaloCell* cell = (*cellIter);
      if(!cell) continue;
      float eta = cell->eta();
      // float phi = cell->phi(); // no longer used
      float qual = cell->quality();
      CaloSampling::CaloSample layer = cell->caloDDE()->getSampling()  ;

      //Calculate badE et totE    
      if( (cell->provenance()  & 0x2000) &&!(cell->provenance()  & 0x0800 )) {
	totE += cell->e();
	if(cell->quality() > m_QCellCut ) badE += cell->e(); 
      }       

      bool isACoreCell =  false;
      isACoreCell = isCore(cell->ID(),neighbourList);
      
      bool isStripCoreCell = false;
      if ( (layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) && fabs(eta-clusterEta)<0.025/2.) {
	isStripCoreCell = true;
      }
      
      //=======================   Set HEC bit ============================================//
      if (layer>=CaloSampling::HEC0 && layer<=CaloSampling::HEC3 && qual>m_QCellHECCut) {
	iflag |= ( 0x1 << xAOD::EgammaParameters::HECHighQ);
      }
      //==================================================================================//

      //======================== Set LAr bits ============================================================//
      HWIdentifier LArhwid = m_larCablingSvc->createSignalChannelIDFromHash(cell->caloDDE()->calo_hash());
      LArBadChannel bc = m_badChannelTool->status(LArhwid);      
      //      if (!bc.good()){  //If it is ok, skip all checks
      if(isACoreCell) {
	if((cell->provenance() & 0x0A00) == 0x0A00) { 
	  iflag |= ( 0x1 << xAOD::EgammaParameters::MissingFEBCellCore);
	}
	if((cell->provenance() & 0x0A00) == 0x0800) {
	  iflag |= ( 0x1 << xAOD::EgammaParameters::MaskedCellCore); 
	}
	if(bc.sporadicBurstNoise() && qual<m_QCellSporCut) {
	  iflag |= ( 0x1 << xAOD::EgammaParameters::SporadicNoiseLowQCore);
	}
	if(bc.deadCalib() || bc.lowNoiseHG() || bc.lowNoiseMG() ||
	   bc.lowNoiseLG() || bc.distorted() || bc.unstable() || 
	   bc.unstableNoiseHG() || bc.unstableNoiseMG() || 
	   bc.unstableNoiseLG() || bc.peculiarCalibrationLine() 
	   || bc.almostDead() ||  bc.shortProblem()) { 
	  iflag |= ( 0x1 << xAOD::EgammaParameters::AffectedCellCore);
	}
	if (qual>=4000) {
	  iflag |= ( 0x1 << xAOD::EgammaParameters::HighQCore);
	}
      }else{
	if((cell->provenance() & 0x0A00) == 0x0A00) { 
	  if( layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE) {
	    iflag |= ( 0x1 << xAOD::EgammaParameters::MissingFEBCellEdgePS);
	  }
	  if( layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) {
	    iflag |= ( 0x1 << xAOD::EgammaParameters::MissingFEBCellEdgeS1);
	    if (isStripCoreCell)  iflag |= ( 0x1 << xAOD::EgammaParameters::BadS1Core);
	  }
	  if( layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) iflag |= ( 0x1 << xAOD::EgammaParameters::MissingFEBCellEdgeS2);
	  if( layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) iflag |= ( 0x1 << xAOD::EgammaParameters::MissingFEBCellEdgeS3); 
	}
	if((cell->provenance() & 0x0A00)  == 0x0800) {
	  if( layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE){
	    iflag |= ( 0x1 << xAOD::EgammaParameters::MaskedCellEdgePS);
	  }
	  if( layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) {
	    iflag |= ( 0x1 << xAOD::EgammaParameters::MaskedCellEdgeS1);
	    if (isStripCoreCell)  iflag |= ( 0x1 << xAOD::EgammaParameters::BadS1Core);
	  }
	  if( layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) iflag |= ( 0x1 << xAOD::EgammaParameters::MaskedCellEdgeS2);
	  if( layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) iflag |= ( 0x1 << xAOD::EgammaParameters::MaskedCellEdgeS3); 
	}
	if(bc.sporadicBurstNoise() && qual<m_QCellSporCut) {
	  iflag |= ( 0x1 << xAOD::EgammaParameters::SporadicNoiseLowQEdge);
	}
	if(bc.deadCalib() || bc.lowNoiseHG() || bc.lowNoiseMG() ||
	   bc.lowNoiseLG() || bc.distorted() || bc.unstable() ||
	   bc.unstableNoiseHG() || bc.unstableNoiseMG() || 
	   bc.unstableNoiseLG() || bc.peculiarCalibrationLine() 
	   || bc.almostDead() ||  bc.shortProblem()) { 
	  if( layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE) {
	    iflag |= ( 0x1 << xAOD::EgammaParameters::AffectedCellEdgePS);
	  }
	  if( layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) iflag |= ( 0x1 << xAOD::EgammaParameters::AffectedCellEdgeS1);
	  if( layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) iflag |= ( 0x1 << xAOD::EgammaParameters::AffectedCellEdgeS2);
	  if( layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) iflag |= ( 0x1 << xAOD::EgammaParameters::AffectedCellEdgeS3);
	}
	if (qual>=4000) {
	  iflag |= ( 0x1 << xAOD::EgammaParameters::HighQEdge);
	}
      }
    }  // end loop over cells
    //====================================================================================================================//

    //==== Set LArQCleaning bit ===============//
    double egammaLArQCleaning=0;
    if(totE !=0) egammaLArQCleaning=badE/totE;
    if(egammaLArQCleaning>m_LArQCut){
      iflag |= (0x1 << xAOD::EgammaParameters::LArQCleaning);
    }
    //=========================================//
  } //close if found central cell

  //========================= Check the HV components ===================================================//
  float deta=0;
  float dphi=0;
  CaloSampling::CaloSample layer;

  //--------------> PRE SAMPLER
  deta=0.5*0.025*cluster->getClusterEtaSize();
  dphi=0.5*0.025*cluster->getClusterPhiSize();
  layer=CaloSampling::PreSamplerE;

  bool checkNNHV_PSE   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_PSE = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::PreSamplerB;

  bool checkNNHV_PSB   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_PSB = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  if(checkNNHV_PSE || checkNNHV_PSB)      iflag |= ( 0x1 << xAOD::EgammaParameters::NonNominalHVPS); 
  if(checkDEADHV_PSE || checkDEADHV_PSB)  iflag |= ( 0x1 << xAOD::EgammaParameters::DeadHVPS);


  //---------------> SAMPLING 2 : CLUSTER CORE
  layer=CaloSampling::EMB2;
  //   deta=0.;
  //   dphi=0.;
  deta=0.5*0.025*3.;
  dphi=0.5*0.025*3.;
  bool checkDEADHV_CORE_B = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::EME2;
  bool checkDEADHV_CORE_E = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  if(checkDEADHV_CORE_B || checkDEADHV_CORE_E)  iflag |= ( 0x1 << xAOD::EgammaParameters::DeadHVS1S2S3Core); 

  //----------------> SAMPLINGS 1,2,3 : CLUSTER EDGE
  deta=0.5*0.025*cluster->getClusterEtaSize();
  dphi=0.5*0.025*cluster->getClusterPhiSize();
  layer=CaloSampling::EMB1;

  bool checkNNHV_EMB1   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_EMB1 = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::EMB2;

  bool checkNNHV_EMB2   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_EMB2 = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::EMB3;

  bool checkNNHV_EMB3   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_EMB3 = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::EME1;

  bool checkNNHV_EME1   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_EME1 = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::EME2;

  bool checkNNHV_EME2   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_EME2 = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  layer=CaloSampling::EME3;

  bool checkNNHV_EME3   = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,1) ; //nnHVPS
  bool checkDEADHV_EME3 = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,2) ; //deadHVPS
  if(checkNNHV_EMB1 || checkNNHV_EME1 || checkNNHV_EMB2 || checkNNHV_EME2 ||checkNNHV_EMB3 || checkNNHV_EME3) 
    iflag |= ( 0x1 << xAOD::EgammaParameters::NonNominalHVS1S2S3); 
  if(checkDEADHV_EMB1 || checkDEADHV_EME1 || checkDEADHV_EMB2 || checkDEADHV_EME2 ||checkDEADHV_EMB3 || checkDEADHV_EME3)  
    iflag |= ( 0x1 << xAOD::EgammaParameters::DeadHVS1S2S3Edge);
  //========================================================================================================================//

  //====================  Check the tile component  ==================================//
  //Get CaloCellContainer
  StatusCode sc = evtStore()->retrieve(m_cellcoll, m_cellsName) ; 
  if(sc.isFailure() || !m_cellcoll) {
    ATH_MSG_WARNING("no Calo Cell Container " << m_cellsName << " found");
    return sc;
  }
  CaloCell_ID::SUBCALO HADCal = static_cast<CaloCell_ID::SUBCALO>(CaloCell_ID::TILE);
  CaloCellList ccl(m_cellcoll,HADCal); 
  double size = 0.12;
  //------------------------ TileBar0 --------------------------------//
  bool isbadtilebar0cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileBar0);
  //------------------------ TileExt0 --------------------------------//
  bool isbadtileext0cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileExt0);
  //----------------------------------------------------------------//

  if( isbadtilebar0cell || isbadtileext0cell ){
    iflag |= (0x1 << xAOD::EgammaParameters::DeadCellTileS0);    
  }
  //---------------------- TileBar1 ----------------------------------//
  bool isbadtilebar1cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileBar1);
    //---------------------- TileExt1 ----------------------------------//
  bool isbadtileext1cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileExt1);
    //---------------------- TileGap1 ----------------------------------//
  bool isbadtilegap1cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileGap1);
  //---------------------- TileBar2 ----------------------------------//
  bool isbadtilebar2cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileBar2);
    //---------------------- TileExt2 ---------------------------------//
  bool isbadtileext2cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileExt2);
    //---------------------- TileGap2 ----------------------------------//
  bool isbadtilegap2cell=isbadtilecell(ccl,clusterEta,clusterPhi,size,size,CaloSampling::TileGap2);
  //---------------------------------------------------------------//
  if( isbadtilebar1cell || isbadtilebar2cell ||
      isbadtileext1cell || isbadtileext2cell ||
      isbadtilegap1cell || isbadtilegap2cell ){
    iflag |= (0x1 << xAOD::EgammaParameters::DeadCellTileS1S2);    
  }

  //=================================================================================//
  eg->setOQ(iflag);
  ATH_MSG_DEBUG("Executing egammaOQFlagsBuilder::execute");
  return StatusCode::SUCCESS;
}

bool egammaOQFlagsBuilder::isbadtilecell ( CaloCellList& ccl, const float clusterEta, const float clusterPhi, 
					   const double sizeEta, const double sizePhi ,const CaloSampling::CaloSample sample) const {

  bool isbadtilecell = false;
  ccl.select(clusterEta,clusterPhi,sizeEta,sizePhi,sample);
  CaloCellList::list_iterator cclIter     = ccl.begin();
  CaloCellList::list_iterator cclIterEnd  = ccl.end();
  for( ;cclIter!=cclIterEnd;cclIter++) {
    const CaloCell* cell = (*cclIter);
    if(cell->badcell() == true){//check of bad tile cell
      isbadtilecell = true;
      break;
      }
  }
  return isbadtilecell;
}
