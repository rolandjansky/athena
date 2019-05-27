/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef NSWL1_PADTDSVALIDATIONTREE_H
#define NSWL1_PADTDSVALIDATIONTREE_H

#include "GeoPrimitives/GeoPrimitives.h" // Amg::Vector3D (cannot fw declare typedef)

#include <string>
#include <utility> // pair
#include <vector>

class TTree;
class Identifier;

// namespace for the NSW LVL1 related classes
namespace NSWL1 {

class PadOfflineData;

/**
   @brief TTree maker to store the variables used to validate the input/output to the PadTdsOfflineTool.

   Sept 2014
   davide.gerbaudo@gmail.com
 */
class PadTdsValidationTree {

public:
    PadTdsValidationTree();
    static std::string treename_from_algoname(const std::string &algoname);
    /// initialize the output containers and the tree
    /**
       The tree should be provided by the THistSvc from Athena.
    */
    bool init_tree(TTree *tree);
    void reset_ntuple_variables(); ///< clear all vectors of internal containers
    void clear_ntuple_variables(); ///< set to 0 all pointers of internal containers
    void fill_num_pad_hits(size_t num); ///< store the number of hits for one pad
    void fill_hit_global_pos(const Amg::Vector3D& pos); ///< store global position of a hit
    void fill_hit_global_corner_pos(const std::vector<Amg::Vector3D> &pos); ///< store global position of a hit
    void fill_truth_hit_global_pos(const Amg::Vector3D& pos); ///< store global position of a truth-matched hit
    /// store eta,phi indices + sector,layer
    /**
       'bin_offset' used to center the bin on the value of the Pad Id.
       Introduced by Alessandro, it's always 0.0. Do we really need it? (DG-2014-09-30)
     */
    void fill_offlineid_info(const PadOfflineData &o, float bin_offset);
    void fill_matched_old_id_new_id(const std::pair<int,int> &old_id, std::pair<int,int> &new_id);
private:
    std::string m_treename;
    TTree* m_tree;                                          ///< ntuple for analysis
    int m_nPadHits;                                         ///< number of PAD hit delivered
    std::vector<float>* m_padGlobalX;                       ///< global position X of the PAD hit
    std::vector<float>* m_padGlobalY;                       ///< global position Y of the PAD hit
    std::vector<float>* m_padGlobalZ;                       ///< global position Z of the PAD hit
    std::vector<float>* m_padDelayedTime;                   ///< delayed time of the PAD hit
    std::vector<int>*   m_padBCHR;                          ///< human readable BC info of the PAD
    std::vector<float>* m_padGlobalCornerX;                 ///< global position X of the PAD hit corner
    std::vector<float>* m_padGlobalCornerY;                 ///< global position Y of the PAD hit corner 
    std::vector<float>* m_padGlobalCornerZ;                 ///< global position Z of the PAD hit corner  
    std::vector<float>* m_padTruthHitGlobalX;               ///< global position X of the truth hit associated to the PAD hit
    std::vector<float>* m_padTruthHitGlobalY;               ///< global position Y of the truth hit associated to the PAD hit
    std::vector<float>* m_padTruthHitGlobalZ;               ///< global position Z of the truth hit associated to the PAD hit
    std::vector<int>*   m_padEtaIdFromOfflineId;            ///< PAD eta Id from the offline Id
    std::vector<int>*   m_padPhiIdFromOfflineId;            ///< PAD phi Id from the offline Id
    std::vector<int>*   m_padSectorIdFromOfflineId;         ///< PAD sector Id from the offline Id
    std::vector<int>*   m_padSectorTypeFromOfflineId;       ///< PAD sector type from the offline Id
    std::vector<int>*   m_padGasGapIdFromOfflineId;         ///< PAD gas gap Id from the offline Id
    std::vector<int>*   m_padModuleIdFromOfflineId;         ///< PAD module Id from the offline Id
    std::vector<int>*   m_padMultipletIdFromOfflineId;      ///< PAD multilayer Id from the offline Id
    std::vector<int>*   m_padSideIdFromOfflineId;           ///< PAD side Id from the offline Id
    std::vector<int>*   m_offlineIdPadEtaConverted;         ///< PAD eta Id from the offline Id that were converted
    std::vector<int>*   m_offlineIdPadPhiConverted;         ///< PAD phi Id from the offline Id that were converted
    std::vector<int>*   m_padEtaIdFromOldSimu;              ///< PAD eta Id from the standalone simulation code
    std::vector<int>*   m_padPhiIdFromOldSimu;              ///< PAD phi Id from the standalone simulation code

};  // PadTdsValidationTree


} // NSWL1
#endif
