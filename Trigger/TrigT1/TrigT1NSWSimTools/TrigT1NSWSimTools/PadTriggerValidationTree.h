/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef NSWL1_PADTRIGGERVALIDATIONTREE_H
#define NSWL1_PADTRIGGERVALIDATIONTREE_H

#include "GeoPrimitives/GeoPrimitives.h" // Amg::Vector3D (cannot fw declare typedef)
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include <string>
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
    void fill_pad_trigger_basics(const std::vector<std::unique_ptr<PadTrigger>> &triggers);  ///< store basic information about the pad triggers 
private:
    std::string m_treename;
    TTree* m_tree;                                          ///< ntuple for analysis
    int m_nPadTriggers; ///< number of triggers per event
    std::vector<unsigned int>* m_padTriggerBCID;      ///< BCID of the pad trigger
    std::vector<int>*          m_padTriggerSectorID;  ///< Sector ID of the pad trigger
    std::vector<int>*          m_padTriggerSectorType;
    std::vector<int>*          m_padTriggerSideID;    ///< Side ID of the pad trigger
    std::vector<unsigned int>* m_padTriggerBandID;    ///< band ID used to determine which strips are read out    
    std::vector<float>*        m_padTriggerEta;       ///< Trigger halfpad eta    
    std::vector<float>*        m_padTriggerPhi;       ///< Trigger halfpad phi    
    std::vector<int>*          m_padTriggerEtaID;     ///< Trigger halfpad eta id   
    std::vector<int>*          m_padTriggerPhiID;     ///< Trigger halfpad phi id   
    std::vector<int>*          m_padTriggerMultipletID;  ///< Multiplet ID of the pad trigger   
    std::vector<int>*          m_padTriggerGasGapID;     ///< GasGap ID of the pad trigger
    
    std::vector<float>*        m_padTriggerEtamin;  
    std::vector<float>*        m_padTriggerEtamax;
    std::vector<float>*        m_padTriggerPhimin;
    std::vector<float>*        m_padTriggerPhimax;
    
    std::vector<std::vector<float>>* m_padTriggerlocalminY;
    std::vector<std::vector<float>>* m_padTriggerlocalmaxY;
};  // PadTriggerValidationTree

} // NSWL1
#endif
