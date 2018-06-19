/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PadTriggerValidationTree.h"

#include "TrigT1NSWSimTools/PadTrigger.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"

using NSWL1::PadTriggerValidationTree;
using NSWL1::PadTrigger;

using std::string;

//------------------------------------------------------------------------------
PadTriggerValidationTree::PadTriggerValidationTree():
    m_tree(NULL),
    m_nPadTriggers(0),
    m_padTriggerBCID(NULL),
    m_padTriggerSectorID(NULL),
    m_padTriggerSideID(NULL),
    m_padTriggerBandID(NULL),
    m_padTriggerEta(NULL),
    m_padTriggerPhi(NULL),
    m_padTriggerEtaID(NULL),
    m_padTriggerPhiID(NULL),
    m_padTriggerMultipletID(NULL),
    m_padTriggerGasGapID(NULL)
    // m_nPadHits(0),
    // m_padGlobalX(NULL),
    // m_padGlobalY(NULL),
    // m_padGlobalZ(NULL),
    // m_padTruthHitGlobalX(NULL),
    // m_padTruthHitGlobalY(NULL),
    // m_padTruthHitGlobalZ(NULL),
    // m_padEtaIdFromOfflineId(NULL),
    // m_padPhiIdFromOfflineId(NULL),
    // m_padSectorFromOfflineId(NULL),
    // m_padLayerFromOfflineId(NULL),
    // m_offlineIdPadEtaConverted(NULL),
    // m_offlineIdPadPhiConverted(NULL),
    // m_padEtaIdFromOldSimu(NULL),
    // m_padPhiIdFromOldSimu(NULL)
{
}
//------------------------------------------------------------------------------
std::string PadTriggerValidationTree::treename_from_algoname(const std::string &algoname)
{
    string treename = algoname+"Tree";
    return treename;    
}
//------------------------------------------------------------------------------
bool PadTriggerValidationTree::init_tree(TTree *tree)
{
    bool success=false;
    if(tree){
        m_tree = tree;
        m_nPadTriggers       = 0;
        m_padTriggerBCID     = new std::vector<unsigned int>();
        m_padTriggerSectorID = new std::vector<int>();
        m_padTriggerSideID   = new std::vector<int>();
        m_padTriggerBandID   = new std::vector<unsigned int>();
        m_padTriggerEta      = new std::vector<float>();
        m_padTriggerPhi      = new std::vector<float>();
        m_padTriggerEtaID    = new std::vector<int>();
        m_padTriggerPhiID    = new std::vector<int>();
        m_padTriggerMultipletID = new std::vector<int>();
        m_padTriggerGasGapID    = new std::vector<int>();
        // m_nPadHits = 0;
        // m_padGlobalX = new std::vector<float>();
        // m_padGlobalY = new std::vector<float>();
        // m_padGlobalZ = new std::vector<float>();
        // m_padTruthHitGlobalX = new std::vector<float>();
        // m_padTruthHitGlobalY = new std::vector<float>();
        // m_padTruthHitGlobalZ = new std::vector<float>();
        // m_padEtaIdFromOfflineId = new std::vector<int>();
        // m_padPhiIdFromOfflineId = new std::vector<int>();
        // m_padSectorFromOfflineId = new std::vector<int>();
        // m_padLayerFromOfflineId = new std::vector<int>();
        // m_offlineIdPadEtaConverted = new std::vector<int>();
        // m_offlineIdPadPhiConverted = new std::vector<int>();
        // m_padEtaIdFromOldSimu   = new std::vector<int>();
        // m_padPhiIdFromOldSimu   = new std::vector<int>();

        m_tree->Branch("nPadTriggers",                 &m_nPadTriggers,"nPadTriggers/i");
        m_tree->Branch("padTriggerBCID",               &m_padTriggerBCID);
        m_tree->Branch("padTriggerSectorID",           &m_padTriggerSectorID);
        m_tree->Branch("padTriggerSideID",             &m_padTriggerSideID);
        m_tree->Branch("padTriggerBandID",             &m_padTriggerBandID);
        m_tree->Branch("padTriggerEta",                &m_padTriggerEta);
        m_tree->Branch("padTriggerPhi",                &m_padTriggerPhi);
        m_tree->Branch("padTriggerEtaID",              &m_padTriggerEtaID);
        m_tree->Branch("padTriggerPhiID",              &m_padTriggerPhiID);
        m_tree->Branch("padTriggerMultipletID",        &m_padTriggerMultipletID);
        m_tree->Branch("padTriggerGasGapID",           &m_padTriggerGasGapID);
        // m_tree->Branch("nPadHits",                  &m_nPadHits,"nPadHits/i");
        // m_tree->Branch("padGlobalX",                &m_padGlobalX);
        // m_tree->Branch("padGlobalY",                &m_padGlobalY);
        // m_tree->Branch("padGlobalZ",                &m_padGlobalZ);
        // m_tree->Branch("padTruthHitGlobalX",        &m_padTruthHitGlobalX);
        // m_tree->Branch("padTruthHitGlobalY",        &m_padTruthHitGlobalY);
        // m_tree->Branch("padTruthHitGlobalZ",        &m_padTruthHitGlobalZ);
        // m_tree->Branch("padEtaIdFromOfflineId",     &m_padEtaIdFromOfflineId);
        // m_tree->Branch("padPhiIdFromOfflineId",     &m_padPhiIdFromOfflineId);
        // m_tree->Branch("padSectorIdFromOfflineId",  &m_padSectorFromOfflineId);
        // m_tree->Branch("padLayerFromOfflineId",     &m_padLayerFromOfflineId);
        // m_tree->Branch("offlineIdPadEtaIdConverted",&m_offlineIdPadEtaConverted);
        // m_tree->Branch("offlineIdPadPhiIdConverted",&m_offlineIdPadPhiConverted);
        // m_tree->Branch("padEtaIdFromOldSimu",       &m_padEtaIdFromOldSimu);
        // m_tree->Branch("padPhiIdFromOldSimu",       &m_padPhiIdFromOldSimu);
        success = true;
    }
    return success;
}
//------------------------------------------------------------------------------
void PadTriggerValidationTree::reset_ntuple_variables()
{
    if(m_tree){
        m_nPadTriggers       = 0;
        m_padTriggerBCID     ->clear();
        m_padTriggerSectorID ->clear();
        m_padTriggerSideID   ->clear();
        m_padTriggerBandID   ->clear();
        m_padTriggerEta      ->clear();
        m_padTriggerPhi      ->clear();
        m_padTriggerEtaID    ->clear();
        m_padTriggerPhiID    ->clear();
        m_padTriggerMultipletID ->clear();
        m_padTriggerGasGapID    ->clear();
        // m_nPadHits = 0;
        // m_padGlobalX->clear();
        // m_padGlobalY->clear();
        // m_padGlobalZ->clear();
        // m_padTruthHitGlobalX->clear();
        // m_padTruthHitGlobalY->clear();
        // m_padTruthHitGlobalZ->clear();
        // m_padEtaIdFromOfflineId->clear();
        // m_padPhiIdFromOfflineId->clear();
        // m_padSectorFromOfflineId->clear();
        // m_padLayerFromOfflineId->clear();
        // m_offlineIdPadEtaConverted->clear();
        // m_offlineIdPadPhiConverted->clear();
        // m_padEtaIdFromOldSimu->clear();
        // m_padPhiIdFromOldSimu->clear();
    }
}
//------------------------------------------------------------------------------
void PadTriggerValidationTree::clear_ntuple_variables()
{
    m_nPadTriggers       = 0;
    m_padTriggerBCID     = NULL;
    m_padTriggerSectorID = NULL;
    m_padTriggerSideID   = NULL;
    m_padTriggerBandID   = NULL;
    m_padTriggerEta      = NULL;
    m_padTriggerPhi      = NULL;
    m_padTriggerEtaID    = NULL;
    m_padTriggerPhiID    = NULL;
    m_padTriggerMultipletID = NULL;
    m_padTriggerGasGapID    = NULL;
    // m_nPadHits = 0;
    // m_padGlobalX = NULL;
    // m_padGlobalY = NULL;
    // m_padGlobalZ = NULL;
    // m_padTruthHitGlobalX = NULL;
    // m_padTruthHitGlobalY = NULL;
    // m_padTruthHitGlobalZ = NULL;
    // m_padEtaIdFromOfflineId    = NULL;
    // m_padPhiIdFromOfflineId    = NULL;
    // m_padSectorFromOfflineId   = NULL;
    // m_padLayerFromOfflineId    = NULL;
    // m_offlineIdPadEtaConverted = NULL;
    // m_offlineIdPadPhiConverted = NULL;
    // m_padEtaIdFromOldSimu      = NULL;
    // m_padPhiIdFromOldSimu      = NULL;
}
//------------------------------------------------------------------------------
void PadTriggerValidationTree::fill_num_pad_triggers(size_t num)
{
    m_nPadTriggers = num;
}
//------------------------------------------------------------------------------
void PadTriggerValidationTree::fill_pad_trigger_basics(const std::vector<PadTrigger*> &triggers) {
  for(auto& trigger : triggers) {
    m_padTriggerBCID       ->push_back(trigger->bctag());
    m_padTriggerSectorID   ->push_back(trigger->sectorId());
    m_padTriggerSideID     ->push_back(trigger->sideId()); 
    m_padTriggerBandID     ->push_back(trigger->bandId()); 
    m_padTriggerEta        ->push_back(trigger->eta()); 
    m_padTriggerPhi        ->push_back(trigger->phi()); 
    m_padTriggerEtaID      ->push_back(trigger->etaId()); 
    m_padTriggerPhiID      ->push_back(trigger->phiId()); 
    m_padTriggerMultipletID->push_back(trigger->multipletId()); 
    m_padTriggerGasGapID   ->push_back(trigger->gasGapId()); 
  }
}

// //------------------------------------------------------------------------------
// void PadTriggerValidationTree::fill_num_pad_hits(size_t num)
// {
//     m_nPadHits = num;
// }
// //------------------------------------------------------------------------------
// void PadTriggerValidationTree::fill_hit_global_pos(const Amg::Vector3D &pos)
// {
//     m_padGlobalX->push_back(pos.x());
//     m_padGlobalY->push_back(pos.y());
//     m_padGlobalZ->push_back(pos.z());
// }
// //------------------------------------------------------------------------------
// void PadTriggerValidationTree::fill_truth_hit_global_pos(const Amg::Vector3D &pos)
// {
//     m_padTruthHitGlobalX->push_back(pos.x());
//     m_padTruthHitGlobalY->push_back(pos.y());
//     m_padTruthHitGlobalZ->push_back(pos.z());
// }
// //------------------------------------------------------------------------------
