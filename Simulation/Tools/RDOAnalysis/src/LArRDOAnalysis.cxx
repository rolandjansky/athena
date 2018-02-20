/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

LArRDOAnalysis::LArRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputRawChannelKey("LArRawChannels")
  , m_inputTTL1HADKey("LArTTL1HAD")
  , m_inputTTL1EMKey("LArTTL1EM")
  , m_inputDigitKey("LArDigitContainer_MC_Thinned")
  , m_larID(0)
  , m_energy(0)
  , m_time(0)
  , m_qual(0)
  , m_prov(0)
  , m_gain(0)
  , m_hadOnID(0)
  , m_hadOffID(0)
  , m_hadSamples(0)
  , m_emOnID(0)
  , m_emOffID(0)
  , m_emSamples(0)
  , m_digiID(0)
  , m_digiGain(0)
  , m_digiSamples(0)

  , m_h_larID(0)
  , m_h_energy(0)
  , m_h_time(0)
  , m_h_qual(0)
  , m_h_prov(0)
  , m_h_gain(0)
  , m_h_hadOnID(0)
  , m_h_hadOffID(0)
  , m_h_hadSamples(0)
  , m_h_emOnID(0)
  , m_h_emOffID(0)
  , m_h_emSamples(0)
  , m_h_digiID(0)
  , m_h_digiGain(0)
  , m_h_digiSamples(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/LArRDOAnalysis/")
  , m_ntupleTreeName("LArRDOAna")
  , m_path("/LArRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputRawChannelKey", m_inputRawChannelKey);
  declareProperty("InputTTL1HADKey", m_inputTTL1HADKey);
  declareProperty("InputTTL1EMKey", m_inputTTL1EMKey);
  declareProperty("InputDigitKey", m_inputDigitKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode LArRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing LArRDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputRawChannelKey.initialize() );
  ATH_CHECK( m_inputTTL1HADKey.initialize() );
  ATH_CHECK( m_inputTTL1EMKey.initialize() );
  ATH_CHECK( m_inputDigitKey.initialize() );

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "LArRDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    m_tree->Branch("larID", &m_larID);
    m_tree->Branch("energy", &m_energy);
    m_tree->Branch("time", &m_time);
    m_tree->Branch("qual", &m_qual);
    m_tree->Branch("prov", &m_prov);
    m_tree->Branch("gain", &m_gain);
    m_tree->Branch("hadOnID", &m_hadOnID);
    m_tree->Branch("hadOffID", &m_hadOffID);
    m_tree->Branch("hadSamples", &m_hadSamples);
    m_tree->Branch("emOnID", &m_emOnID);
    m_tree->Branch("emOffID", &m_emOffID);
    m_tree->Branch("emSamples", &m_emSamples);
    m_tree->Branch("digiID", &m_digiID);
    m_tree->Branch("digiGain", &m_digiGain);
    m_tree->Branch("digiSamples", &m_digiSamples);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  m_h_larID = new TH1F("h_larID", "LAr ID", 100, 0, 5e18);
  m_h_larID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_larID->GetName(), m_h_larID));

  m_h_energy = new TH1F("h_energy", "LAr energy", 100, -1e5, 5e5);
  m_h_energy->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_energy->GetName(), m_h_energy));

  m_h_time = new TH1F("h_time", "LAr time", 100, -1e7, 1e7);
  m_h_time->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_time->GetName(), m_h_time));

  m_h_qual = new TH1F("h_qual", "LAr quality", 100, 0, 70000);
  m_h_qual->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_qual->GetName(), m_h_qual));

  m_h_prov = new TH1F("h_prov", "LAr provenance", 100, 0, 9000);
  m_h_prov->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_prov->GetName(), m_h_prov));

  m_h_gain = new TH1F("h_gain", "LAr gain", 100, 0, 5);
  m_h_gain->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_gain->GetName(), m_h_gain));

  m_h_hadOnID = new TH1F("h_hadOnID", "Had LAr TTL1 online ID", 100, 0, 3e19);
  m_h_hadOnID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_hadOnID->GetName(), m_h_hadOnID));

  m_h_hadOffID = new TH1F("h_hadOffID", "Had LAr TTL1 offline ID", 100, 0, 3e19);
  m_h_hadOffID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_hadOffID->GetName(), m_h_hadOffID));

  m_h_hadSamples = new TH1F("h_hadSamples", "Had LAr TTL1 sample values", 100, -15000, 35000);
  m_h_hadSamples->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_hadSamples->GetName(), m_h_hadSamples));

  m_h_emOnID = new TH1F("h_emOnID", "EM LAr TTL1 online ID", 100, 0, 3e19);
  m_h_emOnID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_emOnID->GetName(), m_h_emOnID));

  m_h_emOffID = new TH1F("h_emOffID", "EM LAr TTL1 offline ID", 100, 0, 3e19);
  m_h_emOffID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_emOffID->GetName(), m_h_emOffID));

  m_h_emSamples = new TH1F("h_emSamples", "EM LAr TTL1 sample values", 100, -1e5, 3.5e5);
  m_h_emSamples->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_emSamples->GetName(), m_h_emSamples));

  m_h_digiID = new TH1F("h_digiID", "LAr digit ID", 100, 0, 5e18);
  m_h_digiID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_digiID->GetName(), m_h_digiID));

  m_h_digiGain = new TH1F("h_digiGain", "LAr digit gain", 100, 0, 5);
  m_h_digiGain->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_digiGain->GetName(), m_h_digiGain));

  m_h_digiSamples = new TH1F("h_digiSamples", "LAr digit sample values", 100, 0, 5000);
  m_h_digiSamples->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_digiSamples->GetName(), m_h_digiSamples));

  return StatusCode::SUCCESS;
}

