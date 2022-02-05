/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXNtupleWriter.cxx  - copied from eFEXNtupleWrite by Tong Qiu
//                              -------------------
//     begin                : 12 12 2020
//     email                : varsiha.sothilingam@cern.ch
//  **************************************************************************

#include <L1CaloFEXSim/jFEXNtupleWriter.h>
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/jFEXSmallRJetAlgo.h"
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"
//#include "L1CaloFEXSim/jFEXtauAlgo.h"
//#include "L1CaloFEXSim/jFEXtauTOB.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"
#include <vector>
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <memory>
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODJet/JetContainer.h"


LVL1::jFEXNtupleWriter::jFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator) { }

LVL1::jFEXNtupleWriter::~jFEXNtupleWriter() {

}

StatusCode LVL1::jFEXNtupleWriter::initialize () {
    ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
    CHECK( histSvc.retrieve() );
    m_myTree = new TTree("data","data");
    CHECK( histSvc->regTree("/ANALYSIS/data",m_myTree) );
    ATH_CHECK( m_jFEXOutputCollectionSGKey.initialize() );

    m_myTree->Branch ("smallRJet_eta",  &m_smallRJet_eta);
    m_myTree->Branch ("smallRJet_phi",  &m_smallRJet_phi);
    m_myTree->Branch ("smallRJet_ET",  &m_smallRJet_ET);
    m_myTree->Branch ("smallRJet_clusterET", &m_smallRJet_clusterET);
    m_myTree->Branch ("smallRJet_nTOBs",  &m_smallRJet_nTOBs, "nTOBs");
    m_myTree->Branch ("smallRJet_isCentralTowerSeed",  &m_smallRJet_isCentralTowerSeed);

    m_myTree->Branch ("smallRJetTOB_eta", &m_smallRJetTOB_eta);
    m_myTree->Branch ("smallRJetTOB_phi", &m_smallRJetTOB_phi);
    m_myTree->Branch ("smallRJetTOB_ET", &m_smallRJetTOB_ET);
    m_myTree->Branch ("smallRJetTOB_sat", &m_smallRJetTOB_sat);
    m_myTree->Branch ("smallRJetTOB_word", &m_smallRJetTOB_word);
    m_myTree->Branch ("smallRJetTOB_jfexID", &m_smallRJetTOB_jfexID);
    m_myTree->Branch ("smallRJetTOB_fpgaID", &m_smallRJetTOB_fpgaID);


    m_myTree->Branch ("largeRJet_eta", &m_largeRJet_eta);
    m_myTree->Branch ("largeRJet_phi", &m_largeRJet_phi);
    m_myTree->Branch ("largeRJet_ET",  &m_largeRJet_ET);
    m_myTree->Branch ("largeRJet_nTOBs",  &m_largeRJet_nTOBs);

    m_myTree->Branch ("largeRJetTOB_ET",  &m_largeRJetTOB_ET);
    m_myTree->Branch ("largeRJetTOB_eta", &m_largeRJetTOB_eta);
    m_myTree->Branch ("largeRJetTOB_phi", &m_largeRJetTOB_phi);
    m_myTree->Branch ("largeRJetTOB_sat", &m_largeRJetTOB_sat);

    m_myTree->Branch ("largeRJetTOB_word",   &m_largeRJetTOB_word);
    m_myTree->Branch ("largeRJetTOB_jfexID", &m_largeRJetTOB_jfexID);
    m_myTree->Branch ("largeRJetTOB_fpgaID", &m_largeRJetTOB_fpgaID);

    m_myTree->Branch ("tau_TT_ID",  &m_tau_TT_ID);
    m_myTree->Branch ("tau_isLocalMax",  &m_tau_isLocalMax);
    m_myTree->Branch ("tau_ET",  &m_tau_ET);
    m_myTree->Branch ("tau_clusterET",  &m_tau_clusterET);
    m_myTree->Branch ("tau_eta",  &m_tau_eta);
    m_myTree->Branch ("tau_phi",  &m_tau_phi);
    m_myTree->Branch ("tau_realeta",  &m_tau_realeta);
    m_myTree->Branch ("tau_ISO",  &m_tau_ISO);
    m_myTree->Branch ("tau_jFEXid",  &m_tau_jFEXid);
    m_myTree->Branch ("tau_FPGAid",  &m_tau_FPGAid);

    m_myTree->Branch ("tau_TOB_word",  &m_tau_TOB_word);
    m_myTree->Branch ("tau_TOB_ET",  &m_tau_TOB_ET);
    m_myTree->Branch ("tau_TOB_eta",  &m_tau_TOB_eta);
    m_myTree->Branch ("tau_TOB_phi",  &m_tau_TOB_phi);
    m_myTree->Branch ("tau_TOB_ISO",  &m_tau_TOB_ISO);
    m_myTree->Branch ("tau_TOB_Sat",  &m_tau_TOB_Sat);

    //Pileup
    m_myTree->Branch ("pileup_FPGAid",    &m_pileup_FPGAid);
    m_myTree->Branch ("pileup_jFEXid",    &m_pileup_jFEXid);
    m_myTree->Branch ("pileup_rho_EM",    &m_pileup_rho_EM);
    m_myTree->Branch ("pileup_rho_HAD1",  &m_pileup_rho_HAD1);
    m_myTree->Branch ("pileup_rho_HAD2",  &m_pileup_rho_HAD2);
    m_myTree->Branch ("pileup_rho_HAD3",  &m_pileup_rho_HAD3);
    m_myTree->Branch ("pileup_rho_FCAL",  &m_pileup_rho_FCAL);

    m_myTree->Branch ("pileup_map_ID",  &m_pileup_map_ID);
    m_myTree->Branch ("pileup_map_Et_values_HAD_jet",  &m_pileup_map_Et_values_HAD_jet);
    m_myTree->Branch ("pileup_map_Et_values_EM_jet",  &m_pileup_map_Et_values_EM_jet);
    m_myTree->Branch ("pileup_map_Et_values_Total_jet",  &m_pileup_map_Et_values_Total_jet);
    m_myTree->Branch ("pileup_map_Et_values_HAD_met",  &m_pileup_map_Et_values_HAD_met);
    m_myTree->Branch ("pileup_map_Et_values_EM_met",  &m_pileup_map_Et_values_EM_met);
    m_myTree->Branch ("pileup_map_Et_values_Total_met",  &m_pileup_map_Et_values_Total_met);

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::execute () {
    SG::ReadHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionobj = SG::ReadHandle<LVL1::jFEXOutputCollection>(m_jFEXOutputCollectionSGKey/*,ctx*/);
    if(!jFEXOutputCollectionobj.isValid()) {
        ATH_MSG_FATAL("Could not retrieve jFEXOutputCollection " << m_jFEXOutputCollectionSGKey.key());
        return StatusCode::FAILURE;
    }
    if (!jFEXOutputCollectionobj->getdooutput()) {
        return StatusCode::SUCCESS;
    }
    CHECK(loadsmallRJetAlgoVariables(jFEXOutputCollectionobj));
    CHECK(loadlargeRJetAlgoVariables(jFEXOutputCollectionobj));
    CHECK(loadtauAlgoVariables(jFEXOutputCollectionobj));
    CHECK(loadPileupVariables(jFEXOutputCollectionobj));
    CHECK(loadPileupEt(jFEXOutputCollectionobj));
    m_myTree->Fill();

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::finalize () {
    ATH_MSG_DEBUG("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadsmallRJetAlgoVariables(SG::ReadHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionobj) {

    m_smallRJet_eta.clear();
    m_smallRJet_phi.clear();
    m_smallRJet_isCentralTowerSeed.clear();
    m_smallRJet_ET.clear();
    m_smallRJet_clusterET.clear();
    m_smallRJetTOB_eta.clear();
    m_smallRJetTOB_phi.clear();
    m_smallRJetTOB_ET.clear();
    m_smallRJetTOB_sat.clear();
    m_smallRJetTOB_word.clear();
    m_smallRJetTOB_jfexID.clear();
    m_smallRJetTOB_fpgaID.clear();

    for (int i = 0; i < jFEXOutputCollectionobj->SRJetSize(); i++)
    {
        m_smallRJet_isCentralTowerSeed.push_back(   jFEXOutputCollectionobj->get_smallRJet(i,"smallRJet_isCentralTowerSeed"));
        m_smallRJet_phi.push_back(                  jFEXOutputCollectionobj->get_smallRJet(i,"smallRJet_phi"));
        m_smallRJet_eta.push_back(                  jFEXOutputCollectionobj->get_smallRJet(i,"smallRJet_eta"));
        m_smallRJet_ET.push_back(                   jFEXOutputCollectionobj->get_smallRJet(i,"smallRJet_ET"));
        m_smallRJet_clusterET.push_back(            jFEXOutputCollectionobj->get_smallRJet(i,"smallRJet_clusterET"));
        m_smallRJetTOB_eta.push_back(               jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_eta"));
        m_smallRJetTOB_phi.push_back(               jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_phi"));
        m_smallRJetTOB_ET.push_back(                jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_ET"));
        m_smallRJetTOB_sat.push_back(               jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_sat"));
        m_smallRJetTOB_word.push_back(              jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_word"));
        m_smallRJetTOB_jfexID.push_back(            jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_jfexID"));
        m_smallRJetTOB_fpgaID.push_back(            jFEXOutputCollectionobj->get_smallRJet(i,"smallRJetTOB_fpgaID"));
    }
    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadlargeRJetAlgoVariables(SG::ReadHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionobj) {

    m_largeRJet_ET.clear();
    m_largeRJet_nTOBs.clear();
    m_largeRJet_eta.clear();
    m_largeRJet_phi.clear();
    m_largeRJetTOB_ET.clear();
    m_largeRJetTOB_eta.clear();
    m_largeRJetTOB_phi.clear();
    m_largeRJetTOB_sat.clear();
    m_largeRJetTOB_word.clear();
    m_largeRJetTOB_fpgaID.clear();
    m_largeRJetTOB_jfexID.clear();

    for (int i = 0; i < jFEXOutputCollectionobj->LRJetSize(); i++)
    {
        m_largeRJet_ET.push_back(       jFEXOutputCollectionobj->get_largeRJet(i,"largeRJet_ET"));
        m_largeRJet_nTOBs.push_back(    jFEXOutputCollectionobj->get_largeRJet(i,"largeRJet_nTOBs"));
        m_largeRJet_eta.push_back(      jFEXOutputCollectionobj->get_largeRJet(i,"largeRJet_eta"));
        m_largeRJet_phi.push_back(      jFEXOutputCollectionobj->get_largeRJet(i,"largeRJet_phi"));
        m_largeRJetTOB_ET.push_back(    jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_ET"));
        m_largeRJetTOB_eta.push_back(   jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_eta"));
        m_largeRJetTOB_phi.push_back(   jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_phi"));
        m_largeRJetTOB_sat.push_back(   jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_sat"));
        m_largeRJetTOB_word.push_back(  jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_word"));
        m_largeRJetTOB_fpgaID.push_back(jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_fpgaID"));
        m_largeRJetTOB_jfexID.push_back(jFEXOutputCollectionobj->get_largeRJet(i,"largeRJetTOB_jfexID"));

    }
    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadtauAlgoVariables(SG::ReadHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionobj) {

    m_tau_TT_ID.clear();
    m_tau_jFEXid.clear();
    m_tau_FPGAid.clear();
    m_tau_isLocalMax.clear();
    m_tau_ET.clear();
    m_tau_clusterET.clear();
    m_tau_eta.clear();
    m_tau_phi.clear();
    m_tau_realeta.clear();
    m_tau_ISO.clear();

    m_tau_TOB_word.clear();
    m_tau_TOB_ET.clear();
    m_tau_TOB_eta.clear();
    m_tau_TOB_phi.clear();
    m_tau_TOB_ISO.clear();
    m_tau_TOB_Sat.clear();

    for (int i = 0; i < jFEXOutputCollectionobj->TauSize(); i++)
    {
        m_tau_isLocalMax.push_back( jFEXOutputCollectionobj->get_tau(i,"tau_isLocalMax"));
        m_tau_TT_ID.push_back(      jFEXOutputCollectionobj->get_tau(i,"tau_TT_ID"));
        m_tau_jFEXid.push_back(     jFEXOutputCollectionobj->get_tau(i,"tau_jFEXid"));
        m_tau_FPGAid.push_back(     jFEXOutputCollectionobj->get_tau(i,"tau_FPGAid"));
        m_tau_ET.push_back(         jFEXOutputCollectionobj->get_tau(i,"tau_ET"));
        m_tau_clusterET.push_back(  jFEXOutputCollectionobj->get_tau(i,"tau_clusterET"));
        m_tau_eta.push_back(        jFEXOutputCollectionobj->get_tau(i,"tau_eta"));
        m_tau_phi.push_back(        jFEXOutputCollectionobj->get_tau(i,"tau_phi"));
        m_tau_realeta.push_back(    jFEXOutputCollectionobj->get_tau(i,"tau_realeta"));
        m_tau_ISO.push_back(        jFEXOutputCollectionobj->get_tau(i,"tau_ISO"));

        m_tau_TOB_word.push_back(   jFEXOutputCollectionobj->get_tau(i,"tau_TOB_word"));
        m_tau_TOB_ET.push_back(     jFEXOutputCollectionobj->get_tau(i,"tau_TOB_ET"));
        m_tau_TOB_eta.push_back(    jFEXOutputCollectionobj->get_tau(i,"tau_TOB_eta"));
        m_tau_TOB_phi.push_back(    jFEXOutputCollectionobj->get_tau(i,"tau_TOB_phi"));
        m_tau_TOB_ISO.push_back(    jFEXOutputCollectionobj->get_tau(i,"tau_TOB_ISO"));
        m_tau_TOB_Sat.push_back(    jFEXOutputCollectionobj->get_tau(i,"tau_TOB_Sat"));

    }
    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadPileupVariables(SG::ReadHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionobj) {

    m_pileup_FPGAid.clear();
    m_pileup_jFEXid.clear();
    m_pileup_rho_EM.clear();
    m_pileup_rho_HAD1.clear();
    m_pileup_rho_HAD2.clear();
    m_pileup_rho_HAD3.clear();
    m_pileup_rho_FCAL.clear();

    for (int i = 0; i < jFEXOutputCollectionobj->PileupSize(); i++)
    {

        m_pileup_FPGAid.push_back(  jFEXOutputCollectionobj->get_pileup(i,"pileup_FPGAid"));
        m_pileup_jFEXid.push_back(  jFEXOutputCollectionobj->get_pileup(i,"pileup_jFEXid"));
        m_pileup_rho_EM.push_back(  jFEXOutputCollectionobj->get_pileup(i,"pileup_rho_EM"));
        m_pileup_rho_HAD1.push_back(jFEXOutputCollectionobj->get_pileup(i,"pileup_rho_HAD1"));
        m_pileup_rho_HAD2.push_back(jFEXOutputCollectionobj->get_pileup(i,"pileup_rho_HAD2"));
        m_pileup_rho_HAD3.push_back(jFEXOutputCollectionobj->get_pileup(i,"pileup_rho_HAD3"));
        m_pileup_rho_FCAL.push_back(jFEXOutputCollectionobj->get_pileup(i,"pileup_rho_FCAL"));
    }
    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXNtupleWriter::loadPileupEt(SG::ReadHandle<LVL1::jFEXOutputCollection> jFEXOutputCollectionobj) {

    m_pileup_map_ID.clear();
    m_pileup_map_Et_values_HAD_jet.clear();
    m_pileup_map_Et_values_EM_jet.clear();
    m_pileup_map_Et_values_Total_jet.clear();
    m_pileup_map_Et_values_HAD_met.clear();
    m_pileup_map_Et_values_EM_met.clear();
    m_pileup_map_Et_values_Total_met.clear();

    for (int i = 0; i < jFEXOutputCollectionobj->PileupEtSize(); i++)
    {
        m_pileup_map_ID.push_back( jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_ID"));
        m_pileup_map_Et_values_HAD_jet.push_back(  jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_Et_values_HAD_jet"));
        m_pileup_map_Et_values_EM_jet.push_back(   jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_Et_values_EM_jet"));
        m_pileup_map_Et_values_Total_jet.push_back(jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_Et_values_Total_jet"));
        m_pileup_map_Et_values_HAD_met.push_back(  jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_Et_values_HAD_met"));
        m_pileup_map_Et_values_EM_met.push_back(   jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_Et_values_EM_met"));
        m_pileup_map_Et_values_Total_met.push_back(jFEXOutputCollectionobj->get_pileup_map(i,"pileup_map_Et_values_Total_met"));

    }


    return StatusCode::SUCCESS;
}
