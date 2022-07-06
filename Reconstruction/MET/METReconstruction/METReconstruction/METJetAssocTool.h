///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METJetAssocTool.h 
// Header file for class METJetAssocTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METJETASSOCTOOL_H
#define METRECONSTRUCTION_METJETASSOCTOOL_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

//Includes for DataHandles
#include "StoreGate/DataHandle.h"

namespace met{
  class METJetAssocTool final
    : public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METJetAssocTool, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METJetAssocTool(const std::string& name);
    ~METJetAssocTool() = default;

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
    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const override;

    virtual
    StatusCode extractPFO(const xAOD::IParticle*,
                          std::vector<const xAOD::IParticle*>&,
                          const met::METAssociator::ConstitHolder&,
                          std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t>&) const override
    {return StatusCode::FAILURE;} // should not be called
    virtual
    StatusCode extractFE(const xAOD::IParticle*,
                         std::vector<const xAOD::IParticle*>&,
                         const met::METAssociator::ConstitHolder&,
                         std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t>&) const override
    {return StatusCode::FAILURE;} // should not be called
    virtual
    StatusCode extractTracks(const xAOD::IParticle*,
                             std::vector<const xAOD::IParticle*>&,
                             const met::METAssociator::ConstitHolder&) const override
    {return StatusCode::FAILURE;} // should not be called
    virtual
    StatusCode extractTopoClusters(const xAOD::IParticle*,
                                   std::vector<const xAOD::IParticle*>&,
                                   const met::METAssociator::ConstitHolder&) const override
    {return StatusCode::FAILURE;} // should not be called

    private:
 
    /// Default constructor: 
    METJetAssocTool();
    SG::ReadHandleKey<xAOD::JetContainer> m_jetContKey{this, "InputCollection", "AntiKt4EMPFlowJets", "jets input key"};
    void getPFOs(const xAOD::Jet *jet,
                 std::vector<const xAOD::IParticle*> &consts,
                 const met::METAssociator::ConstitHolder& constits,
                 std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const;
    void getClus(const xAOD::Jet *jet,std::vector<const xAOD::IParticle*> &consts) const;
    void getOther(const xAOD::Jet *jet,
                  std::vector<const xAOD::IParticle*> &consts,
                  std::set<const xAOD::IParticle*> *newConst) const;

    Gaudi::Property<double> m_matchRadius{this, "MatchRadius", 0.4, ""};
  }; 

}

#endif //> !METRECONSTRUCTION_METJETASSOCTOOL_H
