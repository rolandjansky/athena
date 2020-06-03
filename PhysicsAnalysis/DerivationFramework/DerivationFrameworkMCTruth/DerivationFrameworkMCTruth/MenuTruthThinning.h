/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MenuTruthThinning, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MENUTRUTHTHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_MENUTRUTHTHINNINGTOOL_H

#include <string>
#include <unordered_set>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"


namespace DerivationFramework {
    
    class MenuTruthThinning : public extends<AthAlgTool, IThinningTool> {
    public:
        MenuTruthThinning(const std::string& t, const std::string& n, const IInterface* p);
        virtual ~MenuTruthThinning();
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
        virtual StatusCode doThinning() const override;

        bool isAccepted(const xAOD::TruthParticle*) const;
        bool matchHadronIncTau(const xAOD::TruthParticle* part) const;
        bool matchQuarkIncTau(const xAOD::TruthParticle* part) const;
        bool isOrphanIncTau(const xAOD::TruthParticle* part) const;
        bool matchGenParticle(const xAOD::TruthParticle* part,
                              std::vector<int> &targetIDs, std::vector<int> &intermediateIDs,
                              bool targetsAreRange) const;
        bool isLeptonFromTau(const xAOD::TruthParticle*) const;
        bool isFromTau(const xAOD::TruthParticle*,
                       std::unordered_set<int>& barcode_trace) const;
        bool isBSM(const xAOD::TruthParticle*) const;
        bool isttHFHadron(const xAOD::TruthParticle*) const;
        bool isBoson(const xAOD::TruthParticle*) const;
        bool isFsrFromLepton(const xAOD::TruthParticle*) const;
	bool parentIsLongLived(const xAOD::TruthParticle*) const;


    private:
        // THE MENU
        
        StringProperty m_streamName
          { this, "StreamName", "", "Name of the stream being thinned" };
        SG::ThinningHandleKey<xAOD::TruthParticleContainer> m_particlesKey
          { this, "ParticlesKey", "TruthParticles", "TruthParticle container name" };
        SG::ThinningHandleKey<xAOD::TruthVertexContainer> m_verticesKey
          { this, "VerticesKey", "TruthVertices", "TruthVertex container name" };

        std::string m_eventsKey;
        
        /// Parameter: Keep partons?
        bool m_writePartons;
        
        /// Parameter: Keep hadrons?
        bool m_writeHadrons;
        
        /// Parameter: Keep b-hadrons?
        bool m_writeBHadrons;

        /// Parameter: Keep c-hadrons?
        bool m_writeCHadrons;

        /// Parameter: Keep geant particles?
        bool m_writeGeant;
        
        /// Parameter: Write Geant photons with Pt above this threshold.
        /// Set to < 0 to not write any.
        float m_geantPhotonPtThresh;
        
        /// Parameter: Keep hadronic tau decays?
        bool m_writeTauHad;
        
        /// Parameter: Keep BSM particles?
        bool m_writeBSM;
        
        /// Parameter: Keep bosons?
        bool m_writeBosons;
        float m_photonPtCut;
        
        /// Parameter: Write partons with Pt above this threshold.
        float m_partonPtThresh;
        
        /// Parameter: Write absolutely everything
        bool m_writeEverything;
        
        /// Parameter: Write boson decay products
        bool m_writeBosonProducts;
        
        /// Parameter: Write BSM decay products
        bool m_writeBSMProducts;
        
        /// Parameter: Write top and decay products
        bool m_writeTopAndDecays;
        
        /// Parameter: Write all leptons
        bool m_writeAllLeptons;
        
        /// Parameter: Write all leptons
        bool m_writeLeptonsNotFromHadrons;
        
        //// Parameter: write all stable particles
        bool m_writeAllStable;
        
        /// Parameter: Write particles with status code 3
        bool m_writeStatus3;
        
        /// Parameter: Write particles for tt+HF classification
        bool m_writettHFHadrons;
        
        /// Parameter: First N particles to write
        int m_writeFirstN;
        
        /// Parameter: preserve descendant/ancestor graph completeness
        bool m_preserveDescendants;
        bool m_preserveGeneratorDescendants;
        bool m_preserveAncestors;
        bool m_preserveImmediate; // parents, siblings, children only
        bool m_preserveHadVtx; // hadronization vertices for above

        /// Parameter: List of PDG IDs to always keep
        std::vector<int> m_pdgIdsToKeep;

	/// Parameter: List of PDG IDs of long lived particles so that one can keep their children	
	std::vector<int> m_longLivedPdgIds;

        /// for keeping trace of barcodes in order to detect loops
        mutable std::unordered_set<int> m_barcode_trace;

        // counters
        mutable std::atomic<unsigned int> m_totpart;
        mutable std::atomic<unsigned int> m_removedpart;
        mutable std::atomic<int> m_eventCount;
        

        /// Parameter: simulation barcode offset
        int m_geantOffset;
    }; 
}

#endif // DERIVATIONFRAMEWORK_MENUTRUTHTHINNING_H