StatusCode LArRDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In LArRDOAnalysis::execute()" );

  m_larID->clear();
  m_energy->clear();
  m_time->clear();
  m_qual->clear();
  m_prov->clear();
  m_gain->clear();
  m_hadOnID->clear();
  m_hadOffID->clear();
  m_hadSamples->clear();
  m_emOnID->clear();
  m_emOffID->clear();
  m_emSamples->clear();
  m_digiID->clear();
  m_digiGain->clear();
  m_digiSamples->clear();

  // LAr Raw Channels
  SG::ReadHandle<LArRawChannelContainer> p_larRawCont(m_inputRawChannelKey);
  if (p_larRawCont.isValid()) {
    // loop over LAr raw channels container
    LArRawChannelContainer::const_iterator lar_itr(p_larRawCont->begin());
    const LArRawChannelContainer::const_iterator lar_end(p_larRawCont->end());
    for ( ; lar_itr != lar_end; ++lar_itr ) {
      const HWIdentifier larID(lar_itr->identify());
      const int rawEnergy(lar_itr->energy());
      const int rawTime(lar_itr->time());
      const uint16_t rawQual(lar_itr->quality());
      const uint16_t rawProv(lar_itr->provenance());
      CaloGain::CaloGain larGain(lar_itr->gain());

      const unsigned long long larID_int = larID.get_compact();
      const int larGain_int = (int)larGain;
      m_larID->push_back(larID_int);
      m_energy->push_back(rawEnergy);
      m_time->push_back(rawTime);
      m_qual->push_back(rawQual);
      m_prov->push_back(rawProv);
      m_gain->push_back(larGain_int);

      m_h_larID->Fill(larID_int);
      m_h_energy->Fill(rawEnergy);
      m_h_time->Fill(rawTime);
      m_h_qual->Fill(rawQual);
      m_h_prov->Fill(rawProv);
      m_h_gain->Fill(larGain_int);
    }
  }


  // LAr TTL1 - Had
  SG::ReadHandle<LArTTL1Container> p_larTTL1Cont_had(m_inputTTL1HADKey);
  if (p_larTTL1Cont_had.isValid()) {
    LArTTL1Container::const_iterator ttl1Had_itr(p_larTTL1Cont_had->begin());
    const LArTTL1Container::const_iterator ttl1Had_end(p_larTTL1Cont_had->end());
    for ( ; ttl1Had_itr != ttl1Had_end; ++ttl1Had_itr ) {
      const HWIdentifier& hadOnID((*ttl1Had_itr)->ttOnlineID());
      const Identifier& hadOffID((*ttl1Had_itr)->ttOfflineID());
      const std::vector<float>& hadSamples = (*ttl1Had_itr)->samples();

      const unsigned long long hadOnID_int = hadOnID.get_compact();
      const unsigned long long hadOffID_int = hadOffID.get_compact();
      m_hadOnID->push_back(hadOnID_int);
      m_hadOffID->push_back(hadOffID_int);
      for (std::vector<float>::size_type i = 0; i != hadSamples.size(); ++i) {
        m_hadSamples->push_back(hadSamples.at(i));
        m_h_hadSamples->Fill(hadSamples.at(i));
      }

      m_h_hadOnID->Fill(hadOnID_int);
      m_h_hadOffID->Fill(hadOffID_int);
    }
  }

  // LAr TTL1 - EM
  SG::ReadHandle<LArTTL1Container> p_larTTL1Cont_em(m_inputTTL1EMKey);
  if (p_larTTL1Cont_em.isValid()) {
    LArTTL1Container::const_iterator ttl1EM_itr(p_larTTL1Cont_em->begin());
    const LArTTL1Container::const_iterator ttl1EM_end(p_larTTL1Cont_em->end());
    for ( ; ttl1EM_itr != ttl1EM_end; ++ttl1EM_itr ) {
      const HWIdentifier& emOnID((*ttl1EM_itr)->ttOnlineID());
      const Identifier& emOffID((*ttl1EM_itr)->ttOfflineID());
      const std::vector<float>& emSamples((*ttl1EM_itr)->samples());

      const unsigned long long emOnID_int = emOnID.get_compact();
      const unsigned long long emOffID_int = emOffID.get_compact();
      m_emOnID->push_back(emOnID_int);
      m_emOffID->push_back(emOffID_int);
      for (std::vector<float>::size_type j = 0; j != emSamples.size(); ++j) {
        m_emSamples->push_back(emSamples.at(j));
        m_h_emSamples->Fill(emSamples.at(j));
      }

      m_h_emOnID->Fill(emOnID_int);
      m_h_emOffID->Fill(emOffID_int);
    }
  }


  // LAr Digits
  SG::ReadHandle<LArDigitContainer> p_larDigiCont(m_inputDigitKey);
  if (p_larDigiCont.isValid()) {
    LArDigitContainer::const_iterator digi_itr(p_larDigiCont->begin());
    const LArDigitContainer::const_iterator digi_end(p_larDigiCont->end());
    for ( ; digi_itr != digi_end; ++digi_itr ) {
      const HWIdentifier& digiID((*digi_itr)->hardwareID());
      CaloGain::CaloGain digiGain((*digi_itr)->gain());
      const std::vector<short>& digiSamples = (*digi_itr)->samples();

      const unsigned long long digiID_int = digiID.get_compact();
      const int digiGain_int = (int)digiGain;
      m_digiID->push_back(digiID_int);
      m_digiGain->push_back(digiGain_int);
      for (std::vector<short>::size_type k = 0; k != digiSamples.size(); ++k) {
        m_digiSamples->push_back(digiSamples.at(k));
        m_h_digiSamples->Fill(digiSamples.at(k));
      }

      m_h_digiID->Fill(digiID_int);
      m_h_digiGain->Fill(digiGain_int);
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode LArRDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
