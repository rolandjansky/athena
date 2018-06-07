///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AsgTools/ToolHandle.h"

class ICaloClusterMatchingTool;

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
    ~METMuonAssociator();

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
    
    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const final;
    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
				   std::vector<const xAOD::IParticle*>& tclist,
				   const met::METAssociator::ConstitHolder& constits) const final;
    StatusCode extractPFO(const xAOD::IParticle* obj,
			  std::vector<const xAOD::IParticle*>& pfolist,
			  const met::METAssociator::ConstitHolder& constits,
			  std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final;
    StatusCode extractTracks(const xAOD::IParticle* obj,
			     std::vector<const xAOD::IParticle*>& constlist,
			     const met::METAssociator::ConstitHolder& constits) const final;

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

    private:

    void deltaPhi(float phi1, float phi2, float& result) const;
    void deltaR(float eta1, float phi1, float eta2, float phi2, float& result) const;

    const float m_Drcone = 0.2;       // Cone size for mu-pfo association
    const float m_MinDistCone = 0.4;  // Cone size for getting random Phi of PFO which is not assoc to el or HR        

    bool m_doMuonClusterMatch;

    /// Default constructor: 
    METMuonAssociator();    

  }; 

}

#endif //> !METRECONSTRUCTION_METMUONASSOCIATOR_H
