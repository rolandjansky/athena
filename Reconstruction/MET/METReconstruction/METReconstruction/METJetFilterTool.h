///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METJetFilterTool.h 
// Header file for class METJetFilterTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METJETFILTERTOOL_H
#define METRECONSTRUCTION_METJETFILTERTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METRefinerTool.h"

// Forward declaration
#include "xAODJet/Jet.h"

namespace met{

  class METJetFilterTool
    : public METRefinerTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METJetFilterTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METJetFilterTool(const std::string& name);
    ~METJetFilterTool();

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
    // Accept functions
    bool isHSJet         (const xAOD::Jet* jet) const;

  private:
    // Default constructor: 
    METJetFilterTool();

    bool m_jet_doJVFCut;
    double m_jet_minAbsJVF;
    double m_jet_maxPtJVF;
    double m_jet_maxEtaJVF;
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METJETFILTERTOOL_H
