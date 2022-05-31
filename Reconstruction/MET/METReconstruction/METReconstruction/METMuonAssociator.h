///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METMuonAssociator.h 
// Header file for class METMuonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METMUONASSOCIATOR_H
#define METRECONSTRUCTION_METMUONASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace met{
  class METMuonAssociator final
    : public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METMuonAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METMuonAssociator(const std::string& name);
    ~METMuonAssociator() = default;

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
    
    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const final;
    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
                                   std::vector<const xAOD::IParticle*>& tclist,
                                   const met::METAssociator::ConstitHolder& constits) const final;
    StatusCode extractPFO(const xAOD::IParticle* obj,
                          std::vector<const xAOD::IParticle*>& pfolist,
                          const met::METAssociator::ConstitHolder& constits,
                          std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final;

    StatusCode extractFE(const xAOD::IParticle* obj,
                         std::vector<const xAOD::IParticle*>& felist,
                         const met::METAssociator::ConstitHolder& constits,
                         std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final; 

    StatusCode extractFEsFromLinks(const xAOD::Muon* mu, //TODO
    				    std::vector<const xAOD::IParticle*>& felist,
				    const met::METAssociator::ConstitHolder& constits) const;

    StatusCode extractFEs(const xAOD::Muon* mu, 
				 std::vector<const xAOD::IParticle*>& felist,
				 const met::METAssociator::ConstitHolder& constits) const;

    StatusCode extractTracks(const xAOD::IParticle* obj,
                             std::vector<const xAOD::IParticle*>& constlist,
                             const met::METAssociator::ConstitHolder& constits) const final;

    private:

    Gaudi::Property<bool> m_doMuonClusterMatch{this, "DoClusterMatch", true, ""};

    /// Default constructor: 
    METMuonAssociator();

    SG::ReadHandleKey<xAOD::MuonContainer> m_muContKey{
      this, 
      "InputCollection",
      "Muons",
      "muons input key"
    };
    SG::ReadDecorHandleKey<xAOD::MuonContainer> m_neutralFEReadDecorKey{
      this,
      "NeutralFEReadDecorKey",
      "",
      "Neutral FlowElement links key"
    };
    SG::ReadDecorHandleKey<xAOD::MuonContainer> m_chargedFEReadDecorKey{
      this,
      "ChargedFEReadDecorKey",
      "",
      "Charged FlowElement links key"
    };
    SG::ReadDecorHandleKey<xAOD::CaloClusterContainer> m_elementLinkName{
      this,
      "ElementLinkName",
      "MuonClusterCollection.constituentClusterLinks"
    };
  };
}

#endif //> !METRECONSTRUCTION_METMUONASSOCIATOR_H
