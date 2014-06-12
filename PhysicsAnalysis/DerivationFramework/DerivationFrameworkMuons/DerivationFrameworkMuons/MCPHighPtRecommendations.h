/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MCPInDetRecommendations.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MCPHIGHPTRECOMMENDATIONS_H
#define DERIVATIONFRAMEWORK_MCPHIGHPTRECOMMENDATIONS_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {
    
    /** @class MCPHighPtRecommendations
     
     the code used in this implementation is kindly stolen from:
     atlasoff:: ISF/ISF_Core/ISF_Tools
     
     @author James Catmore -at- cern.ch
     */
    class MCPHighPtRecommendations : public AthAlgTool, public IAugmentationTool {
        
    public:
        /** Constructor with parameters */
        MCPHighPtRecommendations( const std::string& t, const std::string& n, const IInterface* p );
        
        /** Destructor */
        ~MCPHighPtRecommendations();
        
        // Athena algtool's Hooks
        StatusCode  initialize();
        StatusCode  finalize();
        
        /** Check that the current event passes this filter */
        virtual StatusCode addBranches() const;
        
    private:
        // inline functions
        StatusCode retrieveFromStoreGate(void) const;
        float calculateSumPt(float,float,float) const;
        float getSigDiffQOverP(unsigned int) const;
        bool muonPassesBasicSelection(unsigned int,float,float,float) const;
        bool muonPasses3StHitReqs(unsigned int, float) const;
        bool muonPasses2StHitReqs(unsigned int, bool, float) const;
        int getPhiSector(float) const;
        
        // Variables set by the user
        float m_ipD03st;
        float m_ipZ03st;
        float m_trkIso3st;
        float m_ipD02st;
        float m_ipZ02st;
        float m_eta2st;
        float m_trkIso2st;
        std::string m_sgKey3st;
        std::string m_sgKey2st;
        std::string m_collName;
        
        // Variables coming in from StoreGate (D3PD branches)
        std::vector<int>*   m_mu_isCombinedMuon;
        std::vector<float>* m_mu_pt;
        std::vector<float>* m_mu_eta;
        std::vector<float>* m_mu_qoverp_exPV;
        std::vector<float>* m_mu_phi_exPV;
        std::vector<float>* m_mu_id_d0_exPV;
        std::vector<float>* m_mu_id_z0_exPV;
        std::vector<float>* m_mu_id_qoverp_exPV;
        std::vector<float>* m_mu_me_qoverp_exPV;
        std::vector<float>* m_trk_pt;
        std::vector<float>* m_trk_eta;
        std::vector<float>* m_trk_phi_wrtPV;
        std::vector<float>* m_mu_phi;
        std::vector<int>*   m_mu_nMDTBIHits;
        std::vector<int>*   m_mu_nMDTBMHits;
        std::vector<int>*   m_mu_nMDTBOHits;
        std::vector<int>*   m_mu_nMDTEIHits;
        std::vector<int>*   m_mu_nCSCEtaHits;
        std::vector<int>*   m_mu_nMDTEMHits;
        std::vector<int>*   m_mu_nMDTEOHits;
        std::vector<int>*   m_mu_nMDTEEHits;
        std::vector<int>* m_mu_nRPCLayer1PhiHits;
        std::vector<int>* m_mu_nRPCLayer2PhiHits;
        std::vector<int>* m_mu_nRPCLayer3PhiHits;
        std::vector<int>* m_mu_nMDTBIS78Hits;
        std::vector<int>* m_mu_nTGCLayer1PhiHits;
        std::vector<int>* m_mu_nTGCLayer2PhiHits;
        std::vector<int>* m_mu_nTGCLayer3PhiHits;
        std::vector<int>* m_mu_nTGCLayer4PhiHits;
        std::vector<int>* m_mu_nCSCPhiHits;
        std::vector<int>* m_mu_nCSCUnspoiledEtaHits;
        std::vector<int>* m_mu_nMDTBEEHits;
        std::vector<float>* m_mu_me_cov_qoverp_exPV;
        std::vector<float>* m_mu_id_cov_qoverp_exPV;
        std::vector<float>* m_mu_ptcone30;
        
        unsigned int m_nRequired;
        mutable unsigned int m_npass, m_ntot;
                
    };

	
	
    
}


#endif
