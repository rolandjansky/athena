/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "AnalysisTriggerEventTPCnv/LVL1_ROICnv_p1.h"

LVL1_ROICnv_p1::LVL1_ROICnv_p1()
  : T_AthenaPoolTPCnvBase< LVL1_ROI, LVL1_ROI_p1 >() {

}

void LVL1_ROICnv_p1::persToTrans( const LVL1_ROI_p1* persObj, LVL1_ROI* transObj, MsgStream &log ) {

  log << MSG::DEBUG << "Converting LVL1_ROI from persistent state..." << endreq;

  transObj->clear();

  //
  // Translate the muon RoIs:
  //
  for( std::vector< Muon_ROI_p1 >::const_iterator it = persObj->m_muonROIs.begin(); it != persObj->m_muonROIs.end(); ++it ) {
    Muon_ROI roi( it->m_roiWord, it->m_eta, it->m_phi, it->m_thrName, it->m_thrValue );
    transObj->addMuonROI( roi );
  }
  //
  // Translate the jet RoIs:
  //
  for( std::vector< Jet_ROI_p1 >::const_iterator it = persObj->m_jetROIs.begin(); it != persObj->m_jetROIs.end(); ++it ) {
    Jet_ROI roi( it->m_roiWord, it->m_eta, it->m_phi, ( it->m_roiWord & 0xfff ) );
    roi.setET4x4( it->m_ET4x4 );
    roi.setET6x6( it->m_ET6x6 );
    roi.setET8x8( it->m_ET8x8 );
    for( std::vector< std::string >::const_iterator it2 = it->m_thresholdNames.begin(); it2 != it->m_thresholdNames.end(); ++it2 ) {
      roi.addThresholdName( *it2 );
    }
    for( std::vector< Float_t >::const_iterator it2 = it->m_thresholdValues.begin(); it2 != it->m_thresholdValues.end(); ++it2 ) {
      roi.addThresholdValue( *it2 );
    }
    transObj->addJetROI( roi );
  }
  //
  // Translate the jetET RoI(s):
  //
  for( std::vector< JetET_ROI_p1 >::const_iterator it = persObj->m_jetetROIs.begin(); it != persObj->m_jetetROIs.end(); ++it ) {
    JetET_ROI roi( it->m_roiWord, ( it->m_roiWord & 0xf ) );
    for( std::vector< std::string >::const_iterator it2 = it->m_thresholds.begin(); it2 != it->m_thresholds.end(); ++it2 ) {
      roi.addThreshold( *it2 );
    }
    transObj->addJetETROI( roi );
  }
  //
  // Translate the em/tau RoIs:
  //
  for( std::vector< EmTau_ROI_p1 >::const_iterator it = persObj->m_emtauROIs.begin(); it != persObj->m_emtauROIs.end(); ++it ) {
    EmTau_ROI roi( it->m_roiWord, it->m_eta, it->m_phi, ( it->m_roiWord & 0xffff ) );
    roi.setCore( it->m_Core );
    roi.setEMClus( it->m_EMClus );
    roi.setTauClus( it->m_TauClus );
    roi.setEMIsol( it->m_EMIsol );
    roi.setHadIsol( it->m_HadIsol );
    roi.setHadCore( it->m_HadCore );
    for( std::vector< std::string >::const_iterator it2 = it->m_thresholdNames.begin(); it2 != it->m_thresholdNames.end(); ++it2 ) {
      roi.addThresholdName( *it2 );
    }
    for( std::vector< Float_t >::const_iterator it2 = it->m_thresholdValues.begin(); it2 != it->m_thresholdValues.end(); ++it2 ) {
      roi.addThresholdValue( *it2 );
    }
    transObj->addEmTauROI( roi );
  }
  //
  // Translate energy sum RoI(s):
  //
  // Used to test overflow bits (rather than a hard-coded check)
  LVL1::JEPRoIDecoder conv;
  
  for( std::vector< EnergySum_ROI_p1 >::const_iterator it = persObj->m_energysumROIs.begin(); it != persObj->m_energysumROIs.end(); ++it ) {
    // Extract overflow flags from RoIWords
    unsigned int overflows = 0;
    if (conv.energyOverflow(it->m_roiWord0)) overflows |= 0x1;
    if (conv.energyOverflow(it->m_roiWord1)) overflows |= 0x2;
    if (conv.energyOverflow(it->m_roiWord2)) overflows |= 0x4;
    // Create transient RoI
    EnergySum_ROI roi( it->m_roiWord0, it->m_roiWord1, it->m_roiWord2, it->m_energyX, it->m_energyY, it->m_energyT, 
                       ( ( it->m_roiWord1 >> 16 ) & 0xff), ( ( it->m_roiWord2 >> 16 ) & 0xff),  ( ( it->m_roiWord0 >> 16 ) & 0xff),
                         overflows);
    for( std::vector< std::string >::const_iterator it2 = it->m_thresholds.begin(); it2 != it->m_thresholds.end(); ++it2 ) {
      roi.addThreshold( *it2 );
    }
    transObj->addEnergySumROI( roi );
  }

  log << MSG::DEBUG << "Converting LVL1_ROI from persistent state [OK]" << endreq;

  return;

}

