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
#include "METReconstruction/METRecoCommon.h"
#include "xAODEgamma/EgammaFwd.h"

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

    StatusCode GetPFOWana(const xAOD::IParticle* obj,
                         std::vector<const xAOD::IParticle*>& pfolist,
                         const met::METAssociator::ConstitHolder& constits,
                         std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta,
                         std::vector<double>& vPhiRnd,
                         unsigned int& lept_count,
                         float& UEcorr) const final;

    StatusCode hadrecoil_PFO(std::vector<const xAOD::IParticle*> hardObjs, 
                             const met::METAssociator::ConstitHolder& constits, 
                             TLorentzVector& HR,
                             std::vector<double>& vPhiRnd) const final;    

    StatusCode extractTracks(const xAOD::IParticle* obj,
			     std::vector<const xAOD::IParticle*>& constlist,
			     const met::METAssociator::ConstitHolder& constits) const final;

    StatusCode selectEgammaClusters(const xAOD::CaloCluster *swclus,
				    const std::vector<const xAOD::IParticle*>& inputTC,
				    std::vector<const xAOD::IParticle*>& tclist) const;

    StatusCode selectEgammaTracks(const xAOD::Egamma* el,
				  const xAOD::TrackParticleContainer* trkCont,
				  std::set<const xAOD::TrackParticle*>& tracklist) const;

    double m_tcMatch_dR;
    double m_tcMatch_maxRat;
    unsigned short m_tcMatch_method;

    double m_extraTrkMatch_dR;

    private:

    void deltaPhi(float phi1, float phi2, float& result) const;
    void deltaR(float eta1, float phi1, float eta2, float phi2, float& result) const;

    const float m_Drcone = 0.2;       // Cone size for el-pfo association
    const float m_MinDistCone = 0.4;  // Cone size for getting random Phi of PFO which is not assoc to el or HR
 
    /// Default constructor: 
    METEgammaAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METEGAMMAASSOCIATOR_H
