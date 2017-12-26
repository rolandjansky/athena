/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class TBTree_CaloClusterH6 // make ROOT Tree for H6 CaloCluster
//
// author: A.Minaenko
// date:  25/07/2006

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "PathResolver/PathResolver.h"

#include <TFile.h>
#include <TTree.h>

#include "StoreGate/StoreGate.h" 

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
//#include "CaloUtils/ICaloNoiseTool.h"
#include "CaloInterface/ICaloNoiseTool.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArCabling/LArCablingService.h"

#include "TBEvent/TBEventInfo.h"
#include "TBEvent/TBTrack.h"
#include "TBEvent/TBTailCatcher.h"

#include "TBRec/TBH6RunHeader.h"
#include "TBRec/TBTree_CaloClusterH6.h"

#include <fstream>

TBTree_CaloClusterH6::TBTree_CaloClusterH6(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator), m_suffix(""), 
  m_nEvent(0),
  m_nEventRejected(0),
  m_nEventAccepted(0),
  m_nEventRandomTrigger(0),
  m_addNoise(true),
  m_addMoments(false),
  m_addGain(false),
  m_addTime(false),
  m_addQuality(false),
  m_addBeamTrack(false),
  m_addWTC(false), 
  m_useEMTBCluster(false),
  m_first(true),
  m_nRun(0),
  m_beamMom(0),
  m_xCryo(-9999), m_yTable(-9999), m_zCalo(30000),
  m_evType(0),
  m_eTotal(0),
  m_etaTotal(0),
  m_phiTotal(0),
  m_eEME2(0),
  m_eEME3(0),
  m_eHEC0(0),
  m_eHEC1(0),
  m_eHEC2(0),
  m_eFCAL0(0),
  m_eFCAL1(0),
  m_eFCAL2(0),
  m_nClusters(0),
  m_nCellCluster(0),
  m_eCluster(0),
  m_etaCluster (0),
  m_phiCluster (0),
  m_m1_eta(0),
  m_m1_phi(0),
  m_m2_r(0),
  m_m2_lambda(0),
  m_delta_phi(0),
  m_delta_theta(0),
  m_delta_alpha(0),
  m_center_x(0),
  m_center_y(0),
  m_center_z(0),
  m_center_lambda(0),
  m_lateral(0),
  m_longitudinal(0),
  m_eng_frac_em(0),
  m_eng_frac_max(0),
  m_eng_frac_core(0),
  m_m1_dens(0),
  m_m2_dens(0),
  m_nCells(0),
  m_cell_id(0),
  m_cell_ind_cluster(0),
  m_cell_energy(0),
  m_cell_noise(0),
  m_cell_gain(0),
  m_cell_time(0),
  m_cell_quality(0),
  m_beam_coor_x(0),
  m_beam_coor_y(0),
  m_beam_chi2_x(0),
  m_beam_chi2_y(0),
  m_beam_intercept_x(0),
  m_beam_intercept_y(0),
  m_beam_slope_x(0),
  m_beam_slope_y(0),
  m_wtcNOverflow(0),
  m_wtcSignal(0),
  m_digitContainerName("FREE"),
  m_caloCellContainerName("AllCalo"),
  m_clusterContainerName("TBClusters"),
  m_WTCContainerName("TailCatcher"),
  m_TBTreeName("TBH6Tree"),
  m_rootfile_name("tbh6tree.root"),
  m_rootfile(0),
  m_tree(0),
  m_calo_id(0),
  m_calo_dd_man(0),
  m_noiseTool(0),
  m_OFNoiseSupp(0),
  m_adc2mevTool(0),
  m_larCablingSvc(0),
  m_txtFileWithXY("xcryo_ytable.txt")
{ 
  declareProperty("ClusterContainer",m_clusterContainerName);
  declareProperty("CellContainer",m_caloCellContainerName);
  declareProperty("NoiseToolName",m_noiseToolName);
  declareProperty("OFNoiseSuppToolName",m_OFNoiseSuppToolName);
  declareProperty("RootFileName",m_rootfile_name);
  declareProperty("TBTreeName",m_TBTreeName);
  declareProperty("Suffix", m_suffix);
  declareProperty("addNoise", m_addNoise);
  declareProperty("addBeamTrack", m_addBeamTrack);
  declareProperty("addMoments", m_addMoments);
  declareProperty("addGain", m_addGain);
  declareProperty("addTime", m_addTime);
  declareProperty("addWTC", m_addWTC);
  declareProperty("addQuality", m_addQuality);
  declareProperty("useEMTBCluster", m_useEMTBCluster);
  declareProperty("zCalo", m_zCalo);
}

TBTree_CaloClusterH6::~TBTree_CaloClusterH6()
{ }

