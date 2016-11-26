///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonElossTool.h 
// Header file for class METMuonElossTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METMUONELOSSTOOL_H
#define METRECONSTRUCTION_METMUONELOSSTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METRefinerTool.h"

// Forward declaration
#include "xAODJet/Jet.h"

namespace met{

  class METMuonElossTool
    : public METRefinerTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METMuonElossTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METMuonElossTool(const std::string& name);
    ~METMuonElossTool();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();

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
    StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;

  private:
    // Default constructor: 
    METMuonElossTool();
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METMUONELOSSTOOL_H
