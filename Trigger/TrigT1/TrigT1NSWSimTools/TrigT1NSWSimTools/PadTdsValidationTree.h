/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADTDSVALIDATIONTREE_H
#define NSWL1_PADTDSVALIDATIONTREE_H

#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GeoPrimitives/GeoPrimitives.h" // Amg::Vector3D (cannot fw declare typedef)
#include "TTree.h"

#include <string>
#include <utility> // pair
#include <vector>

class Identifier;

// namespace for the NSW LVL1 related classes
namespace NSWL1 {

class PadOfflineData;

/*
 * @brief TTree maker to store the variables used to validate the input/output to the PadTdsOfflineTool
 * Sept 2014 : davide.gerbaudo@gmail.com
 * Major updates for Release 22: francesco.giuseppe.gravili@cern.ch
 */

class ATLAS_NOT_THREAD_SAFE PadTdsValidationTree {
  public:
    PadTdsValidationTree();
    /// initialize the output containers and the tree
    /**
       The tree should be provided by the THistSvc from Athena.
    */
    bool init_tree(TTree *tree);
    bool reset_ntuple_variables(); ///< clear all vectors of internal containers
    void fill_num_pad_hits(size_t num) const; ///< store the number of hits for one pad
    void fill_hit_global_pos(const Amg::Vector3D& pos) const; ///< store global position of a hit
    void fill_hit_global_corner_pos(const std::vector<Amg::Vector3D> &pos) const; ///< store global position of a hit
    void fill_truth_hit_global_pos(const Amg::Vector3D& pos) const; ///< store global position of a truth-matched hit
    /// store eta,phi indices + sector,layer
    /**
       'bin_offset' used to center the bin on the value of the Pad Id.
       Introduced by Alessandro, it's always 0.0. Do we really need it? (DG-2014-09-30)
     */
    void fill_offlineid_info(const PadOfflineData &o, float bin_offset) const;
    void fill_matched_old_id_new_id(const std::pair<int,int> &old_id, std::pair<int,int> &new_id) const;

  private:
    std::string m_treename;
    TTree* m_tree{nullptr};                                          ///< ntuple for analysis
    std::vector<int>*   m_nPadHits{nullptr};                         ///< number of PAD hit delivered
    std::vector<float>* m_padGlobalX{nullptr};                       ///< global position X of the PAD hit
    std::vector<float>* m_padGlobalY{nullptr};                       ///< global position Y of the PAD hit
    std::vector<float>* m_padGlobalZ{nullptr};                       ///< global position Z of the PAD hit
    std::vector<float>* m_padDelayedTime{nullptr};                   ///< delayed time of the PAD hit
    std::vector<int>*   m_padBCHR{nullptr};                          ///< human readable BC info of the PAD
    std::vector<float>* m_padGlobalCornerX{nullptr};                 ///< global position X of the PAD hit corner
    std::vector<float>* m_padGlobalCornerY{nullptr};                 ///< global position Y of the PAD hit corner
    std::vector<float>* m_padGlobalCornerZ{nullptr};                 ///< global position Z of the PAD hit corner
    std::vector<float>* m_padTruthHitGlobalX{nullptr};               ///< global position X of the truth hit associated to the PAD hit
    std::vector<float>* m_padTruthHitGlobalY{nullptr};               ///< global position Y of the truth hit associated to the PAD hit
    std::vector<float>* m_padTruthHitGlobalZ{nullptr};               ///< global position Z of the truth hit associated to the PAD hit
    std::vector<int>*   m_padEtaIdFromOfflineId{nullptr};            ///< PAD eta Id from the offline Id
    std::vector<int>*   m_padPhiIdFromOfflineId{nullptr};            ///< PAD phi Id from the offline Id
    std::vector<int>*   m_padSectorIdFromOfflineId{nullptr};         ///< PAD sector Id from the offline Id
    std::vector<int>*   m_padSectorTypeFromOfflineId{nullptr};       ///< PAD sector type from the offline Id
    std::vector<int>*   m_padGasGapIdFromOfflineId{nullptr};         ///< PAD gas gap Id from the offline Id
    std::vector<int>*   m_padModuleIdFromOfflineId{nullptr};         ///< PAD module Id from the offline Id
    std::vector<int>*   m_padMultipletIdFromOfflineId{nullptr};      ///< PAD multilayer Id from the offline Id
    std::vector<int>*   m_padSideIdFromOfflineId{nullptr};           ///< PAD side Id from the offline Id
    std::vector<int>*   m_offlineIdPadEtaConverted{nullptr};         ///< PAD eta Id from the offline Id that were converted
    std::vector<int>*   m_offlineIdPadPhiConverted{nullptr};         ///< PAD phi Id from the offline Id that were converted
    std::vector<int>*   m_padEtaIdFromOldSimu{nullptr};              ///< PAD eta Id from the standalone simulation code
    std::vector<int>*   m_padPhiIdFromOldSimu{nullptr};              ///< PAD phi Id from the standalone simulation code
  };  // PadTdsValidationTree
} // NSWL1
#endif
