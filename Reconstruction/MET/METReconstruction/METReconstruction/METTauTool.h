///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTauTool.h 
// Header file for class METTauTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTAUTOOL_H
#define METRECONSTRUCTION_METTAUTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

// Forward declaration

namespace met {

  class METTauTool
    : public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTauTool, IMETToolBase)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METTauTool(const std::string& name);
    ~METTauTool();

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
    bool accept(const xAOD::IParticle* object) const;
    bool resolveOverlap(const xAOD::IParticle* object,
			xAOD::MissingETComponentMap* metMap,
			std::vector<const xAOD::IParticle*>& acceptedSignals,
			MissingETBase::Types::weight_t& objWeight) const;

  private:
    double m_tau_minPt;
    double m_tau_maxEta;
    // configurable parameters for PID
    unsigned int m_tau_isTauFlag;
    bool m_tau_doElVeto;
    unsigned int m_tau_elVeto;
    bool m_tau_doMuVeto;
    unsigned int m_tau_muVeto;
    // minimum non-overlapping E fraction to retain jet calibration
    // could become a property of METBuilderTool if necessary
    // or we could apply a min pt cut instead
    double m_tau_minWet;
    bool m_tau_doMinWetPtCut;
    // do track OR
    bool m_tau_doTracks;

    /// Default constructor: 
    METTauTool();
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METTAUTOOL_H
