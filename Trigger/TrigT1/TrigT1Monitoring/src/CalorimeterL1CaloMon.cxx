/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CalorimeterL1CaloMon.cxx
// PACKAGE:  TrigT1Monitoring  
//
// AUTHOR:   Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           Sky French (sfrench@hep.phy.cam.ac.uk)  
// ********************************************************************

#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "Identifier/Identifier.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloMonitoringCaloTool.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"             
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "CalorimeterL1CaloMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
CalorimeterL1CaloMon::CalorimeterL1CaloMon(const std::string & type,
                                           const std::string & name,
					   const IInterface* parent)
  : ManagedMonitorToolBase ( type, name, parent ),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_caloTool("LVL1::L1CaloMonitoringCaloTool/L1CaloMonitoringCaloTool"),
    m_histBooked(false),
    m_h_CaloCell_tile_phi(0),
    m_h_CaloCell_tile_eta(0),
    m_h_CaloCell_tile_HitMap(0),
    m_h_CaloCell_tile_et(0),
    m_h_CaloCell_lar_phi(0),
    m_h_CaloCell_lar_eta(0),
    m_h_CaloCell_lar_HitMap(0),
    m_h_CaloCell_lar_et(0),
    m_h_CaloTT_HitMap_emLUT_Thresh0(0),
    m_h_CaloTT_HitMap_emLUT_Thresh1(0),
    m_h_CaloTT_HitMap_emLUT_Thresh2(0),
    m_h_CaloTT_HitMap_hadLUT_Thresh0(0),
    m_h_CaloTT_HitMap_hadLUT_Thresh1(0),
    m_h_CaloTT_HitMap_hadLUT_Thresh2(0),
    m_h_CaloTT_emLUT(0),
    m_h_CaloTT_emLUT_eta(0),
    m_h_CaloTT_emLUT_phi(0),
    m_h_CaloTT_hadLUT(0),
    m_h_CaloTT_hadLUT_eta(0),
    m_h_CaloTT_hadLUT_phi(0),
    m_h_emEnergy_Match_CaloTT_TT_LArEMB(0),
    m_h_emEnergy_Match_CaloTT_TT_LArOverlap(0),
    m_h_emEnergy_Match_CaloTT_TT_LArEMEC(0),
    m_h_emEnergy_Match_CaloTT_TT_LArFCAL1(0),
    m_h_hadEnergy_Match_CaloTT_TT_TileLB(0),
    m_h_hadEnergy_Match_CaloTT_TT_TileEB(0),
    m_h_hadEnergy_Match_CaloTT_TT_LArHEC(0),
    m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23(0),
    m_h_emEnergy_Match_CaloTT_TT_LArEMB_CP(0),     
    m_h_emEnergy_Match_CaloTT_TT_LArOverlap_CP(0),
    m_h_emEnergy_Match_CaloTT_TT_LArEMEC_CP(0),   
    m_h_emEnergy_Match_CaloTT_TT_LArFCAL1_CP(0),  
    m_h_hadEnergy_Match_CaloTT_TT_TileLB_CP(0),   
    m_h_hadEnergy_Match_CaloTT_TT_TileEB_CP(0),   
    m_h_hadEnergy_Match_CaloTT_TT_LArHEC_CP(0),   
    m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23_CP(0),
    m_h_em_profile_Match(0),
    m_h_had_profile_Match(0),
    m_h_em_profile_Match_CP(0),
    m_h_had_profile_Match_CP(0),
    /*m_h_em_Mismatch_etaphi_alt(0),
    m_h_em_Mismatch_etaphi(0),
    m_h_had_Mismatch_etaphi_alt(0),
    m_h_had_Mismatch_etaphi(0),
    m_h_em_Mismatch_etaphi_alt_CP(0),
    m_h_em_Mismatch_etaphi_CP(0),
    m_h_had_Mismatch_etaphi_alt_CP(0),
    m_h_had_Mismatch_etaphi_CP(0),*/
    m_h_em_1D_Match_LArEMB(0),
    m_h_em_1D_Match_LArOverlap(0),
    m_h_em_1D_Match_LArEMEC(0),
    m_h_em_1D_Match_LArFCAL1(0),
    m_h_had_1D_Match_TileLB(0),
    m_h_had_1D_Match_TileEB(0),
    m_h_had_1D_Match_LArHEC(0),
    m_h_had_1D_Match_LArFCAL23(0),
    m_h_em_1D_Match_LArEMB_CP(0),
    m_h_em_1D_Match_LArOverlap_CP(0),
    m_h_em_1D_Match_LArEMEC_CP(0),
    m_h_em_1D_Match_LArFCAL1_CP(0),
    m_h_had_1D_Match_TileLB_CP(0),
    m_h_had_1D_Match_TileEB_CP(0),
    m_h_had_1D_Match_LArHEC_CP(0),
    m_h_had_1D_Match_LArFCAL23_CP(0),
    m_h_emEnergy_Calo_L1CaloSat_LArEMB(0),
    m_h_emEnergy_Calo_L1CaloSat_LArOverlap(0),
    m_h_emEnergy_Calo_L1CaloSat_LArEMEC(0), 
    m_h_emEnergy_Calo_L1CaloSat_LArFCAL1(0),
    m_h_hadEnergy_Calo_L1CaloSat_TileLB(0),
    m_h_hadEnergy_Calo_L1CaloSat_TileEB(0),
    m_h_hadEnergy_Calo_L1CaloSat_LArHEC(0),
    m_h_hadEnergy_Calo_L1CaloSat_LArFCAL23(0),
    m_h_emEnergy_Calo_L1CaloSat_LArEMB_CP(0),
    m_h_emEnergy_Calo_L1CaloSat_LArOverlap_CP(0),
    m_h_emEnergy_Calo_L1CaloSat_LArEMEC_CP(0),
    m_h_emEnergy_Calo_L1CaloSat_LArFCAL1_CP(0),
    m_h_hadEnergy_Calo_L1CaloSat_TileLB_CP(0),
    m_h_hadEnergy_Calo_L1CaloSat_TileEB_CP(0),
    m_h_hadEnergy_Calo_L1CaloSat_LArHEC_CP(0),
    m_h_hadEnergy_Calo_L1CaloSat_LArFCAL23_CP(0),
    m_h_em_profile_etaRegion(0),
    m_h_had_profile_etaRegion(0),
    m_h_em_profile_phiRegion(0),
    m_h_had_profile_phiRegion(0),
    m_h_em_profile_etaRegion_CP(0),
    m_h_had_profile_etaRegion_CP(0),
    m_h_em_profile_phiRegion_CP(0),
    m_h_had_profile_phiRegion_CP(0),
    m_h_average_emDE_map(0),
    m_h_average_hadDE_map(0),
    m_h_average_emDE_map_CP(0),
    m_h_average_hadDE_map_CP(0),
    m_h_emTTME_etaphi(0),
    m_h_hadTTME_etaphi(0),
    m_h_emTTME_etaphi_CP(0),
    m_h_hadTTME_etaphi_CP(0),
    m_h_emcaloME_etaphi(0),
    m_h_hadcaloME_etaphi(0)