void LVL1_ROICnv_p1::transToPers( const LVL1_ROI* transObj, LVL1_ROI_p1* persObj, MsgStream &log ) {

  log << MSG::DEBUG << "Creating persistent state of LVL1_ROI..." << endreq;

  //
  // Translate the muon RoIs:
  //
  for( LVL1_ROI::muons_type::const_iterator it = transObj->getMuonROIs().begin(); it != transObj->getMuonROIs().end(); ++it ) {
    Muon_ROI_p1 roi;
    roi.m_roiWord  = it->getROIWord();
    roi.m_eta      = it->getEta();
    roi.m_phi      = it->getPhi();
    roi.m_thrName  = it->getThrName();
    roi.m_thrValue = it->getThrValue();
    persObj->m_muonROIs.push_back( roi );
  }
  //
  // Translate the jet RoIs:
  //
  for( LVL1_ROI::jets_type::const_iterator it = transObj->getJetROIs().begin(); it != transObj->getJetROIs().end(); ++it ) {
    Jet_ROI_p1 roi;
    roi.m_roiWord = it->getROIWord();
    roi.m_eta     = it->getEta();
    roi.m_phi     = it->getPhi();
    roi.m_ET4x4   = it->getET4x4();
    roi.m_ET6x6   = it->getET6x6();
    roi.m_ET8x8   = it->getET8x8();
    for( std::vector< std::string >::const_iterator it2 = it->getThresholdNames().begin(); it2 != it->getThresholdNames().end(); ++it2 ) {
      roi.m_thresholdNames.push_back( *it2 );
    }
    for( std::vector< float >::const_iterator it2 = it->getThresholdValues().begin(); it2 != it->getThresholdValues().end(); ++it2 ) {
      roi.m_thresholdValues.push_back( *it2 );
    }
    persObj->m_jetROIs.push_back( roi );
  }
  //
  // Translate the jetET RoI(s):
  //
  for( LVL1_ROI::jetets_type::const_iterator it = transObj->getJetEtROIs().begin(); it != transObj->getJetEtROIs().end(); ++it ) {
    JetET_ROI_p1 roi;
    roi.m_roiWord = it->getROIWord();
    for( std::vector< std::string >::const_iterator it2 = it->getThresholds().begin(); it2 != it->getThresholds().end(); ++it2 ) {
      roi.m_thresholds.push_back( *it2 );
    }
    persObj->m_jetetROIs.push_back( roi );
  }
  //
  // Translate em/tau RoI(s):
  //
  for( LVL1_ROI::emtaus_type::const_iterator it = transObj->getEmTauROIs().begin(); it != transObj->getEmTauROIs().end(); ++it ) {
    EmTau_ROI_p1 roi;
    roi.m_roiWord = it->getROIWord();
    roi.m_eta     = it->getEta();
    roi.m_phi     = it->getPhi();
    roi.m_Core    = it->getCore();
    roi.m_EMClus  = it->getEMClus();
    roi.m_EMIsol  = it->getEMIsol();
    roi.m_HadCore = it->getHadCore();
    roi.m_HadIsol = it->getHadIsol();
    roi.m_TauClus = it->getTauClus();
    for( std::vector< std::string >::const_iterator it2 = it->getThresholdNames().begin(); it2 != it->getThresholdNames().end(); ++it2 ) {
      roi.m_thresholdNames.push_back( *it2 );
    }
    for( std::vector< float >::const_iterator it2 = it->getThresholdValues().begin(); it2 != it->getThresholdValues().end(); ++it2 ) {
      roi.m_thresholdValues.push_back( *it2 );
    }
    persObj->m_emtauROIs.push_back( roi );
  }
  //
  // Translate energy sum RoI(s):
  //
  for( LVL1_ROI::energysums_type::const_iterator it = transObj->getEnergySumROIs().begin(); it != transObj->getEnergySumROIs().end(); ++it ) {
    EnergySum_ROI_p1 roi;
    roi.m_roiWord0 = it->getROIWord0();
    roi.m_roiWord1 = it->getROIWord1();
    roi.m_roiWord2 = it->getROIWord2();
    roi.m_energyX  = it->getEnergyX();
    roi.m_energyY  = it->getEnergyY();
    roi.m_energyT  = it->getEnergyT();
    for( std::vector< std::string >::const_iterator it2 = it->getThresholds().begin(); it2 != it->getThresholds().end(); ++it2 ) {
      roi.m_thresholds.push_back( *it2 );
    }
    persObj->m_energysumROIs.push_back( roi );
  }

  log << MSG::DEBUG << "Creating persistent state of LVL1_ROI [OK]" << endreq;

  return;

}
