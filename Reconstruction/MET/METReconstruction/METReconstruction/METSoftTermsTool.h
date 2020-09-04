///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSoftTermsTool.h 
// Header file for class METSoftTermsTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METSOFTTERMSTOOL_H
#define METRECONSTRUCTION_METSOFTTERMSTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

//Framework includes
#include "StoreGate/DataHandle.h"

// Forward declaration
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

// PFlow EDM and helpers
#include "xAODPFlow/PFOContainer.h"
#include "PFlowUtils/IRetrievePFOTool.h"


namespace met{

  class METSoftTermsTool
    : public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METSoftTermsTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METSoftTermsTool(const std::string& name);
    ~METSoftTermsTool();

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
    bool accept            (const xAOD::IParticle* object) const;
    bool accept            (const xAOD::CaloCluster* clus) const;
    bool accept            (const xAOD::TrackParticle* trk) const;
    bool accept            (const xAOD::PFO* pfo, const xAOD::Vertex* pv) const;
    // Overlap resolver function
    bool resolveOverlap    (const xAOD::IParticle* object,
                            xAOD::MissingETComponentMap* metMap,
                            std::vector<const xAOD::IParticle*>& acceptedSignals,
                            MissingETBase::Types::weight_t& objWeight) const;
    bool resolveOverlap    (xAOD::MissingETComponentMap* metMap,
                            std::vector<const xAOD::IParticle*>& acceptedSignals) const;

  private:
    // Default constructor: 
    METSoftTermsTool();
    // Use Case - Clusters OR Tracks OR PFOs
    std::string m_inputType;
    unsigned short m_st_objtype{0}; // should make this an enum somewhere
    // Cluster selection
    bool m_cl_vetoNegE;
    bool m_cl_onlyNegE;
    // temporary, until a track-vertex association tool is available
    //std::string m_pv_inputkey;
    SG::ReadHandleKey<xAOD::VertexContainer>  m_pv_inputkey{this,"InputPVKey","PrimaryVertices",""};
    SG::ReadHandleKey<xAOD::CaloClusterContainer>  m_caloClusterKey{""};
    SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_trackParticleKey{""};

    ToolHandle<CP::IRetrievePFOTool> m_pfotool{this,"PFOTool","",""};
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METSOFTTERMSTOOL_H