/*---------------------------------------------------*/
{
  declareProperty("CaloCellContainer", m_caloCellContainerName = "AllCalo");
  declareProperty("xAODTriggerTowerContainer",
                    m_xAODTriggerTowerContainerName = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
  declareProperty("CaloThreshold",        m_CaloThreshold = 0.1);
  declareProperty("Calo_HitMap_Thresh0",  m_CaloTT_HitMap_Thresh0 = 1);
  declareProperty("Calo_HitMap_Thresh1",  m_CaloTT_HitMap_Thresh1 = 3);
  declareProperty("Calo_HitMap_Thresh2",  m_CaloTT_HitMap_Thresh2 = 7);
  declareProperty("MaxEnergyRange",       m_MaxEnergyRange = 256) ;
  declareProperty("PathInRootFile",
                  m_PathInRootFile = "LVL1_Interfaces/Calorimeter") ;

}

/*---------------------------------------------------------*/
CalorimeterL1CaloMon::~CalorimeterL1CaloMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode CalorimeterL1CaloMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;

  StatusCode sc;
  
  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_ttTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate tool L1TriggerTowerTool" << endreq;
    return sc;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  sc = m_caloTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate Tool L1CaloMonitoringCaloTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode CalorimeterL1CaloMon:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CalorimeterL1CaloMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  
  msg(MSG::DEBUG) << "in CalorimeterL1CaloMon::bookHistograms" << endreq;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newLumiBlock) { }

  if ( newRun ) {	

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup Calo( this, m_PathInRootFile+"/CalorimeterData", run, attr );
    MonGroup Calo_HitMaps( this, m_PathInRootFile+"/CaloTriggerTowerHitMaps",
                                                              run, attr );
    MonGroup Calo_Dist( this,
      m_PathInRootFile+"/CaloTriggerTowerEnergyDistribution", run, attr );
    MonGroup Calo_TT_Comp( this, m_PathInRootFile+"/ComparisonTT2CaloTT",
                                                              run, attr );

    m_histTool->setMonGroup(&Calo);

    //Calorimeter Energy, eta, phi and hit map
     
    m_h_CaloCell_tile_et = m_histTool->book1F("CaloCell_tile_et",
      "Distribution of Tile CaloCell Energy;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_CaloCell_tile_eta = m_histTool->book1F("CaloCell_tile_eta",
      "#eta - Distribution of Tile CaloCells;eta", 98, -4.9, 4.9);
    m_h_CaloCell_tile_phi = m_histTool->book1F("CaloCell_tile_phi",
      "#phi - Distribution of Tile CaloCells;phi", 64, 0., 2*M_PI);
    m_h_CaloCell_tile_HitMap = m_histTool->book2F("CaloCell_tile_HitMap",
      "#eta - #phi Map of Tile CaloCells;eta;phi", 98, -4.9, 4.9, 64, 0., 2*M_PI);

    m_h_CaloCell_lar_et = m_histTool->book1F("CaloCell_lar_et",
      "Distribution of LAr CaloCell Energy;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_CaloCell_lar_eta = m_histTool->book1F("CaloCell_lar_eta",
      "#eta - Distribution of LAr CaloCells;eta", 98, -4.9, 4.9);
    m_h_CaloCell_lar_phi = m_histTool->book1F("CaloCell_lar_phi",
      "#phi - Distribution of LAr CaloCells;phi", 64, 0., 2*M_PI);
    m_h_CaloCell_lar_HitMap = m_histTool->book2F("CaloCell_lar_HitMap",
      "#eta - #phi Map of LAr CaloCells;eta;phi", 98, -4.9, 4.9, 64, 0., 2*M_PI);

    //CaloE Hitmaps per threshold

    m_histTool->setMonGroup(&Calo_HitMaps);

    std::stringstream buffer;
    buffer.str("");
    buffer << m_CaloTT_HitMap_Thresh0;
    m_h_CaloTT_HitMap_emLUT_Thresh0 = m_histTool->bookPPMEmEtaVsPhi(
      "CaloTTEmLUT"+buffer.str(),
      "#eta - #phi Map of Calo EM E_{T} > "+buffer.str());
      
    buffer.str("");
    buffer << m_CaloTT_HitMap_Thresh1;
    m_h_CaloTT_HitMap_emLUT_Thresh1 = m_histTool->bookPPMEmEtaVsPhi(
      "CaloTTEmLUT"+buffer.str(),
      "#eta - #phi Map of Calo EM E_{T} > "+buffer.str());
      
    buffer.str("");
    buffer << m_CaloTT_HitMap_Thresh2;
    m_h_CaloTT_HitMap_emLUT_Thresh2 = m_histTool->bookPPMEmEtaVsPhi(
      "CaloTTEmLUT"+buffer.str(),
      "#eta - #phi Map of Calo EM E_{T} > "+buffer.str());
      
    buffer.str("");
    buffer << m_CaloTT_HitMap_Thresh0;
    m_h_CaloTT_HitMap_hadLUT_Thresh0 = m_histTool->bookPPMHadEtaVsPhi(
      "CaloTTHadLUT"+buffer.str(),
      "#eta - #phi Map of Calo Had E_{T} > "+buffer.str());
      
    buffer.str("");
    buffer << m_CaloTT_HitMap_Thresh1;
    m_h_CaloTT_HitMap_hadLUT_Thresh1 = m_histTool->bookPPMHadEtaVsPhi(
      "CaloTTHadLUT"+buffer.str(),
      "#eta - #phi Map of Calo Had E_{T} > "+buffer.str());
      
    buffer.str("");
    buffer << m_CaloTT_HitMap_Thresh2;
    m_h_CaloTT_HitMap_hadLUT_Thresh2 = m_histTool->bookPPMHadEtaVsPhi(
      "CaloTTHadLUT"+buffer.str(),
      "#eta - #phi Map of Calo Had E_{T} > "+buffer.str());
      
    //distribution of CaloE LUT peak per detector region

    m_histTool->setMonGroup(&Calo_Dist);

    m_h_CaloTT_emLUT = m_histTool->book1F("CaloTTEMLUT_Dist",
      "Calo EM E_{T} Distribution;Et", m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_CaloTT_emLUT_eta = m_histTool->bookPPMEmEta("CaloTTEMLUT_eta",
      "Calo EM #eta Distribution");
    m_h_CaloTT_emLUT_phi = m_histTool->book1F("CaloTTEMLUT_phi",
      "Calo EM #phi Distribution;phi", 64, 0., 2*M_PI);

    m_h_CaloTT_hadLUT = m_histTool->book1F("CaloTTHADLUT_Dist",
      "Calo HAD E_{T} Distribution;Et", m_MaxEnergyRange, 0, m_MaxEnergyRange); 
    m_h_CaloTT_hadLUT_eta = m_histTool->bookPPMHadEta("CaloTTHADLUT_eta",
      "Calo HAD #eta Distribution");
    m_h_CaloTT_hadLUT_phi = m_histTool->book1F("CaloTTHADLUT_phi",
      "Calo HAD #phi Distribution;phi",64,0,2*M_PI);

    //comparison between CaloE and L1CaloE

    m_histTool->setMonGroup(&Calo_TT_Comp);

    m_h_emEnergy_Match_CaloTT_TT_LArEMB = m_histTool->book2F(
      "emEnergy_CompTT2CaloTT_LArEMB",
      "LArEMB Match between Calo and L1Calo em E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Match_CaloTT_TT_LArOverlap = m_histTool->book2F(
      "emEnergy_CompTT2CaloTT_LArOverlap",
      "LArOverlap Match between Calo and L1Calo em E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Match_CaloTT_TT_LArEMEC = m_histTool->book2F(
      "emEnergy_CompTT2CaloTT_LArEMEC",
      "LArEMEC Match between Calo and L1Calo em E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Match_CaloTT_TT_LArFCAL1 = m_histTool->book2F(
      "emEnergy_CompTT2CaloTT_LArFCAL1",
      "LArFCAL1 Match between Calo and L1Calo em E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);

    m_h_hadEnergy_Match_CaloTT_TT_TileLB = m_histTool->book2F(
      "hadEnergy_CompTT2CaloTT_TileLB",
      "TileLB Match between Calo and L1Calo had E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Match_CaloTT_TT_TileEB = m_histTool->book2F(
      "hadEnergy_CompTT2CaloTT_TileEB",
      "TileEB Match between Calo and L1Calo had E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Match_CaloTT_TT_LArHEC = m_histTool->book2F(
      "hadEnergy_CompTT2CaloTT_LArHEC",
      "LArHEC Match between Calo and L1Calo had E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23 = m_histTool->book2F(
      "hadEnergy_CompTT2CaloTT_LArFCAL23",
      "LArFCAL23 Match between Calo and L1Calo had E_{T} (JEP);L1Calo;Calo",
      m_MaxEnergyRange, 0, m_MaxEnergyRange,
      m_MaxEnergyRange, 0, m_MaxEnergyRange);

   m_h_emEnergy_Match_CaloTT_TT_LArEMB_CP = m_histTool->book2F(
    "emEnergy_CompTT2CaloTT_LArEMB_CP",
    "LArEMB Match between Calo and L1Calo em E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);
  m_h_emEnergy_Match_CaloTT_TT_LArOverlap_CP = m_histTool->book2F(
    "emEnergy_CompTT2CaloTT_LArOverlap_CP",
    "LArOverlap Match between Calo and L1Calo em E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);
  m_h_emEnergy_Match_CaloTT_TT_LArEMEC_CP = m_histTool->book2F(
    "emEnergy_CompTT2CaloTT_LArEMEC_CP",
    "LArEMEC Match between Calo and L1Calo em E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);
  m_h_emEnergy_Match_CaloTT_TT_LArFCAL1_CP = m_histTool->book2F(
    "emEnergy_CompTT2CaloTT_LArFCAL1_CP",
    "LArFCAL1 Match between Calo and L1Calo em E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);

  m_h_hadEnergy_Match_CaloTT_TT_TileLB_CP = m_histTool->book2F(
    "hadEnergy_CompTT2CaloTT_TileLB_CP",
    "TileLB Match between Calo and L1Calo had E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);
  m_h_hadEnergy_Match_CaloTT_TT_TileEB_CP = m_histTool->book2F(
    "hadEnergy_CompTT2CaloTT_TileEB_CP",
    "TileEB Match between Calo and L1Calo had E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);
  m_h_hadEnergy_Match_CaloTT_TT_LArHEC_CP = m_histTool->book2F(
    "hadEnergy_CompTT2CaloTT_LArHEC_CP",
    "LArHEC Match between Calo and L1Calo had E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);
  m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23_CP = m_histTool->book2F(
    "hadEnergy_CompTT2CaloTT_LArFCAL23_CP",
    "LArFCAL23 Match between Calo and L1Calo had E_{T} (CP);L1Calo;Calo",
    m_MaxEnergyRange, 0, m_MaxEnergyRange,
    m_MaxEnergyRange, 0, m_MaxEnergyRange);

    m_h_em_profile_etaRegion = m_histTool->bookProfile(
      "emEnergy_eta_DiffTT2CaloTT",
      "Relative difference in eta: (L1Calo (JEP) - Calo) / Calo em E_{T};eta;(L1Calo-Calo)/Calo Et", 98, -4.9, 4.9);
    m_h_had_profile_etaRegion = m_histTool->bookProfile(
      "hadEnergy_eta_DiffTT2CaloTT",
      "Relative difference in eta: (L1Calo (JEP) - Calo) / Calo had E_{T};eta;(L1Calo-Calo)/Calo Et", 98, -4.9, 4.9);

    m_h_em_profile_etaRegion_CP = m_histTool->bookProfile(
      "emEnergy_eta_DiffTT2CaloTT_CP",
      "Relative difference in eta: (L1Calo (CP) - Calo) / Calo em E_{T};eta;(L1Calo-Calo)/Calo Et", 98, -4.9, 4.9);
    m_h_had_profile_etaRegion_CP = m_histTool->bookProfile(
      "hadEnergy_eta_DiffTT2CaloTT_CP",
      "Relative difference in eta: (L1Calo (CP) - Calo) / Calo had E_{T};eta;(L1Calo-Calo)/Calo Et", 98, -4.9, 4.9);
    
    /*m_h_em_Mismatch_etaphi_alt = m_histTool->bookPPMEmEtaVsPhi(
      "em_Mismatch_etaphi_alt",
      "#eta-#phi map of em mismatches with L1Calo (JEP) E_{T} > 5 and zero Calo E_{T}");
    m_h_em_Mismatch_etaphi = m_histTool->bookPPMEmEtaVsPhi(
      "em_Mismatch_etaphi",
      "#eta-#phi map of em mismatches with zero L1Calo (JEP) E_{T} and Calo E_{T} > 5");
    m_h_had_Mismatch_etaphi_alt = m_histTool->bookPPMHadEtaVsPhi(
      "had_Mismatch_etaphi_alt",
      "#eta-#phi map of had mismatches with L1Calo (JEP) E_{T} > 5 and zero Calo E_{T}");
    m_h_had_Mismatch_etaphi = m_histTool->bookPPMHadEtaVsPhi(
      "had_Mismatch_etaphi",
      "#eta-#phi map of had mismatches with zero L1Calo (JEP) E_{T} and Calo E_{T} > 5");
    
    m_h_em_Mismatch_etaphi_alt_CP = m_histTool->bookPPMEmEtaVsPhi(
      "em_Mismatch_etaphi_alt_CP",
      "#eta-#phi map of em mismatches with L1Calo (CP) E_{T} > 5 and zero Calo E_{T}");
    m_h_em_Mismatch_etaphi_CP = m_histTool->bookPPMEmEtaVsPhi(
      "em_Mismatch_etaphi_CP",
      "#eta-#phi map of em mismatches with zero L1Calo (CP) E_{T} and Calo E_{T} > 5");
    m_h_had_Mismatch_etaphi_alt_CP = m_histTool->bookPPMHadEtaVsPhi(
      "had_Mismatch_etaphi_alt_CP",
      "#eta-#phi map of had mismatches with L1Calo (CP) E_{T} > 5 and zero Calo E_{T}");
    m_h_had_Mismatch_etaphi_CP = m_histTool->bookPPMHadEtaVsPhi(
      "had_Mismatch_etaphi_CP",
      "#eta-#phi map of had mismatches with zero L1Calo (CP) E_{T} and Calo E_{T} > 5");
    */
    m_h_em_1D_Match_LArEMB = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArEMB",
      "LArEMB relative difference between L1Calo (JEP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_em_1D_Match_LArOverlap = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArOverlap",
      "LArOverlap relative difference between L1Calo (JEP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_em_1D_Match_LArEMEC = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArEMEC",
      "LArEMEC relative difference between L1Calo (JEP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_em_1D_Match_LArFCAL1 = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArFCAL1",
      "LArFCAL1 relative difference between L1Calo (JEP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
      
    m_h_had_1D_Match_TileLB = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_TileLB",
      "TileLB relative difference between L1Calo (JEP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_had_1D_Match_TileEB = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_TileEB",
      "TileEB relative difference between L1Calo (JEP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_had_1D_Match_LArHEC = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_LArHEC",
      "LArHEC relative difference between L1Calo (JEP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_had_1D_Match_LArFCAL23 = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_LArFCAL23",
      "LArFCAL23 relative difference between L1Calo (JEP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    
    m_h_em_1D_Match_LArEMB_CP = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArEMB_CP",
      "LArEMB relative difference between L1Calo (CP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_em_1D_Match_LArOverlap_CP = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArOverlap_CP",
      "LArOverlap relative difference between L1Calo (CP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_em_1D_Match_LArEMEC_CP = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArEMEC_CP",
      "LArEMEC relative difference between L1Calo (CP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_em_1D_Match_LArFCAL1_CP = m_histTool->book1F(
      "emEnergy_DiffCaloL1CaloE_LArFCAL1_CP",
      "LArFCAL1 relative difference between L1Calo (CP) and Calo em E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
      
    m_h_had_1D_Match_TileLB_CP = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_TileLB_CP",
      "TileLB relative difference between L1Calo (CP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_had_1D_Match_TileEB_CP = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_TileEB_CP",
      "TileEB relative difference between L1Calo (CP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_had_1D_Match_LArHEC_CP = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_LArHEC_CP",
      "LArHEC relative difference between L1Calo (CP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);
    m_h_had_1D_Match_LArFCAL23_CP = m_histTool->book1F(
      "hadEnergy_DiffCaloL1CaloE_LArFCAL23_CP",
      "LArFCAL23 relative difference between L1Calo (CP) and Calo had E_{T};(L1Calo-Calo)/Calo Et", 24, -0.6, 0.6);

    m_h_em_profile_Match = m_histTool->bookProfile("emEnergy_DiffTT2CaloTT",
      "Relative difference between L1Calo (JEP) and Calo em E_{T};L1Calo Et;(L1Calo-Calo)/Calo Et",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_had_profile_Match = m_histTool->bookProfile("hadEnergy_DiffTT2CaloTT",
      "Relative difference between L1Calo (JEP) and Calo had E_{T};L1Calo Et;(L1Calo-Calo)/Calo Et",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_em_profile_phiRegion = m_histTool->bookProfile(
      "emEnergy_phi_DiffTT2CaloTT",
      "Relative difference in phi: L1Calo (JEP) and Calo em E_{T};phi;(L1Calo-Calo)/Calo Et", 64, 0., 2.*M_PI);
    m_h_had_profile_phiRegion = m_histTool->bookProfile(
      "hadEnergy_phi_DiffTT2CaloTT",
      "Relative difference in phi: L1Calo (JEP) and Calo  had E_{T};phi;(L1Calo-Calo)/Calo Et", 64, 0., 2.*M_PI);
    
    m_h_em_profile_Match_CP = m_histTool->bookProfile("emEnergy_DiffTT2CaloTT_CP",
      "Relative difference between L1Calo (CP) and Calo em E_{T};L1Calo Et;(L1Calo-Calo)/Calo Et",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_had_profile_Match_CP = m_histTool->bookProfile("hadEnergy_DiffTT2CaloTT_CP",
      "Relative difference between L1Calo (CP) and Calo had E_{T};L1Calo Et;(L1Calo-Calo)/Calo Et",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_em_profile_phiRegion_CP = m_histTool->bookProfile(
      "emEnergy_phi_DiffTT2CaloTT_CP",
      "Relative difference in phi: L1Calo (CP) and Calo em E_{T};phi;(L1Calo-Calo)/Calo Et", 64, 0., 2.*M_PI);
    m_h_had_profile_phiRegion_CP = m_histTool->bookProfile(
      "hadEnergy_phi_DiffTT2CaloTT_CP",
      "Relative difference in phi: L1Calo (CP) and Calo  had E_{T};phi;(L1Calo-Calo)/Calo Et", 64, 0., 2.*M_PI);

    m_h_average_emDE_map = m_histTool->bookProfilePPMEmEtaVsPhi(
      "emDeltaEnergy_Map",
      "#eta-#phi map of average (L1Calo (JEP) - Calo) / Calo em E_{T} relative difference");
    m_h_average_hadDE_map = m_histTool->bookProfilePPMHadEtaVsPhi(
      "hadDeltaEnergy_Map",
      "#eta-#phi map of average (L1Calo (JEP) - Calo) / Calo had E_{T} relative difference");

    m_h_average_emDE_map_CP = m_histTool->bookProfilePPMEmEtaVsPhi(
      "emDeltaEnergy_Map_CP",
      "#eta-#phi map of average (L1Calo (CP) - Calo) / Calo em E_{T} relative difference");
    m_h_average_hadDE_map_CP = m_histTool->bookProfilePPMHadEtaVsPhi(
      "hadDeltaEnergy_Map_CP",
      "#eta-#phi map of average (L1Calo (CP) - Calo) / Calo had E_{T} relative difference");

    m_h_emEnergy_Calo_L1CaloSat_LArEMB = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArEMB",
      "Distribution of LArEMB Calo em Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Calo_L1CaloSat_LArOverlap = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArOverlap",
      "Distribution of LArOverlap Calo em Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Calo_L1CaloSat_LArEMEC = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArEMEC",
      "Distribution of LArEMEC Calo em Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Calo_L1CaloSat_LArFCAL1 = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArFCAL1",
      "Distribution of LArFCAL1 Calo em Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_TileLB = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_TileLB",
      "Distribution of TileLB Calo had Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_TileEB = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_TileEB",
      "Distribution of TileEB Calo had Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_LArHEC = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_LArHEC",
      "Distribution of LArHEC Calo had Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_LArFCAL23 = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_LArFCAL23",
      "Distribution of LArFCAL23 Calo had Et for L1Calo JEP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);

    m_h_emEnergy_Calo_L1CaloSat_LArEMB_CP = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArEMB_CP",
      "Distribution of LArEMB Calo em Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Calo_L1CaloSat_LArOverlap_CP = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArOverlap_CP",
      "Distribution of LArOverlap Calo em Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Calo_L1CaloSat_LArEMEC_CP = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArEMEC_CP",
      "Distribution of LArEMEC Calo em Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_emEnergy_Calo_L1CaloSat_LArFCAL1_CP = m_histTool->book1F("emEnergy_Calo_L1CaloSat_LArFCAL1_CP",
      "Distribution of LArFCAL1 Calo em Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_TileLB_CP = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_TileLB_CP",
      "Distribution of TileLB Calo had Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_TileEB_CP = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_TileEB_CP",
      "Distribution of TileEB Calo had Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_LArHEC_CP = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_LArHEC_CP",
      "Distribution of LArHEC Calo had Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);
    m_h_hadEnergy_Calo_L1CaloSat_LArFCAL23_CP = m_histTool->book1F("hadEnergy_Calo_L1CaloSat_LArFCAL23_CP",
      "Distribution of LArFCAL23 Calo had Et for L1Calo CP Et = 255;energy",
      m_MaxEnergyRange, 0, m_MaxEnergyRange);

    m_h_emTTME_etaphi = m_histTool->bookPPMEmEtaVsPhi(
      "em_MostEnergetic_TTMap", "#eta-#phi map of em most energetic TT (JEP)");
    m_h_hadTTME_etaphi = m_histTool->bookPPMHadEtaVsPhi(
      "had_MostEnergetic_TTMap", "#eta-#phi map of had most energetic TT (JEP)");

    m_h_emTTME_etaphi_CP = m_histTool->bookPPMEmEtaVsPhi(
      "em_MostEnergetic_TTMap_CP", "#eta-#phi map of em most energetic TT (CP)");
    m_h_hadTTME_etaphi_CP = m_histTool->bookPPMEmEtaVsPhi(
      "had_MostEnergetic_TTMap_CP", "#eta-#phi map of had most energetic TT (CP)");

    m_h_emcaloME_etaphi = m_histTool->bookPPMEmEtaVsPhi(
      "em_MostEnergetic_caloMap", "#eta-#phi map of em most energetic caloTT");
    m_h_hadcaloME_etaphi = m_histTool->bookPPMHadEtaVsPhi(
      "had_MostEnergetic_caloMap",
      "#eta-#phi map of had most energetic caloTT");

    m_histTool->unsetMonGroup();
    m_histBooked = true;

    StatusCode sc = m_ttTool->retrieveConditions();
    if(!sc.isSuccess()) {
      msg(MSG::WARNING) << "Conditions not retrieved " << endreq;
    }

  }

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode CalorimeterL1CaloMon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "in fillHistograms()" << endreq;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc;

  //Retreive Calo Cell collection from SG

  const CaloCellContainer* caloCellContainer = 0;
  sc = evtStore()->retrieve(caloCellContainer, m_caloCellContainerName); 
  if(!sc.isSuccess() || !caloCellContainer) {
    msg(MSG::WARNING) << "No CaloCellContainer found at AllCalo" << endreq; 
    return StatusCode::SUCCESS;
  }
  sc = m_caloTool->loadCaloCells();
  if(!sc.isSuccess()) {
    msg(MSG::WARNING) << "Could not load CaloCells" << endreq;
    return StatusCode::SUCCESS;
  }

  // =========================================================================
  // ================= CaloCells  ============================================
  // =========================================================================
  
  // CaloCells exist irrespective of how low the Energy is, 
  // so need an Et cut to select only those towers with a deposited energy
  
  CaloCellContainer::const_iterator CaloCellIterator    = caloCellContainer->begin();
  CaloCellContainer::const_iterator CaloCellIteratorEnd = caloCellContainer->end();
  
  for (; CaloCellIterator != CaloCellIteratorEnd; ++CaloCellIterator) {
      
    //select only Cells with an energy deposit
    double energy = (*CaloCellIterator)->energy()*1e-3;
    if (energy > m_CaloThreshold) {
      const CaloDetDescrElement* caloDDE = (*CaloCellIterator)->caloDDE();
      if (!usedInTT(caloDDE)) continue;
      double eta = caloDDE->eta_raw();
      double phi = caloDDE->phi_raw();
      if (phi < 0) phi += 2*M_PI;

      if (caloDDE->is_tile()) {
        m_h_CaloCell_tile_phi->Fill(phi);
        m_h_CaloCell_tile_eta->Fill(eta);
        m_h_CaloCell_tile_HitMap->Fill(eta, phi);
        m_h_CaloCell_tile_et->Fill(energy);
      } else {
        m_h_CaloCell_lar_phi->Fill(phi);
        m_h_CaloCell_lar_eta->Fill(eta);
        m_h_CaloCell_lar_HitMap->Fill(eta, phi);
        m_h_CaloCell_lar_et->Fill(energy);
      }
    }
  }

  // =========================================================================
  // =================== comparison between CaloE and L1CaloE ================
  // =========================================================================

  const xAOD::TriggerTowerContainer* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_xAODTriggerTowerContainerName); 
  if(sc==StatusCode::FAILURE || !triggerTowerTES) {           
    msg(MSG::INFO) << "No xAODTriggerTower found in TES at "
                   << m_xAODTriggerTowerContainerName << endreq ; 
    return StatusCode::SUCCESS;
  }                                                           
    
  xAOD::TriggerTowerContainer::const_iterator ttIterator =
	      triggerTowerTES->begin();
  xAOD::TriggerTowerContainer::const_iterator ttIteratorEnd =
	      triggerTowerTES->end();
      
  double em_maximum  = 0;
  double had_maximum = 0;
  double em_maximum_CP = 0;
  double had_maximum_CP = 0;
  double caloem_maximum  = 0;
  double calohad_maximum = 0;
      
  double eta_max_had = -10;
  double eta_max_em  = -10;
  double phi_max_had = -10;
  double phi_max_em  = -10;
  double eta_max_had_CP = -10;
  double eta_max_em_CP  = -10;
  double phi_max_had_CP = -10;
  double phi_max_em_CP  = -10;
  double eta_max_calohad = -10;
  double eta_max_caloem  = -10;
  double phi_max_calohad = -10;
  double phi_max_caloem  = -10;
      
  L1CaloCoolChannelId em_coolId;
  L1CaloCoolChannelId had_coolId;

  double em_caloE    = 0;
  double had_caloE   = 0;
  double em_L1CaloE  = 0;
  double had_L1CaloE = 0;
  double em_L1CaloE_CP = 0;
  double had_L1CaloE_CP = 0;

  for (; ttIterator != ttIteratorEnd; ++ttIterator) {
    double eta = (*ttIterator)->eta();
    double absEta = fabs(eta);
    double phi = (*ttIterator)->phi();
    const int layer = (*ttIterator)->layer();      
    const xAOD::TriggerTower_v2* tt = *ttIterator; 
    std::vector<float> CaloEnergyLayers;           
    std::vector<float> CaloETLayers;               
    
    em_caloE = 0;
    had_caloE = 0;
    em_L1CaloE = 0;
    had_L1CaloE = 0;
    em_L1CaloE_CP = 0;
    had_L1CaloE_CP = 0;
    
    if (!(tt->jepET() || tt->cpET())) continue;
 
    /********* EM LAYER *********/
    
    if (layer == 0) { //EM
      
      // Get L1Calo ET
      em_L1CaloE = int(tt->jepET());
      em_L1CaloE_CP = int(0.5*(tt->cpET()));  // CP scale hardcoded for now, but this should be updated to be taken automatically

      // Get Calo  ET
      CaloEnergyLayers = tt->auxdataConst< std::vector<float> > ("CaloCellEnergyByLayer");
      CaloETLayers = tt->auxdataConst< std::vector<float> > ("CaloCellETByLayer");
      
      if (absEta < 3.2) {
        if ( tt->isAvailable< std::vector<float> > ("CaloCellEnergyByLayer") ) { 
          for (std::vector<float>::iterator it = CaloEnergyLayers.begin(); it != CaloEnergyLayers.end(); it++) {  
            em_caloE += *it;
          }
          em_caloE = em_caloE / cosh(eta); 
        }
      } else {  // FCal: need to use different method due to large variation in cell eta 
        if ( tt->isAvailable< std::vector<float> > ("CaloCellETByLayer") ) {
          for (std::vector<float>::iterator it = CaloETLayers.begin(); it != CaloETLayers.end(); it++) {
            em_caloE += *it;
          }
        }
      }

      if (em_caloE < 0) em_caloE = 0; //convert -ve energies to 0 
      em_caloE = int(em_caloE+0.5); //round calo energy to nearest GeV for comparison with L1 energies
      if (em_caloE > 255) em_caloE = 255; //set calo tt energies to saturate
    
      // Comparison of L1Calo (JEP) and Calo 
      double emDiffE = 0.;
      double emRelDiffE = 0.;
      
      //if (em_L1CaloE > 5 || em_caloE > 5) { // KW change OR to AND to remove low-ET noise 
      if (em_L1CaloE > 5 && em_caloE > 5) {  
        em_coolId = m_ttTool->channelID(eta, phi, 0);
        bool em_disabled = m_ttTool->disabledChannel(em_coolId);
        if (!em_disabled) {
	    
          emDiffE = em_L1CaloE-em_caloE;
          
          if (em_caloE != 0) {
            emRelDiffE = emDiffE / em_caloE;
            //if (em_L1CaloE < 255) {
              m_h_em_profile_Match->Fill(em_L1CaloE, emRelDiffE);
              m_h_em_profile_etaRegion->Fill(eta, emRelDiffE);                           
              m_histTool->fillPPMPhi(m_h_em_profile_phiRegion, eta, phi, emRelDiffE);
	      m_histTool->fillPPMEmEtaVsPhi(m_h_average_emDE_map, eta, phi, emRelDiffE);
           // }
          }

          /*if (em_L1CaloE == 0 && em_caloE > 5) {                                   // KW commenting out for now... will never be filled with new ET cut
            m_histTool->fillPPMEmEtaVsPhi(m_h_em_Mismatch_etaphi, eta, phi);
          }
          if (em_L1CaloE > 5 && em_caloE == 0) {
            m_histTool->fillPPMEmEtaVsPhi(m_h_em_Mismatch_etaphi_alt, eta, phi);
          }*/

          if (absEta < 1.4) {
            if (em_L1CaloE >= 255) {
              m_h_emEnergy_Calo_L1CaloSat_LArEMB->Fill(em_caloE);
            } else {
             if (em_caloE != 0) m_h_em_1D_Match_LArEMB->Fill(emRelDiffE);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArEMB->Fill(em_L1CaloE, em_caloE);
          } else if (absEta < 1.5) {
            if (em_L1CaloE >= 255) {
              m_h_emEnergy_Calo_L1CaloSat_LArOverlap->Fill(em_caloE);
            } else {
              if (em_caloE!= 0) m_h_em_1D_Match_LArOverlap->Fill(emRelDiffE);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArOverlap->Fill(em_L1CaloE, em_caloE);
          } else if (absEta < 3.2) {
            if (em_L1CaloE >= 255) {
              m_h_emEnergy_Calo_L1CaloSat_LArEMEC->Fill(em_caloE);
            } else { 
              if (em_caloE != 0) m_h_em_1D_Match_LArEMEC->Fill(emRelDiffE);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArEMEC->Fill(em_L1CaloE, em_caloE);
          } else if (absEta < 4.9) {
            if (em_L1CaloE >= 255) {
              m_h_emEnergy_Calo_L1CaloSat_LArFCAL1->Fill(em_caloE);
            } else {
              if (em_caloE != 0) m_h_em_1D_Match_LArFCAL1->Fill(emRelDiffE);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArFCAL1->Fill(em_L1CaloE, em_caloE);
          }   
        }
      }

      // Comparison of L1Calo (CP) and Calo 
      
      double emDiffE_CP = 0.;
      double emRelDiffE_CP = 0.;

      //if (em_L1CaloE_CP>5 || em_caloE>5) {
      if (em_L1CaloE_CP > 5 && em_caloE > 5) {            // KW change OR to AND to remove low-ET noise  
        em_coolId = m_ttTool->channelID(eta, phi, 0);
        bool em_disabled = m_ttTool->disabledChannel(em_coolId);
        if (!em_disabled) {
	    
          emDiffE_CP = em_L1CaloE_CP-em_caloE;
          
          if (em_caloE != 0) {
            emRelDiffE_CP = emDiffE_CP / em_caloE;
            //if (em_L1CaloE_CP < 255) {  
              m_h_em_profile_Match_CP->Fill(em_L1CaloE_CP, emRelDiffE_CP);
              m_h_em_profile_etaRegion_CP->Fill(eta, emRelDiffE_CP);
              m_histTool->fillPPMPhi(m_h_em_profile_phiRegion_CP, eta, phi, emRelDiffE_CP);
	      m_histTool->fillPPMEmEtaVsPhi(m_h_average_emDE_map_CP, eta, phi, emRelDiffE_CP);
           // } 
	  } 
          /*                                        
          if (em_L1CaloE_CP == 0 && em_caloE > 5) {                              // KW commenting out for now... will never be filled with new ET cut
            m_histTool->fillPPMEmEtaVsPhi(m_h_em_Mismatch_etaphi_CP, eta, phi);
          }
          if (em_L1CaloE_CP > 5 && em_caloE == 0) {
            m_histTool->fillPPMEmEtaVsPhi(m_h_em_Mismatch_etaphi_alt_CP, eta, phi);
          }
          */

          if (absEta < 1.4) {
            if (em_L1CaloE_CP >= 127) {
              m_h_emEnergy_Calo_L1CaloSat_LArEMB_CP->Fill(em_caloE);
            } else {
              if (em_caloE != 0) m_h_em_1D_Match_LArEMB_CP->Fill(emRelDiffE_CP);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArEMB_CP->Fill(em_L1CaloE_CP, em_caloE);
          } else if (absEta < 1.5) {
            if (em_L1CaloE_CP >= 127) {
              m_h_emEnergy_Calo_L1CaloSat_LArOverlap_CP->Fill(em_caloE);
            } else {
              if (em_caloE != 0) m_h_em_1D_Match_LArOverlap_CP->Fill(emRelDiffE_CP);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArOverlap_CP->Fill(em_L1CaloE_CP, em_caloE);
          } else if (absEta < 3.2) {
            if (em_L1CaloE_CP >= 127) {
              m_h_emEnergy_Calo_L1CaloSat_LArEMEC_CP->Fill(em_caloE);
            } else {
              if (em_caloE != 0)  m_h_em_1D_Match_LArEMEC_CP->Fill(emRelDiffE_CP);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArEMEC_CP->Fill(em_L1CaloE_CP, em_caloE);
          } else if (absEta < 4.9) {
            if (em_L1CaloE_CP >= 127) {
              m_h_emEnergy_Calo_L1CaloSat_LArFCAL1_CP->Fill(em_caloE);
            } else {
             if (em_caloE != 0) m_h_em_1D_Match_LArFCAL1_CP->Fill(emRelDiffE_CP);
            }
            m_h_emEnergy_Match_CaloTT_TT_LArFCAL1_CP->Fill(em_L1CaloE_CP, em_caloE);
          }
        }
      }

      if (em_maximum < em_L1CaloE) {	      
        em_maximum = em_L1CaloE; 
        eta_max_em = eta;
        phi_max_em = phi;
      }

      if (em_maximum_CP < em_L1CaloE_CP) {
        em_maximum_CP = em_L1CaloE_CP;
        eta_max_em_CP = eta;
        phi_max_em_CP = phi;
      }

      if(caloem_maximum < em_caloE) {
        caloem_maximum = em_caloE;
        eta_max_caloem = eta;
        phi_max_caloem = phi;
      }
	  
      if (em_caloE > 0) {
        m_h_CaloTT_emLUT_eta-> Fill(eta);
        m_histTool->fillPPMPhi(m_h_CaloTT_emLUT_phi, eta, phi);
        m_h_CaloTT_emLUT->Fill(em_caloE);
      }
	  
      if (em_caloE > m_CaloTT_HitMap_Thresh0) {
        m_histTool->fillPPMEmEtaVsPhi(m_h_CaloTT_HitMap_emLUT_Thresh0, eta, phi);
      }
      if (em_caloE > m_CaloTT_HitMap_Thresh1) {
        m_histTool->fillPPMEmEtaVsPhi(m_h_CaloTT_HitMap_emLUT_Thresh1, eta, phi);
      }
      if (em_caloE > m_CaloTT_HitMap_Thresh2) {
        m_histTool->fillPPMEmEtaVsPhi(m_h_CaloTT_HitMap_emLUT_Thresh2, eta, phi);
      }

    } // End of EM Layer
    
    /********* HAD LAYER *********/
    if (layer == 1) { //HAD
      
      // Get L1Calo ET
      had_L1CaloE = int(tt->jepET());
      had_L1CaloE_CP = int(0.5*(tt->cpET())); // CP scale hardcoded for now, but this should be updated to be taken automatically
       
      // Get Calo ET
      CaloEnergyLayers = tt->auxdataConst< std::vector<float> > ("CaloCellEnergyByLayer");
      CaloETLayers = tt->auxdataConst< std::vector<float> > ("CaloCellETByLayer");

      if (absEta < 3.2) {
        if ( tt->isAvailable< std::vector<float> > ("CaloCellEnergyByLayer") ) {
          for (std::vector<float>::iterator it = CaloEnergyLayers.begin(); it != CaloEnergyLayers.end(); it++) {
            had_caloE += *it;
          }
          had_caloE = had_caloE / cosh(eta);
        }
      } else {  // FCal: need to use different method due to large variation in cell eta 
        if ( tt->isAvailable< std::vector<float> > ("CaloCellETByLayer") ) {
          for (std::vector<float>::iterator it = CaloETLayers.begin(); it != CaloETLayers.end(); it++) {
            had_caloE += *it;
          }
        }
      }

      if (had_caloE < 0) had_caloE = 0; //convert -ve energies to 0 
      had_caloE = int(had_caloE+0.5); //round calo energy to nearest GeV for comparison with L1 energies
      if (had_caloE > 255) had_caloE = 255; //set calo tt energies to saturate
    
      // Comparison of L1Calo (JEP) and Calo 
      //if (had_L1CaloE>5 || had_caloE>5) {                         // KW change OR to AND to remove low-ET noise
      if (had_L1CaloE > 5 && had_caloE > 5) { 
        had_coolId = m_ttTool->channelID(eta, phi, 0);
        bool had_disabled = m_ttTool->disabledChannel(had_coolId);
        if (!had_disabled) {
	    
          double hadDiffE = had_L1CaloE-had_caloE;
          double hadRelDiffE = 0.;
          
          if (had_caloE != 0) {
            hadRelDiffE = hadDiffE / had_caloE;
            //if (had_L1CaloE < 255) {
              m_h_had_profile_Match->Fill(had_L1CaloE, hadRelDiffE);
              m_h_had_profile_etaRegion->Fill(eta, hadRelDiffE);                           
              m_histTool->fillPPMPhi(m_h_had_profile_phiRegion, eta, phi, hadRelDiffE);
	      m_histTool->fillPPMHadEtaVsPhi(m_h_average_hadDE_map, eta, phi, hadRelDiffE);
           // }
          }

          /*if (had_L1CaloE == 0 && had_caloE > 5) {           // KW commenting out for now... will never be filled with new ET cut
            m_histTool->fillPPMHadEtaVsPhi(m_h_had_Mismatch_etaphi, eta, phi);
          }
          if (had_L1CaloE > 5 && had_caloE == 0) {
            m_histTool->fillPPMHadEtaVsPhi(m_h_had_Mismatch_etaphi_alt, eta, phi);
          }*/

          if (absEta < 0.9) {
            if (had_L1CaloE >= 255) {
              m_h_hadEnergy_Calo_L1CaloSat_TileLB->Fill(had_caloE);
            } else {
             if (had_caloE != 0) m_h_had_1D_Match_TileLB->Fill(hadRelDiffE);
            }
            m_h_hadEnergy_Match_CaloTT_TT_TileLB->Fill(had_L1CaloE, had_caloE);
          } else if (absEta < 1.5) {
            if (had_L1CaloE >= 255) {
              m_h_hadEnergy_Calo_L1CaloSat_TileEB->Fill(had_caloE);
            } else {
              if (had_caloE!= 0) m_h_had_1D_Match_TileEB->Fill(hadRelDiffE);
            }
            m_h_hadEnergy_Match_CaloTT_TT_TileEB->Fill(had_L1CaloE, had_caloE);
          } else if (absEta < 3.2) {
            if (had_L1CaloE >= 255) {
              m_h_hadEnergy_Calo_L1CaloSat_LArHEC->Fill(had_caloE);
            } else { 
              if (had_caloE != 0) m_h_had_1D_Match_LArHEC->Fill(hadRelDiffE);
            }
            m_h_hadEnergy_Match_CaloTT_TT_LArHEC->Fill(had_L1CaloE, had_caloE);
          } else if (absEta < 4.9) {
            if (had_L1CaloE >= 255) {
              m_h_hadEnergy_Calo_L1CaloSat_LArFCAL23->Fill(had_caloE);
            } else {
              if (had_caloE != 0) m_h_had_1D_Match_LArFCAL23->Fill(hadRelDiffE);
            }
            m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23->Fill(had_L1CaloE, had_caloE);
          }   
        }
      }

      // Comparison of L1Calo (CP) and Calo 
      //if (had_L1CaloE_CP>5 || had_caloE>5) {
      if (had_L1CaloE_CP > 5 && had_caloE > 5) {           // KW change OR to AND to remove low-ET noise 
        had_coolId = m_ttTool->channelID(eta, phi, 0);
        bool had_disabled = m_ttTool->disabledChannel(had_coolId);
        if (!had_disabled) {
	    
          double hadDiffE_CP = had_L1CaloE_CP-had_caloE;
          double hadRelDiffE_CP = 0;
          
          if (had_caloE != 0) {
            hadRelDiffE_CP = hadDiffE_CP / had_caloE;
            //if (had_L1CaloE_CP < 255) {
              m_h_had_profile_Match_CP->Fill(had_L1CaloE_CP, hadRelDiffE_CP);
              m_h_had_profile_etaRegion_CP->Fill(eta, hadRelDiffE_CP);
              m_histTool->fillPPMPhi(m_h_had_profile_phiRegion_CP, eta, phi, hadRelDiffE_CP);
	      m_histTool->fillPPMHadEtaVsPhi(m_h_average_hadDE_map_CP, eta, phi, hadRelDiffE_CP);
           // }
	  }  
          /*if (had_L1CaloE_CP == 0 && had_caloE > 5) {     // KW commenting out for now... will never be filled with new ET cut
            m_histTool->fillPPMHadEtaVsPhi(m_h_had_Mismatch_etaphi_CP, eta, phi);
          }
          if (had_L1CaloE_CP > 5 && had_caloE == 0) {
            m_histTool->fillPPMHadEtaVsPhi(m_h_had_Mismatch_etaphi_alt_CP, eta, phi);
          }*/

          if (absEta < 0.9) {
            if (had_L1CaloE_CP >= 127) {
              m_h_hadEnergy_Calo_L1CaloSat_TileLB_CP->Fill(had_caloE);
            } else {
              if (had_caloE != 0) m_h_had_1D_Match_TileLB_CP->Fill(hadRelDiffE_CP);
            }
            m_h_hadEnergy_Match_CaloTT_TT_TileLB_CP->Fill(had_L1CaloE_CP, had_caloE);
          } else if (absEta < 1.5) {
            if (had_L1CaloE_CP >= 127) {
              m_h_hadEnergy_Calo_L1CaloSat_TileEB_CP->Fill(had_caloE);
            } else {
              if (had_caloE != 0) m_h_had_1D_Match_TileEB_CP->Fill(hadRelDiffE_CP);
            }
            m_h_hadEnergy_Match_CaloTT_TT_TileEB_CP->Fill(had_L1CaloE_CP, had_caloE);
          } else if (absEta < 3.2) {
            if (had_L1CaloE_CP >= 127) {
              m_h_hadEnergy_Calo_L1CaloSat_LArHEC_CP->Fill(had_caloE);
            } else {
              if (had_caloE != 0)  m_h_had_1D_Match_LArHEC_CP->Fill(hadRelDiffE_CP);
            }
            m_h_hadEnergy_Match_CaloTT_TT_LArHEC_CP->Fill(had_L1CaloE_CP, had_caloE);
          } else if (absEta < 4.9) {
            if (had_L1CaloE_CP >= 127) {
              m_h_hadEnergy_Calo_L1CaloSat_LArFCAL23_CP->Fill(had_caloE);
            } else {
             if (had_caloE != 0) m_h_had_1D_Match_LArFCAL23_CP->Fill(hadRelDiffE_CP);
            }
            m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23_CP->Fill(had_L1CaloE_CP, had_caloE);
          }
        }
      }

      if (had_maximum < had_L1CaloE) {
        had_maximum = had_L1CaloE;
        eta_max_had = eta;
        phi_max_had = phi;
      }

      if (had_maximum_CP < had_L1CaloE_CP) {
        had_maximum_CP = had_L1CaloE_CP;
        eta_max_had_CP = eta;
        phi_max_had_CP = phi;
      }
 
      if (calohad_maximum < had_caloE) {
        calohad_maximum = had_caloE; 
        eta_max_calohad = eta;
        phi_max_calohad = phi;
      }
     
      if (had_caloE > 0) {
        m_h_CaloTT_hadLUT_eta-> Fill(eta);
        m_histTool->fillPPMPhi(m_h_CaloTT_hadLUT_phi, eta, phi);
        m_h_CaloTT_hadLUT->Fill(had_caloE);
      }
	  
      if (had_caloE > m_CaloTT_HitMap_Thresh0) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_CaloTT_HitMap_hadLUT_Thresh0, eta, phi);
      }
      if (had_caloE > m_CaloTT_HitMap_Thresh1) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_CaloTT_HitMap_hadLUT_Thresh1, eta, phi);
      }
      if (had_caloE > m_CaloTT_HitMap_Thresh2) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_CaloTT_HitMap_hadLUT_Thresh2, eta, phi);
      }

    } // End of HAD layer
 
  }  // end of loop over ttIterator 
      
  if (eta_max_caloem != -10) {
    m_histTool->fillPPMEmEtaVsPhi(m_h_emcaloME_etaphi, eta_max_caloem,
                                                       phi_max_caloem);
  }
  if (eta_max_calohad != -10) {
    m_histTool->fillPPMHadEtaVsPhi(m_h_hadcaloME_etaphi, eta_max_calohad,
                                                         phi_max_calohad);
  }
  if (eta_max_em != -10) {
    m_histTool->fillPPMEmEtaVsPhi(m_h_emTTME_etaphi, eta_max_em, phi_max_em);
  }
  if (eta_max_had != -10) {
    m_histTool->fillPPMHadEtaVsPhi(m_h_hadTTME_etaphi, eta_max_had, phi_max_had);
  }
  if (eta_max_em_CP != -10) {
    m_histTool->fillPPMEmEtaVsPhi(m_h_emTTME_etaphi_CP, eta_max_em_CP, phi_max_em_CP);
  }
  if (eta_max_had_CP != -10) {
    m_histTool->fillPPMHadEtaVsPhi(m_h_hadTTME_etaphi_CP, eta_max_had_CP, phi_max_had_CP);
  }

  return StatusCode::SUCCESS;     
}
/*---------------------------------------------------------*/
StatusCode CalorimeterL1CaloMon::procHistograms()
/*---------------------------------------------------------*/
{
  if ( endOfLumiBlock ) { }
	
  if ( endOfRun ) { }
  
  return StatusCode::SUCCESS;
}

// Is cell used in TT - this is not exact

bool CalorimeterL1CaloMon::usedInTT(const CaloDetDescrElement* caloDDE)
{
  CaloCell_ID::CaloSample sampling = caloDDE->getSampling();
  bool result = ( sampling == CaloCell_ID::PreSamplerB ||
                  sampling == CaloCell_ID::EMB1        ||
		  sampling == CaloCell_ID::EMB2        ||
		  sampling == CaloCell_ID::EMB3        ||
		  sampling == CaloCell_ID::PreSamplerE ||
		  sampling == CaloCell_ID::EME1        ||
		  sampling == CaloCell_ID::EME2        ||
		  sampling == CaloCell_ID::EME3        ||
		  sampling == CaloCell_ID::HEC0        ||
		  sampling == CaloCell_ID::HEC1        ||
		  sampling == CaloCell_ID::HEC2        ||
		  sampling == CaloCell_ID::TileBar0    ||
		  sampling == CaloCell_ID::TileBar1    ||
		  sampling == CaloCell_ID::TileBar2    ||
		  sampling == CaloCell_ID::TileGap1    ||
		  sampling == CaloCell_ID::TileGap2    ||
		  sampling == CaloCell_ID::TileExt0    ||
		  sampling == CaloCell_ID::TileExt1    ||
		  sampling == CaloCell_ID::TileExt2    ||
		  sampling == CaloCell_ID::FCAL0       ||
		  sampling == CaloCell_ID::FCAL1       ||
		  sampling == CaloCell_ID::FCAL2 );
  return result;
}
// ============================================================================
}  // end namespace
// ============================================================================
