///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METTruthTool.h 
// Header file for class METTruthTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTRUTHTOOL_H
#define METRECONSTRUCTION_METTRUTHTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

#include "xAODTruth/TruthEventContainer.h"

// Truth EDM
#include "xAODTruth/TruthParticleFwd.h"

namespace met{

  class METTruthTool
    : public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTruthTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METTruthTool(const std::string& name);
    ~METTruthTool() = default;

    // AsgTool Hooks
    virtual StatusCode initialize() override;

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
    virtual
    StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const override;
    // Accept functions
    virtual
    bool accept            (const xAOD::IParticle* object) const override;
    bool accept_nonint     (const xAOD::TruthParticle* truth) const;
    bool accept_int        (const xAOD::TruthParticle* truth) const;
    bool accept_intout     (const xAOD::TruthParticle* truth) const;
    bool accept_intmuons   (const xAOD::TruthParticle* truth) const;
    // Overlap resolver function
    virtual
    bool resolveOverlap    (const xAOD::IParticle*,
                            xAOD::MissingETComponentMap*,
                            std::vector<const xAOD::IParticle*>&,
                            MissingETBase::Types::weight_t&) const override { return true;};

  private:
    // Default constructor: 
    METTruthTool();

    Gaudi::Property<std::string> m_inputType{this, "InputComposition", "NonInt", "truth type"}; // NonInt, Int, IntMuons, IntOut
    MissingETBase::Types::bitmask_t m_truth_type{};

    Gaudi::Property<double> m_det_maxEta{this, "MaxEtaDet", 5., "nominal max detector eta"};
    Gaudi::Property<double> m_truthmu_minPt{this, "MinPtMu", 6e3, "nominal min muon pt"};
    Gaudi::Property<double> m_truthmu_maxEta{this, "MaxEtaMu", 2.7, "nominal max MS eta"};
    SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventKey{this, "InputCollection", "TruthEvents", "truth events input collection"};

    // TEMPORARILY recopy some helper from TruthHelper and GeneratorUtils packages
    //  *** via JetSimTools ***
    // We'll have to use this package when they work properly with xAOD.
  };

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METTRUTHTOOL_H
