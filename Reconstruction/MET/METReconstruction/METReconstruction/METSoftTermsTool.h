///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METSoftTermsTool.h 
// Header file for class METSoftTermsTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
// Note that this tool is now quite outdated and should be used only
// for basic monitoring purposes.
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

//#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace met{

  class METSoftTermsTool
    : public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METSoftTermsTool, IMETToolBase)

  public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METSoftTermsTool(const std::string& name);
    ~METSoftTermsTool() = default;

    // AsgTool Hooks
    virtual StatusCode initialize() override;

  protected:
    virtual
    StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const override;
    // Accept functions
    virtual
    bool accept            (const xAOD::IParticle* object) const override;
    bool accept            (const xAOD::CaloCluster* clus) const;
    bool accept            (const xAOD::TrackParticle* trk) const;
    // Overlap resolver function
    virtual
    bool resolveOverlap    (const xAOD::IParticle* object,
                            xAOD::MissingETComponentMap* metMap,
                            std::vector<const xAOD::IParticle*>& acceptedSignals,
                            MissingETBase::Types::weight_t& objWeight) const override;
    bool resolveOverlap    (xAOD::MissingETComponentMap* metMap,
                            std::vector<const xAOD::IParticle*>& acceptedSignals) const;

  private:
    // Default constructor: 
    METSoftTermsTool();

    // Use Case - Clusters OR Tracks OR PFOs
    Gaudi::Property<std::string> m_inputType{this, "InputComposition", "Clusters", ""};  // Options : Clusters (default) OR Tracks
    xAOD::Type::ObjectType m_st_objtype;
    // Cluster selection
    Gaudi::Property<bool> m_cl_vetoNegE{this, "VetoNegEClus", true, ""};
    Gaudi::Property<bool> m_cl_onlyNegE{this, "OnlyNegEClus", false, ""};
    SG::ReadHandleKey<xAOD::CaloClusterContainer>  m_caloClusterKey{this, "CaloClusterKey", "", "Input calo cluster container name (not to be configured manually)"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_trackParticleKey{this, "TrackKey", "", "Input track container name (not to be configured manually)"};
  }; 

}

#endif //> !METRECONSTRUCTION_METSOFTTERMSTOOL_H