StatusCode TBTree_CaloClusterH6::initialize()
{
  ATH_MSG_INFO ( "in initialize()" );

  // Cluster vectors
  m_nCellCluster = new std::vector<int>;
  m_eCluster = new std::vector<float>; 
  m_etaCluster = new std::vector<float>; 
  m_phiCluster = new std::vector<float>; 
  // Cluster moments
  if (m_addMoments) {
    m_m1_eta = new std::vector<float>;
    m_m1_phi = new std::vector<float>;
    m_m2_r = new std::vector<float>;
    m_m2_lambda = new std::vector<float>;
    m_delta_phi = new std::vector<float>;
    m_delta_theta = new std::vector<float>;
    m_delta_alpha = new std::vector<float>;
    m_center_x = new std::vector<float>;
    m_center_y = new std::vector<float>;
    m_center_z = new std::vector<float>;
    m_center_lambda = new std::vector<float>;
    m_lateral = new std::vector<float>;
    m_longitudinal = new std::vector<float>;
    m_eng_frac_em = new std::vector<float>;
    m_eng_frac_max = new std::vector<float>;
    m_eng_frac_core = new std::vector<float>;
    m_m1_dens = new std::vector<float>;
    m_m2_dens = new std::vector<float>;
  } 
  // Cell vectors
  m_cell_id = new std::vector<int>;
  m_cell_ind_cluster = new std::vector<int>;
  m_cell_energy = new std::vector<float>;
  if (m_addNoise) m_cell_noise = new std::vector<float>;
  if (m_addGain) m_cell_gain = new std::vector<int>;
  if (m_addTime) m_cell_time = new std::vector<float>;
  if (m_addQuality) m_cell_quality = new std::vector<float>;
  if (m_addWTC) {
    m_wtcNOverflow = new std::vector<short>;
    m_wtcSignal = new std::vector<float>;
  }

  // Open file and create TTree
  m_rootfile = new TFile(m_rootfile_name.data(), "RECREATE");
  if (!m_rootfile->IsOpen()) {
    ATH_MSG_FATAL ( "Cann't open Root file" );
    return StatusCode::FAILURE;
  }
  m_tree = new TTree(m_TBTreeName.c_str(), m_TBTreeName.c_str());

  // Define branches
  //Event type
  m_tree->Branch(("ev_type"+m_suffix).c_str(),&m_evType,
		 ("ev_type"+m_suffix+"/s").c_str());
  // Parameters of the sum of clusters (TB particle)
  m_tree->Branch(("eTotal"+m_suffix).c_str(),&m_eTotal,
		 ("eTotal"+m_suffix+"/F").c_str());
  m_tree->Branch(("etaTotal"+m_suffix).c_str(),&m_etaTotal,
		 ("etaTotal"+m_suffix+"/F").c_str());
  m_tree->Branch(("phiTotal"+m_suffix).c_str(),&m_phiTotal,
		 ("phiTotal"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_emec2"+m_suffix).c_str(),&m_eEME2,
		 ("e_emec2"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_emec3"+m_suffix).c_str(),&m_eEME3,
		 ("e_emec3"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_hec0"+m_suffix).c_str(),&m_eHEC0,
		 ("e_hec0"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_hec1"+m_suffix).c_str(),&m_eHEC1,
		 ("e_hec1"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_hec2"+m_suffix).c_str(),&m_eHEC2,
		 ("e_hec2"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_fcal0"+m_suffix).c_str(),&m_eFCAL0,
		 ("e_fcal0"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_fcal1"+m_suffix).c_str(),&m_eFCAL1,
		 ("e_fcal1"+m_suffix+"/F").c_str());
  m_tree->Branch(("e_fcal2"+m_suffix).c_str(),&m_eFCAL2,
		 ("e_fcal2"+m_suffix+"/F").c_str());
  // Claster parameters
  m_tree->Branch(("cl_num"+m_suffix).c_str(),&m_nClusters,
		 ("cl_num"+m_suffix+"/I").c_str());
  m_tree->Branch(("cl_cell_num"+m_suffix).c_str(),&m_nCellCluster);
  m_tree->Branch(("cl_ener"+m_suffix).c_str(),&m_eCluster);
  m_tree->Branch(("cl_eta"+m_suffix).c_str(),&m_etaCluster);
  m_tree->Branch(("cl_phi"+m_suffix).c_str(),&m_phiCluster);
  if (m_addMoments) {
    m_tree->Branch(("cl_m1_eta"+m_suffix).c_str(),& m_m1_eta);
    m_tree->Branch(("cl_m1_phi"+m_suffix).c_str(), &m_m1_phi);
    m_tree->Branch(("cl_m2_r"+m_suffix).c_str(),&m_m2_r);
    m_tree->Branch(("cl_m2_lambda"+m_suffix).c_str(), &m_m2_lambda);
    m_tree->Branch(("cl_delta_phi"+m_suffix).c_str(), &m_delta_phi);
    m_tree->Branch(("cl_delta_theta"+m_suffix).c_str(), &m_delta_theta);
    m_tree->Branch(("cl_delta_alpha"+m_suffix).c_str(), &m_delta_alpha);
    m_tree->Branch(("cl_center_x"+m_suffix).c_str(), &m_center_x);
    m_tree->Branch(("cl_center_y"+m_suffix).c_str(), &m_center_y);
    m_tree->Branch(("cl_center_z"+m_suffix).c_str(), &m_center_z);
    m_tree->Branch(("cl_center_lambda"+m_suffix).c_str(), &m_center_lambda);
    m_tree->Branch(("cl_lateral"+m_suffix).c_str(), &m_lateral);
    m_tree->Branch(("cl_longitudinal"+m_suffix).c_str(), &m_longitudinal);
    m_tree->Branch(("cl_eng_frac_em"+m_suffix).c_str(), &m_eng_frac_em);
    m_tree->Branch(("cl_eng_frac_max"+m_suffix).c_str(), &m_eng_frac_max);
    m_tree->Branch(("cl_eng_frac_core"+m_suffix).c_str(), &m_eng_frac_core);
    m_tree->Branch(("cl_m1_dens"+m_suffix).c_str(), &m_m1_dens);
    m_tree->Branch(("cl_m2_dens"+m_suffix).c_str(), &m_m2_dens);
  }
  // Cell parameters
  m_tree->Branch(("cl_ncells"+m_suffix).c_str(),&m_nCells,
		 ("cl_ncells"+m_suffix+"/I").c_str());
  m_tree->Branch(("cl_cell_id"+m_suffix).c_str(), &m_cell_id);
  m_tree->Branch(("cl_cell_clu_ind"+m_suffix).c_str(), &m_cell_ind_cluster);
  m_tree->Branch(("cl_cell_ener"+m_suffix).c_str(), &m_cell_energy);
  if (m_addNoise)
    m_tree->Branch(("cl_cell_noise"+m_suffix).c_str(), &m_cell_noise);
  if (m_addGain) 
    m_tree->Branch(("cl_cell_gain"+m_suffix).c_str(), &m_cell_gain);
  if (m_addTime) 
    m_tree->Branch(("cl_cell_time"+m_suffix).c_str(), &m_cell_time);
  if (m_addQuality)
    m_tree->Branch(("cl_cell_quality"+m_suffix).c_str(), &m_cell_quality);
  
  // Add beam track parameters
  if (m_addBeamTrack) {
    m_tree->Branch(("bm_x"+m_suffix).c_str(), &m_beam_coor_x, 
		   ("bm_x"+m_suffix).c_str());
    m_tree->Branch(("bm_y"+m_suffix).c_str(), &m_beam_coor_y, 
		   ("bm_y"+m_suffix).c_str());
    m_tree->Branch(("bm_chi2_x"+m_suffix).c_str(), &m_beam_chi2_x, 
		   ("bm_chi2_x"+m_suffix).c_str());
    m_tree->Branch(("bm_chi2_y"+m_suffix).c_str(), &m_beam_chi2_y, 
		   ("bm_chi2_y"+m_suffix).c_str());
    m_tree->Branch(("bm_x0"+m_suffix).c_str(), &m_beam_intercept_x, 
		   ("bm_x0"+m_suffix).c_str());
    m_tree->Branch(("bm_y0"+m_suffix).c_str(), &m_beam_intercept_y, 
		   ("bm_y0"+m_suffix).c_str());
    m_tree->Branch(("bm_slope_x"+m_suffix).c_str(),&m_beam_slope_x,
		   ("bm_slope_x"+m_suffix).c_str());
    m_tree->Branch(("bm_slope_y"+m_suffix).c_str(),&m_beam_slope_y,
		   ("bm_slope_y"+m_suffix).c_str());
 }
 
  // Add Warm TailCatcher data
  if (m_addWTC) {
    m_tree->Branch(("wtc_n_overflow"+m_suffix).c_str(), &m_wtcNOverflow);
    m_tree->Branch(("wtc_signal"+m_suffix).c_str(), &m_wtcSignal);
  }

  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();

  // allocate ToolSvc
  IToolSvc* toolSvc;
  ATH_CHECK( service("ToolSvc", toolSvc) );

  if (m_addNoise) {
    IAlgTool* algtool;
    ATH_CHECK( toolSvc->retrieveTool("LArADC2MeVTool", algtool) );
    m_adc2mevTool=dynamic_cast<ILArADC2MeVTool*>(algtool);
    if (!m_adc2mevTool) {
      ATH_MSG_ERROR ( "Unable to d-cast LArADC2MeV" );
      return StatusCode::FAILURE;
    }
  }

  ListItem ntool(m_noiseToolName);	  
  ATH_CHECK( toolSvc->retrieveTool(ntool.type(), ntool.name(), m_noiseTool) );
  ATH_MSG_INFO ( "Noise Tool " << m_noiseToolName << " is selected!" );
  
  if (m_addNoise) {
    ListItem ntool(m_OFNoiseSuppToolName);
    ATH_CHECK( toolSvc->retrieveTool(ntool.type(), ntool.name(), m_OFNoiseSupp) );
    ATH_MSG_INFO ("Noise Tool " << m_OFNoiseSuppToolName << " is selected!" );
    // Translate offline ID into online ID
    ATH_CHECK( toolSvc->retrieveTool("LArCablingService",m_larCablingSvc) );
  }
  
  ATH_MSG_INFO ( "end of initialize()" );
  return StatusCode::SUCCESS;
}


StatusCode TBTree_CaloClusterH6::finalize()
{
  m_tree->AutoSave();
  ATH_MSG_INFO ( "finalize(): (invoked/accept/reject/random) ("<< m_nEvent
      << "/" << m_nEventAccepted << "/" << m_nEventRejected 
                 << "/" << m_nEventRandomTrigger << ")" );
  ATH_MSG_INFO ( "Print contents of " << m_TBTreeName );
  m_rootfile->Print();
  m_rootfile->Close();
  return StatusCode::SUCCESS;
}

void TBTree_CaloClusterH6::clear()
{
  m_eTotal = 0.;
  m_etaTotal = 0.;
  m_phiTotal = 0.;
  m_eEME2 = 0.;
  m_eEME3 = 0.;
  m_eHEC0 = 0.;
  m_eHEC1 = 0.;
  m_eHEC2 = 0.;
  m_eFCAL0 = 0.;
  m_eFCAL1 = 0.;
  m_eFCAL2 = 0.;
  m_nClusters = 0;
  m_nCellCluster->clear();
  m_eCluster->clear(); 
  m_etaCluster->clear(); 
  m_phiCluster->clear(); 
  if (m_addMoments) {
    m_m1_eta->clear();
    m_m1_phi->clear();
    m_m2_r->clear();
    m_m2_lambda->clear();
    m_delta_phi->clear();
    m_delta_theta->clear();
    m_delta_alpha->clear();
    m_center_x->clear();
    m_center_y->clear();
    m_center_z->clear();
    m_center_lambda->clear();
    m_lateral->clear();
    m_longitudinal->clear();
    m_eng_frac_em->clear();
    m_eng_frac_max->clear();
    m_eng_frac_core->clear();
    m_m1_dens->clear();
    m_m2_dens->clear();
  }
  m_nCells=0;
  m_cell_id->clear(); 
  m_cell_ind_cluster->clear();
  m_cell_energy->clear(); 
  if (m_addNoise) m_cell_noise->clear();
  if (m_addGain) m_cell_gain->clear(); 
  if (m_addTime) m_cell_time->clear(); 
  if (m_addQuality) m_cell_quality->clear(); 
  if (m_addWTC) {
    m_wtcNOverflow->clear(); 
    m_wtcSignal->clear(); 
  }
}

StatusCode TBTree_CaloClusterH6::execute()
{
  const int lastScintInLayer[] = {11, 23, 29, 35, 41, 47};
  m_nEvent++;
  ATH_MSG_DEBUG ( "Executing TBTree_CaloClusterH6 for "
                  << m_clusterContainerName );

  // Retrieve Event Info
  const TBEventInfo* theEventInfo;
  StatusCode sc = evtStore()->retrieve(theEventInfo,"TBEventInfo");
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR
      ( "Cannot retrieve TBEventInfo from StoreGate" );
    m_nEventRejected++;
    return StatusCode::FAILURE;
  }
  m_evType = theEventInfo->getEventType();
  ATH_MSG_DEBUG ( "Event Type found " << m_evType );
  if (m_evType != 1 && m_evType != 3 && m_evType != 0) {
    m_nEventRejected++;
    return StatusCode::FAILURE;
  }
  if (m_evType == 3) {                               // Random trigger
    if (m_cell_id->size() == 0) {
      ATH_MSG_WARNING 
        ( "No filled cell_id vector for random trigger event" );
      m_nEventRejected++;
      return StatusCode::FAILURE;
    }
    m_beam_coor_x = 9999.;
    m_beam_coor_y = 9999.;
    m_beam_chi2_x = 9999.;
    m_beam_chi2_y = 9999.;
    m_beam_intercept_x = 9999.;
    m_beam_intercept_y = 9999.;
    m_beam_slope_x = 0.;
    m_beam_slope_y = 0.;
    m_eTotal = 0.;
    m_eEME2 = 0.;
    m_eEME3 = 0.;
    m_eHEC0 = 0.;
    m_eHEC1 = 0.;
    m_eHEC2 = 0.;
    m_eFCAL0 = 0.;
    m_eFCAL1 = 0.;
    m_eFCAL2 = 0.;
    for (int i=0; i<(int)m_eCluster->size(); i++) (*m_eCluster)[i] = 0;
    for (int i=0; i<(int)m_cell_energy->size(); i++) (*m_cell_energy)[i] = 0;
    if (m_addGain) m_cell_gain->clear();
    if (m_addTime) m_cell_time->clear();
    if (m_addQuality) m_cell_quality->clear();
    if (m_addNoise) m_cell_noise->clear();
    // Get cell information
    const CaloCellContainer* cellContainer;
    sc = evtStore()->retrieve(cellContainer, m_caloCellContainerName);
    if (sc.isFailure()) {
      m_nEventRejected++;
      ATH_MSG_ERROR
        ( "cannot allocate CaloCellContainer with key <"
	  << m_caloCellContainerName
	  << "> for random trigger event");
      return sc;
    }
    // Cell loop
    CaloCellContainer::const_iterator itc = cellContainer->begin();
    for (;itc!=cellContainer->end(); itc++) {
      const CaloCell* cell = (*itc);
      const Identifier id = cell->ID();
      for (int icell=0; icell<(int)m_cell_id->size(); icell++) {
          if ((int)id.get_identifier32().get_compact() == (*m_cell_id)[icell]) {
	  m_eTotal += (float)cell->energy();
	  (*m_cell_energy)[icell] = (float)cell->energy();
	  int ind = (*m_cell_ind_cluster)[icell];
	  (*m_eCluster)[ind] += (float)cell->energy();
	  break;
	}
      }
    }
    // Fill the tree
    m_tree->Fill();
    m_nEventRandomTrigger++;
    return StatusCode::SUCCESS;
  }

  // Do first event initialization (run header filling)
  if (m_first) {
    m_first = false;
    // Fill run header
    m_nRun = theEventInfo->getRunNum();
    m_beamMom = theEventInfo->getBeamMomentum();
    // Get xcryo and ytable from a file
    if (!this->getXcryoYtable(m_xCryo, m_yTable, m_beamMom)) {
      ATH_MSG_ERROR ( "xCryo and yTable are not found for run " <<
                      m_nRun << " in file " << m_txtFileWithXY );
      if (m_addBeamTrack) return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "nRun = " << m_nRun << ", beamMomentum = "
	<< m_beamMom << " GeV, CryoX = " << m_xCryo << ", tableY = "
                   << m_yTable );
    
    // Fill the run header and attach it to the tree
    TBH6RunHeader* rh = new TBH6RunHeader(m_nRun);
    rh->SetBeamMomentum(m_beamMom);
    rh->SetXCryo(m_xCryo);
    rh->SetYTable(m_yTable);
    rh->SetZCalo(m_zCalo);
    
    // Get cell information
    const CaloCellContainer* cellContainer = 0;
    ATH_CHECK( evtStore()->retrieve(cellContainer, m_caloCellContainerName) );
    m_nCells = cellContainer->size();
    ATH_MSG_DEBUG ( "CaloCellContainer container size = " <<  m_nCells );
    
    if (m_nCells > CELLMAX) {
      ATH_MSG_ERROR ( "CaloCellContainer container size = " <<
                      m_nCells << " > CELLMAX = " << CELLMAX );
      return StatusCode::FAILURE;
    }
    
    // Cell loop
    int icell=0;
    CaloCellContainer::const_iterator itc = cellContainer->begin();
    for (;itc!=cellContainer->end(); itc++) {
      const CaloCell* cell = (*itc);
      const CaloDetDescrElement* caloDDE = cell->caloDDE();
      const Identifier id = cell->ID();
      rh->SetCellID(icell,id.get_identifier32().get_compact());
      rh->SetCellEta(icell,cell->eta());
      rh->SetCellPhi(icell,cell->phi());
      rh->SetCellX(icell,caloDDE->x());
      rh->SetCellY(icell,caloDDE->y());
      rh->SetCellZ(icell,caloDDE->z());
      rh->SetCellR(icell,caloDDE->r());
      rh->SetCelldEta(icell,caloDDE->deta());
      rh->SetCelldPhi(icell,caloDDE->dphi());
      rh->SetCelldZ(icell,caloDDE->dz());
      rh->SetCelldR(icell,caloDDE->dr());
      rh->SetCellVolume(icell,caloDDE->volume());
      rh->SetCelliEta(icell,m_calo_id->eta(id));
      rh->SetCelliPhi(icell,m_calo_id->phi(id));
      if (m_calo_id->sub_calo(id)== CaloCell_ID::LARFCAL)
	rh->SetCellRegion(icell,0);
      else rh->SetCellRegion(icell,m_calo_id->region(id));
      rh->SetCellCalo(icell,m_calo_id->sub_calo(id));
      rh->SetCellSampling(icell,m_calo_id->calo_sample(id));
      rh->SetCellNoiseRMS(icell,
	 m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE));
      icell++;
    }
    rh->SetCellNum(icell+1);
    
    // Attach rh to the tree
    m_tree->GetUserInfo()->Add(rh);
  }       // if (m_first)
  
  this->clear();

  // Get beam coordinates
  if (m_addBeamTrack && (m_evType == 1 || m_evType == 0)) {
    TBTrack *track;
    sc = evtStore()->retrieve(track, "Track");
    if (sc.isFailure()){
      ATH_MSG_ERROR ( "Retrieval of beam track failed" );
      m_nEventRejected++;
      return StatusCode::FAILURE;
    }
    m_beam_coor_x = track->getUslope()*m_zCalo + track->getUintercept()
      + m_xCryo;
    m_beam_coor_y = track->getVslope()*m_zCalo + track->getVintercept();
    m_beam_chi2_x = track->getChi2_u();
    m_beam_chi2_y = track->getChi2_v();
    m_beam_intercept_x = track->getUintercept();
    m_beam_intercept_y = track->getVintercept();
    m_beam_slope_x = track->getUslope();
    m_beam_slope_y = track->getVslope();
  }
  // Warm TailCatcher data
  if (m_addWTC) {
    TBTailCatcher * wtc;
    sc = evtStore()->retrieve(wtc, m_WTCContainerName);
    if (sc.isFailure()){
      ATH_MSG_ERROR ( "Retrieval of WTC data failed" );
      m_nEventRejected++;
      return StatusCode::FAILURE;
    }
    DataVector< TBScintillator >::const_iterator its = wtc->begin();
    ATH_MSG_VERBOSE("scint name/signal/overflow flag:");
    short novflow = 0;
    float signal = 0;
    int nScint = 0; int nLayer = 0;
    for (; its != wtc->end(); its++) {
      ATH_MSG_VERBOSE((*its)->getDetectorName()<<"/"<<(*its)->getSignal()<<
                      "/"<<(*its)->isSignalOverflow()<<" ");
      if ((*its)->isSignalOverflow()) novflow++;
      signal += (*its)->getSignal();
      if (nScint == lastScintInLayer[nLayer]) {
	m_wtcNOverflow->push_back(novflow);
	m_wtcSignal->push_back(signal);
	novflow = 0; signal = 0;
	nLayer++;
      }
      nScint++;
    }
    if (msgLvl(MSG::VERBOSE)) {
        msg()<<MSG::VERBOSE<<"nOverflow: ";
        for (int i=0; i<nLayer; i++) {
          msg()<<MSG::VERBOSE<<(*m_wtcNOverflow)[i]<<" ";
        }
        msg()<<MSG::VERBOSE<<endmsg;
        msg()<<MSG::VERBOSE<<"Signals: ";
        for (int i=0; i<nLayer; i++) {
          msg()<<MSG::VERBOSE<<(*m_wtcSignal)[i]<<" ";
        }
        msg()<<MSG::VERBOSE<<endmsg;
      }
  }

  // Reject hadrons in 10 GeV electron beam
  if (m_useEMTBCluster && fabs(m_beamMom-10.) < 0.5) {
    const CaloClusterContainer* clusterContainer;
    sc = evtStore()->retrieve(clusterContainer, "EMTBClusters");
    if (sc.isFailure()) {
      ATH_MSG_ERROR
        ( "cannot allocate CaloClusterContainer with key <"
	  << "EMTBClusters"  << ">" );
      m_nEventRejected++;
      return sc;
    }
    if ( clusterContainer->size()== 1) {
      CaloClusterContainer::const_iterator it_clu = clusterContainer->begin();
      const CaloCluster* cluster = (*it_clu);
      float e = (float)cluster->energy();
      float ecut = 6000;
      if ( m_yTable < -100) ecut = 4000;
      if (e < ecut) {
	m_nEventRejected++;
	return StatusCode::FAILURE;
      }    
    }
  }
  // Reject muons in 40 GeV pion beam
  if (m_useEMTBCluster && fabs(m_beamMom-40.) < 0.5) {
    const CaloClusterContainer* clusterContainer;
    sc = evtStore()->retrieve(clusterContainer, "EMTBClusters");
    if (sc.isFailure()) {
      ATH_MSG_ERROR
        ( "cannot allocate CaloClusterContainer with key <"
	  << "EMTBClusters"  << ">" );
      m_nEventRejected++;
      return sc;
    }
    if ( clusterContainer->size()== 1) {
      CaloClusterContainer::const_iterator it_clu = clusterContainer->begin();
      const CaloCluster* cluster = (*it_clu);
      float e = (float)cluster->energy();
      float ecut = 9000;
      //if ( m_yTable < -100) ecut = 4000;
      if (e < ecut) {
	m_nEventRejected++;
	return StatusCode::FAILURE;
      }    
    }
  }
  // Clusters
  const CaloClusterContainer* clusterContainer;
  sc = evtStore()->retrieve(clusterContainer, m_clusterContainerName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR
      ( "cannot allocate CaloClusterContainer with key <"
	<< m_clusterContainerName
	<< ">");
    m_nEventRejected++;
    return sc;
  }
  m_nClusters = clusterContainer->size();
  ATH_MSG_DEBUG ( "CaloClusterContainer container size = " <<  
                  m_nClusters );
  if ( clusterContainer->size() < 1) {
    ATH_MSG_ERROR ( " wrong CaloClusterContainer container size = " <<
                    m_nClusters );
    m_nEventRejected++;
    return StatusCode::FAILURE;
  }

  int clu_ind = 0;
  float eAbsTotal = 0.;
  CaloClusterContainer::const_iterator it_clu = clusterContainer->begin();
  for (; it_clu!=clusterContainer->end(); it_clu++) {
    const CaloCluster* cluster = (*it_clu);
    m_nCells += cluster->getNumberOfCells();
    m_nCellCluster->push_back((int)cluster->getNumberOfCells());
    m_eCluster->push_back((float)cluster->energy());
    m_etaCluster->push_back((float)cluster->eta());
    m_phiCluster->push_back((float)cluster->phi());
    m_eTotal += (float)cluster->energy();
    eAbsTotal += fabs((float)cluster->energy());
    m_etaTotal += fabs((float)cluster->energy())*(float)cluster->eta();
    m_phiTotal += fabs((float)cluster->energy())*(float)cluster->phi();
    //m_cell_energy->reserve(m_nCells);
    //if (m_addNoise) m_cell_noise->reserve(m_nCells);
    //if (m_addGain) m_cell_gain->reserve(m_nCells);
    //if (m_addTime) m_cell_time->reserve(m_nCells);
    //if (m_addQuality) m_cell_quality->reserve(m_nCells);
    
    // Loop through cluster cells
    CaloCluster::cell_iterator itc=cluster->cell_begin();
    for (; itc!=cluster->cell_end(); itc++) {
      const CaloCell* cell = (*itc);
      const Identifier id = cell->ID();
      m_cell_id->push_back(id.get_identifier32().get_compact());
      m_cell_ind_cluster->push_back(clu_ind);
      m_cell_energy->push_back((float)cell->energy());
      if (m_addGain) m_cell_gain->push_back((int)cell->gain());
      if (m_addTime) m_cell_time->push_back((float)cell->time());
      if (m_addQuality) m_cell_quality->push_back((float)cell->quality());
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::EME2)
	m_eEME2 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::EME3)
	m_eEME3 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::HEC0)
	m_eHEC0 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::HEC1)
	m_eHEC1 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::HEC2)
	m_eHEC2 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::FCAL0)
	m_eFCAL0 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::FCAL1)
	m_eFCAL1 += (float)cell->energy();
      if (m_calo_id->calo_sample(id) == (int)CaloSampling::FCAL2)
	m_eFCAL2 += (float)cell->energy();
    }
    // Add noise vector
    if (m_addNoise) {
      ATH_CHECK( this->getNoise(cluster) );
    }
    
    // Add cluster moments
    if (m_addMoments) {
      float m1_eta = -9999;
      float m1_phi = -9999;
      float m2_r = -9999;
      float m2_lambda = -9999;
      float delta_phi = -9999;
      float delta_theta = -9999;
      float delta_alpha = -9999;
      float center_x = -9999;
      float center_y = -9999;
      float center_z = -9999;
      float center_lambda = -9999;
      float lateral = -9999;
      float longitudinal = -9999;
      float eng_frac_em = -9999;
      float eng_frac_max = -9999;
      float eng_frac_core = -9999;
      float m1_dens = -9999;
      float m2_dens = -9999;
      CaloCluster::moment_iterator it_mom=cluster->beginMoment(false);
      for (; it_mom != cluster->endMoment(false); it_mom++) {
	switch ( it_mom.getMomentType() ) {
	case CaloClusterMoment::FIRST_ETA:
	  m1_eta = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::FIRST_PHI:
	  m1_phi = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::SECOND_R:
	  m2_r = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::SECOND_LAMBDA:
	  m2_lambda = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::DELTA_PHI:
	  delta_phi = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::DELTA_THETA:
	  delta_theta = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::DELTA_ALPHA:
	  delta_alpha = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::CENTER_X:
	  center_x = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::CENTER_Y:
	  center_y = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::CENTER_Z:
	  center_z = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::CENTER_LAMBDA:
	  center_lambda = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::LATERAL:
	  lateral = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::LONGITUDINAL:
	  longitudinal = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::ENG_FRAC_EM:
	  eng_frac_em = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::ENG_FRAC_MAX:
	  eng_frac_max = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::ENG_FRAC_CORE:
	  eng_frac_core = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::FIRST_ENG_DENS:
	  m1_dens = it_mom.getMoment().getValue();
	  break;
	case CaloClusterMoment::SECOND_ENG_DENS:
	  m2_dens = it_mom.getMoment().getValue();
	  break;
	default:
	  break;
	}
      }
      m_m1_eta->push_back(m1_eta);
      m_m1_phi->push_back(m1_phi);
      m_m2_r->push_back(m2_r);
      m_m2_lambda->push_back(m2_lambda);
      m_delta_phi->push_back(delta_phi);
      m_delta_theta->push_back(delta_theta);
      m_delta_alpha->push_back(delta_alpha);
      m_center_x->push_back(center_x);
      m_center_y->push_back(center_y);
      m_center_z->push_back(center_z);
      m_center_lambda->push_back(center_lambda);
      m_lateral->push_back(lateral);
      m_longitudinal->push_back(longitudinal);
      m_eng_frac_em->push_back(eng_frac_em);
      m_eng_frac_max->push_back(eng_frac_max);
      m_eng_frac_core->push_back(eng_frac_core);
      m_m1_dens->push_back(m1_dens);
      m_m2_dens->push_back(m2_dens);
    }
    clu_ind++;
  } // end of cluster loop 

  if (eAbsTotal > 0.) {
    const float inv_eAbsTotal = 1. / eAbsTotal;
    m_etaTotal *= inv_eAbsTotal;
    m_phiTotal *= inv_eAbsTotal;
  }

  // Fill the tree
  m_tree->Fill();
  m_nEventAccepted++;

  return sc;
}

