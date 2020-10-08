///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// METCaloRegionsTool.h 
// Header file for class METCaloRegionsTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METCALOREGIONSTOOL_H
#define METRECONSTRUCTION_METCALOREGIONSTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "StoreGate/ReadCondHandleKey.h"


// MET EDM
#include "xAODMissingET/MissingETContainer.h"

// METRecoInterface includes
#include "METRecoInterface/IMETToolBase.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloCellContainer;

// MET EDM
#if defined(XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
#else
#include "CaloConditions/CaloNoise.h"
#endif



namespace met{

  class METCaloRegionsTool
    : public asg::AsgTool,
      virtual public IMETToolBase
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METCaloRegionsTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METCaloRegionsTool(const std::string& name);
    ~METCaloRegionsTool();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    StatusCode  execute(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  protected: 

    // Internal variables
    std::string      m_input_data_key;
    std::string      m_output_met_key;
    bool             m_calo_useCells;
    bool             m_calo_doTriggerMet;
    // Accept functions
    bool             accept            (const xAOD::IParticle* object) const;
    // Overlap resolver function
    bool             resolveOverlap    (const xAOD::IParticle* object,
                                        xAOD::MissingETComponentMap* metMap,
                                        std::vector<const xAOD::IParticle*>& acceptedSignals,
                                        MissingETBase::Types::weight_t& objWeight) const;
    // Fill Cell MET
    StatusCode       fillCellMet       (xAOD::MissingETContainer* metContainer,
                                        const CaloCellContainer* caloCellContainer ) const;
    // Fill Cluster MET
    StatusCode       fillClusterMet    (xAOD::MissingETContainer* metContainer,
                                        const xAOD::CaloClusterContainer* caloClusContainer) const;
    // Find MET term for a given sampling
    xAOD::MissingET* findMetTerm       (xAOD::MissingETContainer* metContainer, 
                                        CaloSampling::CaloSample sample) const;
    // CaloRegions enum - do NOT mess w/ the order
    enum CaloRegions {
      EMB = 0,
      EME,
      FCAL,
      HEC,
      PEMB,
      PEME,
      TILE,
      REGIONS_TOTAL
    };
    // CaloRegions human-readable names for enums
    const static std::string s_CaloRegionNames[];
    
  private:
    // Default constructor: 
    METCaloRegionsTool();

    #if defined(XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
    #else
    SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};
    #endif
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterKey{this,"CaloClusterKey","","SG Key of Calo Cluster Collection"};
    SG::ReadHandleKey<CaloCellContainer> m_caloCellKey{this,"CaloCellKey","","SG Key of Calo Cell Container"};
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METCALOREGIONSTOOL_H
