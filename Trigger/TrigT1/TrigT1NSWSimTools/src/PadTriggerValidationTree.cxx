/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadTriggerValidationTree.h"

#include "TrigT1NSWSimTools/PadTrigger.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"


namespace NSWL1{
    //------------------------------------------------------------------------------
    PadTriggerValidationTree::PadTriggerValidationTree():
        m_tree(nullptr),
        m_nPadTriggers(0),
        m_padTriggerBCID(nullptr),
        m_padTriggerSectorID(nullptr),
        m_padTriggerSectorType(nullptr),
        m_padTriggerSideID(nullptr),
        m_padTriggerBandID(nullptr),
        m_padTriggerEta(nullptr),
        m_padTriggerPhi(nullptr),
        m_padTriggerEtaID(nullptr),
        m_padTriggerPhiID(nullptr),
        m_padTriggerMultipletID(nullptr),
        m_padTriggerGasGapID(nullptr),
        m_padTriggerEtamin(nullptr),
        m_padTriggerEtamax(nullptr),
        m_padTriggerPhimin(nullptr),
        m_padTriggerPhimax(nullptr),
        m_padTriggerlocalminY(nullptr),
        m_padTriggerlocalmaxY(nullptr)
    {
    }
    //------------------------------------------------------------------------------
    std::string PadTriggerValidationTree::treename_from_algoname(const std::string &algoname)
    {
        std::string treename = algoname+"Tree";
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
            m_padTriggerSectorType = new std::vector<int>();
            m_padTriggerSideID   = new std::vector<int>();
            m_padTriggerBandID   = new std::vector<unsigned int>();
            m_padTriggerEta      = new std::vector<float>();
            m_padTriggerPhi      = new std::vector<float>();
            m_padTriggerEtaID    = new std::vector<int>();
            m_padTriggerPhiID    = new std::vector<int>();
            m_padTriggerMultipletID = new std::vector<int>();
            m_padTriggerGasGapID    = new std::vector<int>();
            m_padTriggerEtamin      =new std::vector<float>();
            m_padTriggerEtamax      =new std::vector<float>();
            m_padTriggerPhimin      =new std::vector<float>();
            m_padTriggerPhimax      =new std::vector<float>();
            m_padTriggerlocalminY   =new std::vector<std::vector<float>>();
            m_padTriggerlocalmaxY   =new std::vector<std::vector<float>>();

            m_tree->Branch("nPadTriggers",                 &m_nPadTriggers,"nPadTriggers/i");
            m_tree->Branch("padTriggerBCID",               &m_padTriggerBCID);
            m_tree->Branch("padTriggerSectorID",           &m_padTriggerSectorID);
            m_tree->Branch("padTriggerSectorType",         &m_padTriggerSectorType);
            m_tree->Branch("padTriggerSideID",             &m_padTriggerSideID);
            m_tree->Branch("padTriggerBandID",             &m_padTriggerBandID);
            m_tree->Branch("padTriggerEta",                &m_padTriggerEta);
            m_tree->Branch("padTriggerPhi",                &m_padTriggerPhi);
            m_tree->Branch("padTriggerEtaID",              &m_padTriggerEtaID);
            m_tree->Branch("padTriggerPhiID",              &m_padTriggerPhiID);
            m_tree->Branch("padTriggerMultipletID",        &m_padTriggerMultipletID);
            m_tree->Branch("padTriggerGasGapID",           &m_padTriggerGasGapID);

            m_tree->Branch("padTriggerEtamin",           &m_padTriggerEtamin);
            m_tree->Branch("padTriggerEtamax",           &m_padTriggerEtamax);
            m_tree->Branch("padTriggerPhimin",           &m_padTriggerPhimin);
            m_tree->Branch("padTriggerPhimax",           &m_padTriggerPhimax);
            
            m_tree->Branch("padTriggerlocalminY",           &m_padTriggerlocalminY);
            m_tree->Branch("padTriggerlocalmaxY",           &m_padTriggerlocalmaxY);
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
            m_padTriggerSectorType ->clear();
            m_padTriggerSideID   ->clear();
            m_padTriggerBandID   ->clear();
            m_padTriggerEta      ->clear();
            m_padTriggerPhi      ->clear();
            m_padTriggerEtaID    ->clear();
            m_padTriggerPhiID    ->clear();
            m_padTriggerMultipletID ->clear();
            m_padTriggerGasGapID    ->clear();
            m_padTriggerEtamin->clear();
            m_padTriggerEtamax->clear();
            m_padTriggerPhimin->clear();
            m_padTriggerPhimax->clear();
            m_padTriggerlocalminY->clear();
            m_padTriggerlocalmaxY->clear();
        }
    }
    //------------------------------------------------------------------------------
    void PadTriggerValidationTree::clear_ntuple_variables()
    {
        m_nPadTriggers       = 0;
        m_padTriggerBCID     = nullptr;
        m_padTriggerSectorID = nullptr;
        m_padTriggerSectorType = nullptr;
        m_padTriggerSideID   = nullptr;
        m_padTriggerBandID   = nullptr;
        m_padTriggerEta      = nullptr;
        m_padTriggerPhi      = nullptr;
        m_padTriggerEtaID    = nullptr;
        m_padTriggerPhiID    = nullptr;
        m_padTriggerMultipletID = nullptr;
        m_padTriggerGasGapID    = nullptr;
        m_padTriggerEtamax=nullptr;
        m_padTriggerEtamin=nullptr;
        m_padTriggerPhimin=nullptr;
        m_padTriggerPhimax=nullptr;
        m_padTriggerlocalminY=nullptr;
        m_padTriggerlocalmaxY=nullptr;
    }
    //------------------------------------------------------------------------------
    void PadTriggerValidationTree::fill_num_pad_triggers(size_t num)
    {
        m_nPadTriggers = num;
    }
    //------------------------------------------------------------------------------
    void PadTriggerValidationTree::fill_pad_trigger_basics(const std::vector<std::unique_ptr<PadTrigger>> &triggers) {
    for(auto& trigger : triggers) {
        m_padTriggerBCID       ->push_back(trigger->bctag());
        m_padTriggerSectorID   ->push_back(trigger->sectorId());
        m_padTriggerSectorType   ->push_back((trigger->isSmall()+1)%2);//1 for L 0 for S
        m_padTriggerSideID     ->push_back(trigger->sideId()); 
        m_padTriggerBandID     ->push_back(trigger->bandId()); 
        m_padTriggerEta        ->push_back(trigger->eta()); 
        m_padTriggerPhi        ->push_back(trigger->phi()); 
        m_padTriggerEtaID      ->push_back(trigger->etaId()); 
        m_padTriggerPhiID      ->push_back(trigger->phiId()); 
        m_padTriggerMultipletID->push_back(trigger->multipletId()); 
        m_padTriggerGasGapID   ->push_back(trigger->gasGapId()); 
        m_padTriggerEtamin->push_back(trigger->etaMin());
        m_padTriggerEtamax->push_back(trigger->etaMax());
        m_padTriggerPhimin->push_back(trigger->phiMin());
        m_padTriggerPhimax->push_back(trigger->phiMax());
        m_padTriggerlocalminY->push_back(trigger->trglocalminY());
        m_padTriggerlocalmaxY->push_back(trigger->trglocalmaxY());

    }
    }

}