StatusCode TBTree_CaloClusterH6::getXcryoYtable(float &x, float &y, float &e) {

  ATH_MSG_DEBUG ( "in getXcryoYtable(float x, float y)" );

  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(m_txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    ATH_MSG_ERROR ( "File " << m_txtFileWithXY << " fail to open in $DATAPATH");
    return StatusCode::FAILURE;
  }

  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    ATH_MSG_DEBUG ( "run,x,y,e= "<<run<<" "<<x<<" "<<y<<" "<<e);
    if (run == m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

StatusCode TBTree_CaloClusterH6::getNoise(CaloCluster const * const cluster) {
  // Constants for EMEC correction
  float EMEC_eta1=2.5; float EMEC_eta2=2.8; float EMEC_eta3=3.2;
  float EMEC_eta0_1 = 0.5*(EMEC_eta1 + EMEC_eta2);
  float EMEC_eta0_2 = 0.5*(EMEC_eta2 + EMEC_eta3);
  float EMEC_beta1=1.04; float EMEC_alpha1=0.55;
  float EMEC_beta2=1.00; float EMEC_alpha2=0.55;
  const float ramp_corr(1.045);
  const float ramp_corr_eta1=2.8; 
  const float ramp_corr_eta2=2.9;
  const float EMEC_rescale=1.215;

  const float inv_ramp_corr = 1. / ramp_corr;
  const float inv_EMEC_rescale = 1. / EMEC_rescale;

  const LArDigitContainer* digitContainer = 0;
  const ILArPedestal* larPedestal = 0;

  StatusCode sc=evtStore()->retrieve(digitContainer,m_digitContainerName);
  if(sc.isFailure()) {
    ATH_MSG_ERROR ( "Can't retrieve LArDigitContainer with key " <<
                    m_digitContainerName << "from StoreGate." );
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( detStore()->retrieve(larPedestal) );

  // Loop over cluster cells
  CaloCluster::cell_iterator itc=cluster->cell_begin();
  for (; itc!=cluster->cell_end(); itc++) {
    float noise = 0;
    const CaloCell* cell = (*itc);
    const Identifier id = cell->ID();
    float eta = cell->eta();
    const CaloGain::CaloGain gain= cell->gain();
    HWIdentifier chid;
    try {
      chid = m_larCablingSvc->createSignalChannelID(id);
    }
    catch ( const LArID_Exception& except) {
      ATH_MSG_ERROR("HWId not found: "<<(const std::string&)except);
      return StatusCode::FAILURE;
    }
    // Find the cell data in the DigitContainer
    LArDigitContainer::const_iterator cont_it=digitContainer->begin();
    for (;cont_it!=digitContainer->end(); cont_it++) {
      if (chid !=(*cont_it)->channelID()) continue;
      const std::vector<short>& samples=(*cont_it)->samples();
      noise = samples[0];
      break;
    }
    if (cont_it == digitContainer->end()) {
      ATH_MSG_ERROR("Samples are not found for cell with HWId = "<<
                    chid.get_identifier32().get_compact());
      return StatusCode::FAILURE;
    }
    // Get pedestal
    float pedestal=larPedestal->pedestal(chid,gain);
    if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) {
      float val = m_noiseTool->
      getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
      noise = m_rndm.Gaus(0,val);
      ATH_MSG_WARNING("No pedestal found for HWId = "<< chid.get_identifier32().get_compact() <<
                      " gain = "<<(int)gain<<". Generated(sigma="<<val<<
                      ") noise taken: "<<noise);
      return StatusCode::SUCCESS;
    }
    noise -= pedestal;
    // Apply OF noise suppression factor
    noise *= m_OFNoiseSupp->
      getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
    // Apply ADCtoMeV factor
    const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(chid,gain);
    if (ramp.size()==0) {
      ATH_MSG_ERROR("No ADC2MeV data found for channel HWId = "
                    << chid.get_identifier32().get_compact() <<", gain = "<<(int)gain);
      return StatusCode::FAILURE;
    }
    noise *= ramp[1];
    // EMEC corrections =======================================================
    if (cell->caloDDE()->is_lar_em_endcap()) {
      // Cell level correction of EMEC eta-dependent response
      if (eta > EMEC_eta1 && eta < EMEC_eta2) {
	noise *= EMEC_beta1/(1 + EMEC_alpha1*(eta - EMEC_eta0_1));
      }
      else if (eta > EMEC_eta2 && eta < EMEC_eta3) {
	noise *= EMEC_beta2/(1 + EMEC_alpha2*(eta - EMEC_eta0_2));
      }
      if (eta > ramp_corr_eta1 && eta < ramp_corr_eta2) noise *= inv_ramp_corr;
      noise *= inv_EMEC_rescale;
    }
    // End of EMEC corrections ================================================
    m_cell_noise->push_back(noise);
  }       // for (itc)
  return StatusCode::SUCCESS;
}
