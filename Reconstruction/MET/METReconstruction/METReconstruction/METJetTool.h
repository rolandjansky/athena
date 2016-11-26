///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METJetTool.h 
// Header file for class METJetTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METJETTOOL_H
#define METRECONSTRUCTION_METJETTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

// Forward declaration

namespace met{

  class METJetTool
    : public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METJetTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METJetTool(const std::string& name);
    ~METJetTool();

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
    double m_jet_minPt;
    double m_jet_maxEta;
    // permit to select jets based on some object quality
    bool m_jet_doQualCut;
    std::string m_jet_badQual;
    // minimum non-overlapping E fraction to retain jet calibration
    // could become a property of METBuilderTool if necessary
    // or we could apply a min pt cut instead
    double m_jet_minWet;
    bool   m_jet_doMinWetPtCut;
    // do track OR
    bool m_jet_doTracks;

    /// Default constructor: 
    METJetTool();

  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METJETTOOL_H
