///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRefinerTool.h 
// Header file for class METRefinerTool
//
// This is the base class for tools that modify MET terms
// created by METBuilderTools
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METREFINERTOOL_H
#define METRECONSTRUCTION_METREFINERTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METRecoInterface includes
#include "METRecoInterface/IMETToolBase.h"

// Forward declaration
namespace xAOD {
  class IParticle;
}

namespace met {
  class METRefinerTool
    : virtual public asg::AsgTool,
      virtual public IMETToolBase
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METRefinerTool, IMETToolBase)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METRefinerTool(const std::string& name);
    virtual ~METRefinerTool();

    // AsgTool Hooks
    virtual StatusCode initialize();
    virtual StatusCode execute(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;
    virtual StatusCode finalize();

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

    std::string m_output_met_key;
    bool m_useRapidity;  // by default, use pseudorapidity for matching

    // reconstruction process to be defined in the individual tools
    // pure virtual -- we have no default
    virtual StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const = 0;

  private:
    /// Default constructor:
    METRefinerTool();

  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> !METRECONSTRUCTION_METREFINERTOOL_H
