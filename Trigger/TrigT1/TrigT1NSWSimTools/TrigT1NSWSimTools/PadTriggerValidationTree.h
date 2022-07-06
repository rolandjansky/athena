/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADTRIGGERVALIDATIONTREE_H
#define NSWL1_PADTRIGGERVALIDATIONTREE_H

#include "GaudiKernel/ITHistSvc.h"
#include "GeoPrimitives/GeoPrimitives.h" // Amg::Vector3D (cannot fw declare typedef)
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TTree.h"

#include "CxxUtils/checker_macros.h"
#include <string>
#include <utility> // pair
#include <vector>

namespace NSWL1 {

class PadTrigger;

/**
   @brief Make TTree to validate the PadTriggerLogicOfflineTool.
bb
   \ todo implement all branches (for now just a skeleton based on
   PadTdsValidationTree)

   davide.gerbaudo@gmail.com
   Oct 2015

   Updates for release 22 processing: francesco.giuseppe.gravili@cern.ch
*/


class ATLAS_NOT_THREAD_SAFE PadTriggerValidationTree {

  public:
    PadTriggerValidationTree();
    /// initialize the output containers and the tree
    /**
       The tree should be provided by the THistSvc from Athena.
    */
    bool init_tree(TTree *tree);
    bool reset_ntuple_variables(); ///< clear all vectors of internal containers
    bool fill_num_pad_triggers(size_t num) const;                                  ///< store the number of pad triggers per event
    bool fill_pad_trigger_basics(const std::vector<std::unique_ptr<PadTrigger>> &triggers) const;  ///< store basic information about the pad triggers

  private:
    TTree* m_tree{nullptr};                                    ///< ntuple for analysis
    mutable int m_nPadTriggers;                                ///< number of triggers per event
    std::vector<unsigned int>* m_padTriggerBCID{nullptr};      ///< BCID of the pad trigger
    std::vector<int>*          m_padTriggerModuleIDinner{nullptr};
    std::vector<int>*          m_padTriggerModuleIDouter{nullptr};
    std::vector<std::vector<int>>* m_padTriggerSelectedLayersInner{nullptr};
    std::vector<std::vector<int>>* m_padTriggerSelectedLayersOuter{nullptr};
    std::vector<std::vector<int>>* m_padTriggerSelectedBandsInner{nullptr};
    std::vector<std::vector<int>>* m_padTriggerSelectedBandsOuter{nullptr};
    std::vector<std::vector<int>>* m_padTriggerPadEtaIndicesInner{nullptr};
    std::vector<std::vector<int>>* m_padTriggerPadPhiIndicesInner{nullptr};
    std::vector<std::vector<int>>* m_padTriggerPadEtaIndicesOuter{nullptr};
    std::vector<std::vector<int>>* m_padTriggerPadPhiIndicesOuter{nullptr};
    std::vector<std::vector<float>>* m_padTriggerRCenterMinInner{nullptr};
    std::vector<std::vector<float>>* m_padTriggerRCenterMaxInner{nullptr};
    std::vector<std::vector<float>>* m_padTriggerRCenterMinOuter{nullptr};
    std::vector<std::vector<float>>* m_padTriggerRCenterMaxOuter{nullptr};

    std::vector<int>*          m_padTriggerSectorID{nullptr};  ///< Sector ID of the pad trigger
    std::vector<int>*          m_padTriggerSectorType{nullptr};
    std::vector<int>*          m_padTriggerSideID{nullptr};    ///< Side ID of the pad trigger
    std::vector<unsigned int>* m_padTriggerBandID{nullptr};    ///< band ID used to determine which strips are read out
    std::vector<float>*        m_padTriggerEta{nullptr};       ///< Trigger halfpad eta
    std::vector<float>*        m_padTriggerPhi{nullptr};       ///< Trigger halfpad phi
    std::vector<int>*          m_padTriggerEtaID{nullptr};     ///< Trigger halfpad eta id
    std::vector<int>*          m_padTriggerPhiID{nullptr};     ///< Trigger halfpad phi id
    std::vector<int>*          m_padTriggerMultipletID{nullptr};  ///< Multiplet ID of the pad trigger

    std::vector<float>*        m_padTriggerEtamin{nullptr};
    std::vector<float>*        m_padTriggerEtamax{nullptr};
    std::vector<float>*        m_padTriggerPhimin{nullptr};
    std::vector<float>*        m_padTriggerPhimax{nullptr};

    std::vector<std::vector<float>>* m_padTriggerlocalminYInner{nullptr};
    std::vector<std::vector<float>>* m_padTriggerlocalmaxYInner{nullptr};
    std::vector<std::vector<float>>* m_padTriggerlocalminYOuter{nullptr};
    std::vector<std::vector<float>>* m_padTriggerlocalmaxYOuter{nullptr};
    std::vector<int>* m_padTriggerIndex{nullptr};
  };  // PadTriggerValidationTree
} // NSWL1
#endif
