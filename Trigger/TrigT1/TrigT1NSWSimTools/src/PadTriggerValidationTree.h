/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef NSWL1_PADTRIGGERVALIDATIONTREE_H
#define NSWL1_PADTRIGGERVALIDATIONTREE_H

#include "GeoPrimitives/GeoPrimitives.h" // Amg::Vector3D (cannot fw declare typedef)

#include <string>
#include <utility> // pair
#include <vector>

class TTree;

namespace NSWL1 {

class PadTrigger;

/**
   @brief Make TTree to validate the PadTriggerLogicOfflineTool.
bb
   \ todo implement all branches (for now just a skeleton based on
   PadTdsValidationTree)

   davide.gerbaudo@gmail.com
   Oct 2015
*/
class PadTriggerValidationTree {

public:
    PadTriggerValidationTree();
    static std::string treename_from_algoname(const std::string &algoname);
    /// initialize the output containers and the tree
    /**
       The tree should be provided by the THistSvc from Athena.
    */
    bool init_tree(TTree *tree);
    void reset_ntuple_variables(); ///< clear all vectors of internal containers
    void clear_ntuple_variables(); ///< set to 0 all pointers of internal containers
    void fill_num_pad_triggers(size_t num);                                  ///< store the number of pad triggers per event
    void fill_pad_trigger_basics(const std::vector<PadTrigger*> &triggers);  ///< store basic information about the pad triggers 
    /*
    void fill_num_pad_hits(size_t num); ///< store the number of hits for one pad
    void fill_hit_global_pos(const Amg::Vector3D& pos); ///< store global position of a hit
    void fill_truth_hit_global_pos(const Amg::Vector3D& pos); ///< store global position of a truth-matched hit
    */
private:
    std::string m_treename;
    TTree* m_tree;                                          ///< ntuple for analysis
    int m_nPadTriggers; ///< number of triggers per event
    std::vector<unsigned int>* m_padTriggerBCID;      ///< BCID of the pad trigger
    std::vector<int>*          m_padTriggerSectorID;  ///< Sector ID of the pad trigger
    std::vector<int>*          m_padTriggerSideID;    ///< Side ID of the pad trigger
    std::vector<unsigned int>* m_padTriggerBandID;    ///< band ID used to determine which strips are read out    
    std::vector<float>*        m_padTriggerEta;       ///< Trigger halfpad eta    
    std::vector<float>*        m_padTriggerPhi;       ///< Trigger halfpad phi    
    std::vector<int>*          m_padTriggerEtaID;     ///< Trigger halfpad eta id   
    std::vector<int>*          m_padTriggerPhiID;     ///< Trigger halfpad phi id   
    std::vector<int>*          m_padTriggerMultipletID;  ///< Multiplet ID of the pad trigger   
    std::vector<int>*          m_padTriggerGasGapID;     ///< GasGap ID of the pad trigger
    // int m_nPadHits;                                         ///< number of PAD hit delivered
    // std::vector<float>* m_padGlobalX;                       ///< global position X of the PAD hit
    // std::vector<float>* m_padGlobalY;                       ///< global position Y of the PAD hit
    // std::vector<float>* m_padGlobalZ;                       ///< global position Z of the PAD hit
    // std::vector<float>* m_padTruthHitGlobalX;               ///< global position X of the truth hit associated to the PAD hit
    // std::vector<float>* m_padTruthHitGlobalY;               ///< global position Y of the truth hit associated to the PAD hit
    // std::vector<float>* m_padTruthHitGlobalZ;               ///< global position Z of the truth hit associated to the PAD hit
    // std::vector<int>*   m_padEtaIdFromOfflineId;            ///< PAD eta Id from the offline Id
    // std::vector<int>*   m_padPhiIdFromOfflineId;            ///< PAD phi Id from the offline Id
    // std::vector<int>*   m_padSectorFromOfflineId;           ///< PAD sector Id from the offline Id
    // std::vector<int>*   m_padLayerFromOfflineId;            ///< PAD layer Id from the offline Id
    // std::vector<int>*   m_offlineIdPadEtaConverted;         ///< PAD eta Id from the offline Id that were converted
    // std::vector<int>*   m_offlineIdPadPhiConverted;         ///< PAD phi Id from the offline Id that were converted
    // std::vector<int>*   m_padEtaIdFromOldSimu;              ///< PAD eta Id from the standalone simulation code
    // std::vector<int>*   m_padPhiIdFromOldSimu;              ///< PAD phi Id from the standalone simulation code
};  // PadTriggerValidationTree

} // NSWL1
#endif
