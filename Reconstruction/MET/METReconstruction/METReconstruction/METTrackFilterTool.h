///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTrackFilterTool.h 
// Header file for class METTrackFilterTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTRACKFILTERTOOL_H
#define METRECONSTRUCTION_METTRACKFILTERTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METRefinerTool.h"

// Forward declaration
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

namespace met{

  class METTrackFilterTool
    : virtual public METRefinerTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTrackFilterTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METTrackFilterTool(const std::string& name);
    ~METTrackFilterTool();

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
    StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap);
    // Accept functions
    bool isPVTrack         (const xAOD::TrackParticle* trk, const xAOD::Vertex* pv) const;
    bool isGoodEoverP      (const xAOD::TrackParticle* trk,
			    const std::vector<const xAOD::IParticle*>& trkList,
			    const xAOD::CaloClusterContainer* clusters) const;

  private:
    // Default constructor: 
    METTrackFilterTool();

    bool m_trk_doPVsel;
    double m_trk_d0Max;
    double m_trk_z0Max;
    std::string m_pv_inputkey;
    const xAOD::VertexContainer* m_pv_cont;

    bool m_trk_doEoverPsel;
    std::string m_cl_inputkey;
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METTRACKFILTERTOOL_H
