///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaAssociator.h 
// Header file for class METEgammaAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METEGAMMAASSOCIATOR_H
#define METRECONSTRUCTION_METEGAMMAASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"
#include "METRecoInterface/METRecoCommon.h"
#include "xAODEgamma/EgammaFwd.h"

#include "StoreGate/ReadDecorHandle.h"

#include <set>

namespace met{

  class METEgammaAssociator
    : virtual public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METEgammaAssociator, IMETAssocToolBase)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METEgammaAssociator(const std::string& name);
    virtual ~METEgammaAssociator();

    // AsgTool Hooks
    StatusCode initialize();
    StatusCode finalize();

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

    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
                                   std::vector<const xAOD::IParticle*>& tclist,
                                   const met::METAssociator::ConstitHolder& constits) const final;

    StatusCode extractPFO(const xAOD::IParticle* obj,
                          std::vector<const xAOD::IParticle*>& pfolist,
                          const met::METAssociator::ConstitHolder& constits,
                          std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final;

    StatusCode extractPFOsFromLinks(const xAOD::Egamma* eg,
    				    std::vector<const xAOD::IParticle*>& pfolist,
				    const met::METAssociator::ConstitHolder& constits) const;


    StatusCode extractPFOs(const xAOD::Egamma* eg,
				 std::vector<const xAOD::IParticle*>& pfolist,
				 const met::METAssociator::ConstitHolder& constits) const;

    StatusCode extractFE(const xAOD::IParticle* obj,
                         std::vector<const xAOD::IParticle*>& felist,
                         const met::METAssociator::ConstitHolder& constits,
                         std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final; //TODO: split in extractFEsFromLinks and extractFEs, similarly to PFOs, to use links

    StatusCode extractFEsFromLinks(const xAOD::Egamma* eg, //testFELinks
    				    std::vector<const xAOD::IParticle*>& felist,
				    const met::METAssociator::ConstitHolder& constits) const;


    StatusCode extractFEs(const xAOD::Egamma* eg, //testFELinks
				 std::vector<const xAOD::IParticle*>& felist,
				 const met::METAssociator::ConstitHolder& constits) const;


    StatusCode extractTracks(const xAOD::IParticle* obj,
                             std::vector<const xAOD::IParticle*>& constlist,
                             const met::METAssociator::ConstitHolder& constits) const final;

    StatusCode selectEgammaClusters(const xAOD::CaloCluster *swclus,
                                    const std::vector<const xAOD::IParticle*>& inputTC,
                                    std::vector<const xAOD::IParticle*>& tclist) const;

    StatusCode selectEgammaTracks(const xAOD::Egamma* el,
                                  const xAOD::TrackParticleContainer* trkCont,
                                  std::set<const xAOD::TrackParticle*>& tracklist) const;
 
    bool hasUnmatchedClusters(const xAOD::Egamma* eg, const xAOD::PFO* pfo) const;

    double m_tcMatch_dR;
    double m_tcMatch_maxRat;
    unsigned short m_tcMatch_method;

    double m_extraTrkMatch_dR;

    SG::ReadDecorHandleKey<xAOD::PhotonContainer> m_photonNeutralPFOReadDecorKey{this,"photonNeutralPFOReadDecorKey","", "Neutral PFO links key"};
    SG::ReadDecorHandleKey<xAOD::PhotonContainer> m_photonChargedPFOReadDecorKey{this,"photonChargedPFOReadDecorKey","", "Charged PFO links key"};
    SG::ReadDecorHandleKey<xAOD::PhotonContainer> m_photonNeutralFEReadDecorKey{this,"photonNeutralFEReadDecorKey","", "Neutral FE links key"};
    SG::ReadDecorHandleKey<xAOD::PhotonContainer> m_photonChargedFEReadDecorKey{this,"photonChargedFEReadDecorKey","", "Charged FE links key"};

    SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralPFOReadDecorKey{this,"electronNeutralPFOReadDecorKey","", "Neutral PFO links key"};
    SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_electronChargedPFOReadDecorKey{this,"electronCargedPFOReadDecorKey","", "Charged PFO links key"};
    SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralFEReadDecorKey{this,"electronNeutralFEReadDecorKey","", "Neutral FE links key"};
    SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_electronChargedFEReadDecorKey{this,"electronCargedFEReadDecorKey","", "Charged FE links key"};

    bool m_usePFOElectronLinks;
    bool m_usePFOPhotonLinks; 
    bool m_useFEElectronLinks;
    bool m_useFEPhotonLinks;
    bool m_checkUnmatched; 

    private:
 
    /// Default constructor: 
    METEgammaAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METEGAMMAASSOCIATOR_H

