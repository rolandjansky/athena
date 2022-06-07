///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METAssociationTool.h 
// Header file for class METAssociationTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METASSOCIATIONTOOL_H
#define METRECONSTRUCTION_METASSOCIATIONTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "CxxUtils/checker_macros.h"
#include "StoreGate/DataHandle.h"


// METRecoInterface includes
#include "METRecoInterface/IMETRecoTool.h"
#include "METRecoInterface/IMETAssocToolBase.h"

// EDM includes
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAuxAssociationMap.h"

// Timing
#include "TStopwatch.h"

// Forward declaration

namespace met{
  /* @brief Top-level class scheduling MET reconstruction tools
   *
   *  @author  Peter Loch <loch_AT_physics.arizona.edu>
   *  @author  Silvia Resconi <Silvia.Resconi_AT_cern.ch>
   *  @author  Teng Jian Khoo <Teng.Jian.Khoo_AT_cern.ch>
   *  
   *  @date    Nov. 21, 2013
   *  @version v1.0
   *
   *  The METAssociationTool can be called within an Athena algorithm or a ROOT programme
   *  to carry out the reconstruction of a MET configuration, and will produce a
   *  MissingETContainer, the contents of which will be the MissingET objects produced
   *  by the various tools scheduled by METAssociationTool.
   *
   */
  class ATLAS_NOT_THREAD_SAFE METAssociationTool final
  //    ^ the (optional) use of TStopwatch makes this not thread-safe
    : public asg::AsgTool,
      virtual public IMETRecoTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METAssociationTool, IMETRecoTool)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METAssociationTool(const std::string& name);

    // AsgTool Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute() const override;
    virtual StatusCode finalize() override;

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private: 

    /// Default constructor: 
    METAssociationTool();

    // Run the MET tools here
    StatusCode buildMET(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const;

    // Data members
    Gaudi::Property<std::string> m_metSuffix{this, "METSuffix", "AntiKt4LCTopo", "MET suffix"};
    SG::WriteHandleKey<xAOD::MissingETContainer> m_coreKey{this, "CoreOutputKey", "", ""};
    SG::WriteHandleKey<xAOD::MissingETAssociationMap> m_mapKey{this, "AssociationOutputKey", "", ""};

    ToolHandleArray<IMETAssocToolBase> m_metAssociators{this, "METAssociators", {}, ""};

    // Monitor timing
    Gaudi::Property<int> m_timeDetail{this, "TimingDetail", 0, ""};

    mutable std::atomic<unsigned int> m_nevt;
    mutable TStopwatch m_clock;
    mutable std::vector<TStopwatch> m_toolClocks;
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METASSOCIATIONTOOL_H
