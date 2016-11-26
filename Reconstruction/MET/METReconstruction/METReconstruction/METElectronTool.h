///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METElectronTool.h 
// Header file for class METElectronTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METELECTRONTOOL_H
#define METRECONSTRUCTION_METELECTRONTOOL_H 1

// METReconstruction includes
#include "METReconstruction/METEgammaTool.h"

// Egamma EDM
#include "xAODEgamma/ElectronFwd.h"

namespace met{

  class METElectronTool
    : public METEgammaTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METElectronTool, IMETToolBase)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METElectronTool(const std::string& name);
    ~METElectronTool();

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
    StatusCode executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;
    bool resolveOverlap(const xAOD::IParticle* object,
			xAOD::MissingETComponentMap* metMap,
			std::vector<const xAOD::IParticle*>& acceptedSignals,
			MissingETBase::Types::weight_t& objWeight) const;


  private:

    /// Default constructor: 
    METElectronTool();

    // Identify tracks that may contribute to the electron SW cluster
    // or are associated to the electron for some other reason
    // and hence should not enter later sums
    void matchTracks(const xAOD::Electron* el, std::vector<const xAOD::IParticle*>& trklist) const;

  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> !METRECONSTRUCTION_METELECTRONTOOL_H
