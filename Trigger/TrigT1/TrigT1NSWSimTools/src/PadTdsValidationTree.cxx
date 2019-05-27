/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadTdsValidationTree.h"

#include "TrigT1NSWSimTools/PadOfflineData.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"


namespace NSWL1{
    //------------------------------------------------------------------------------
    PadTdsValidationTree::PadTdsValidationTree():
        m_tree(NULL),
        m_nPadHits(0),
        m_padGlobalX(NULL),
        m_padGlobalY(NULL),
        m_padGlobalZ(NULL),
        m_padDelayedTime(NULL),
        m_padBCHR(NULL),
        m_padGlobalCornerX(NULL),
        m_padGlobalCornerY(NULL),
        m_padGlobalCornerZ(NULL),
        m_padTruthHitGlobalX(NULL),
        m_padTruthHitGlobalY(NULL),
        m_padTruthHitGlobalZ(NULL),
        m_padEtaIdFromOfflineId(NULL),
        m_padPhiIdFromOfflineId(NULL),
        m_padSectorIdFromOfflineId(NULL),
        m_padSectorTypeFromOfflineId(NULL),
        m_padGasGapIdFromOfflineId(NULL),
        m_padModuleIdFromOfflineId(NULL),
        m_padMultipletIdFromOfflineId(NULL),
        m_padSideIdFromOfflineId(NULL),
        m_offlineIdPadEtaConverted(NULL),
        m_offlineIdPadPhiConverted(NULL),
        m_padEtaIdFromOldSimu(NULL),
        m_padPhiIdFromOldSimu(NULL)
    {
    }
    //------------------------------------------------------------------------------
    std::string PadTdsValidationTree::treename_from_algoname(const std::string &algoname)
    {
        std::string treename = algoname+"Tree";
        return treename;    
    }
    //------------------------------------------------------------------------------
    bool PadTdsValidationTree::init_tree(TTree *tree)
    {
        bool success=false;
        if(tree){
            m_tree = tree;
            m_nPadHits = 0;
            m_padGlobalX                  = new std::vector<float>();
            m_padGlobalY                  = new std::vector<float>();
            m_padGlobalZ                  = new std::vector<float>();
            m_padDelayedTime              = new std::vector<float>();
            m_padBCHR                     = new std::vector<int>();
            m_padGlobalCornerX            = new std::vector<float>();
            m_padGlobalCornerY            = new std::vector<float>();
            m_padGlobalCornerZ            = new std::vector<float>();
            m_padTruthHitGlobalX          = new std::vector<float>();
            m_padTruthHitGlobalY          = new std::vector<float>();
            m_padTruthHitGlobalZ          = new std::vector<float>();
            m_padEtaIdFromOfflineId       = new std::vector<int>();
            m_padPhiIdFromOfflineId       = new std::vector<int>();
            m_padSectorIdFromOfflineId    = new std::vector<int>();
            m_padSectorTypeFromOfflineId  = new std::vector<int>();
            m_padGasGapIdFromOfflineId    = new std::vector<int>();
            m_padModuleIdFromOfflineId    = new std::vector<int>();
            m_padMultipletIdFromOfflineId = new std::vector<int>();
            m_padSideIdFromOfflineId      = new std::vector<int>();
            m_offlineIdPadEtaConverted    = new std::vector<int>();
            m_offlineIdPadPhiConverted    = new std::vector<int>();
            m_padEtaIdFromOldSimu         = new std::vector<int>();
            m_padPhiIdFromOldSimu         = new std::vector<int>();

            m_tree->Branch("nPadHits",                    &m_nPadHits,"nPadHits/i");
            m_tree->Branch("padGlobalX",                  &m_padGlobalX);
            m_tree->Branch("padGlobalY",                  &m_padGlobalY);
            m_tree->Branch("padGlobalZ",                  &m_padGlobalZ);
            m_tree->Branch("padDelayedTime",              &m_padDelayedTime);
            m_tree->Branch("padBCHR",                     &m_padBCHR);
            m_tree->Branch("padGlobalCornerX",            &m_padGlobalCornerX);
            m_tree->Branch("padGlobalCornerY",            &m_padGlobalCornerY);
            m_tree->Branch("padGlobalCornerZ",            &m_padGlobalCornerZ);
            m_tree->Branch("padTruthHitGlobalX",          &m_padTruthHitGlobalX);
            m_tree->Branch("padTruthHitGlobalY",          &m_padTruthHitGlobalY);
            m_tree->Branch("padTruthHitGlobalZ",          &m_padTruthHitGlobalZ);
            m_tree->Branch("padEtaIdFromOfflineId",       &m_padEtaIdFromOfflineId);
            m_tree->Branch("padPhiIdFromOfflineId",       &m_padPhiIdFromOfflineId);
            m_tree->Branch("padSectorIdFromOfflineId",    &m_padSectorIdFromOfflineId);
            m_tree->Branch("padSectorTypeFromOfflineId",  &m_padSectorTypeFromOfflineId);
            m_tree->Branch("padGasGapIdFromOfflineId",    &m_padGasGapIdFromOfflineId);
            m_tree->Branch("padModuleIdFromOfflineId",    &m_padModuleIdFromOfflineId);
            m_tree->Branch("padMultipletIdFromOfflineId", &m_padMultipletIdFromOfflineId);
            m_tree->Branch("padSideIdFromOfflineId",      &m_padSideIdFromOfflineId);
            m_tree->Branch("offlineIdPadEtaIdConverted",  &m_offlineIdPadEtaConverted);
            m_tree->Branch("offlineIdPadPhiIdConverted",  &m_offlineIdPadPhiConverted);
            m_tree->Branch("padEtaIdFromOldSimu",         &m_padEtaIdFromOldSimu);
            m_tree->Branch("padPhiIdFromOldSimu",         &m_padPhiIdFromOldSimu);
    /**/
            success = true;
        }
        return success;
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::reset_ntuple_variables()
    {
        if(m_tree){
            m_nPadHits = 0;
            m_padGlobalX->clear();
            m_padGlobalY->clear();
            m_padGlobalZ->clear();
            m_padDelayedTime->clear();
            m_padBCHR->clear();
            m_padGlobalCornerX->clear();
            m_padGlobalCornerY->clear();
            m_padGlobalCornerZ->clear();
            m_padTruthHitGlobalX->clear();
            m_padTruthHitGlobalY->clear();
            m_padTruthHitGlobalZ->clear();
            m_padEtaIdFromOfflineId->clear();
            m_padPhiIdFromOfflineId->clear();
            m_padSectorIdFromOfflineId->clear();
            m_padSectorTypeFromOfflineId->clear();
            m_padGasGapIdFromOfflineId->clear();
            m_padModuleIdFromOfflineId->clear();
            m_padMultipletIdFromOfflineId->clear();
            m_padSideIdFromOfflineId->clear();
            m_offlineIdPadEtaConverted->clear();
            m_offlineIdPadPhiConverted->clear();
            m_padEtaIdFromOldSimu->clear();
            m_padPhiIdFromOldSimu->clear();
        }
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::clear_ntuple_variables()
    {
        m_nPadHits                    = 0;
        m_padGlobalX                  = NULL;
        m_padGlobalY                  = NULL;
        m_padGlobalZ                  = NULL;
        m_padDelayedTime              = NULL;
        m_padBCHR                     = NULL;
        m_padGlobalCornerX            = NULL;
        m_padGlobalCornerY            = NULL;
        m_padGlobalCornerZ            = NULL;
        m_padTruthHitGlobalX          = NULL;
        m_padTruthHitGlobalY          = NULL;
        m_padTruthHitGlobalZ          = NULL;
        m_padEtaIdFromOfflineId       = NULL;
        m_padPhiIdFromOfflineId       = NULL;
        m_padSectorIdFromOfflineId    = NULL;
        m_padSectorTypeFromOfflineId  = NULL;
        m_padGasGapIdFromOfflineId    = NULL;    
        m_padModuleIdFromOfflineId    = NULL;    
        m_padMultipletIdFromOfflineId = NULL;
        m_padSideIdFromOfflineId      = NULL;
        m_offlineIdPadEtaConverted    = NULL;
        m_offlineIdPadPhiConverted    = NULL;
        m_padEtaIdFromOldSimu         = NULL;
        m_padPhiIdFromOldSimu         = NULL;
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::fill_num_pad_hits(size_t num)
    {
        m_nPadHits = num;
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::fill_hit_global_pos(const Amg::Vector3D &pos)
    {
        m_padGlobalX->push_back(pos.x());
        m_padGlobalY->push_back(pos.y());
        m_padGlobalZ->push_back(pos.z());
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::fill_hit_global_corner_pos(const std::vector<Amg::Vector3D> &pos)
    {
        // Logic is 4-corners:
        //   x1,y1 : Lower left (locally)
        //   x2,y2 : Lower right (locally) s.t. y1=y2
        //   x3,y3 : Upper left (locally)
        //   x4,y4 : Upper right (locally) s.t. y3=y4
        for(auto corner : pos) {
        m_padGlobalCornerX->push_back(corner.x());
        m_padGlobalCornerY->push_back(corner.y());
        m_padGlobalCornerZ->push_back(corner.z());
        }
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::fill_truth_hit_global_pos(const Amg::Vector3D &pos)
    {
        m_padTruthHitGlobalX->push_back(pos.x());
        m_padTruthHitGlobalY->push_back(pos.y());
        m_padTruthHitGlobalZ->push_back(pos.z());
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::fill_offlineid_info(const PadOfflineData &o, float bin_offset)
    {
        m_padEtaIdFromOfflineId->push_back       ( o.padEtaId() + bin_offset );
        m_padPhiIdFromOfflineId->push_back       ( o.padPhiId() + bin_offset );
        m_padSideIdFromOfflineId->push_back      ( o.sideId()      ); 
        m_padModuleIdFromOfflineId->push_back    ( o.moduleId()    );
        m_padSectorIdFromOfflineId->push_back    ( o.sectorId()    );
        m_padSectorTypeFromOfflineId->push_back  ( o.sectorType()  );
        m_padMultipletIdFromOfflineId->push_back ( o.multipletId() );
        m_padGasGapIdFromOfflineId->push_back    ( o.gasGapId()    );
        m_padDelayedTime->push_back              ( o.time()        );
        // For debugging - use w/ CARE!!! ASM 16/3/17
        uint16_t bc_tag = o.BC_Tag();
        uint16_t bc_max = 0xFFFF;
        int      bc_hr  = -999;
        if((bc_tag >> 15) == 1) { // 400 BC 
            bc_hr = (int) ((~bc_tag)*(-1)) - (int) bc_max - 1;
        } else {
            bc_hr = (int) bc_tag;
        }
        m_padBCHR->push_back( bc_hr );
    }
    //------------------------------------------------------------------------------
    void PadTdsValidationTree::fill_matched_old_id_new_id(const std::pair<int,int> &old_id, std::pair<int,int> &new_id)
    {
        m_padEtaIdFromOldSimu->push_back(old_id.first );
        m_padPhiIdFromOldSimu->push_back(old_id.second);
        m_offlineIdPadEtaConverted->push_back(new_id.first );
        m_offlineIdPadPhiConverted->push_back(new_id.second);
    }
    //------------------------------------------------------------------------------
}
