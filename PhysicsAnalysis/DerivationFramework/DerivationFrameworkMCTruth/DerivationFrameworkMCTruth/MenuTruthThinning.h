/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_MENUTRUTHTHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_MENUTRUTHTHINNINGTOOL_H

#include <string>
#include <unordered_set>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"

class IThinningSvc;

namespace DerivationFramework {

    class MenuTruthThinning : public AthAlgTool, public IThinningTool {
    public:
        MenuTruthThinning(const std::string& t, const std::string& n, const IInterface* p);
        ~MenuTruthThinning();
        StatusCode initialize();
        StatusCode finalize();
        virtual StatusCode doThinning() const;
        bool isAccepted(const xAOD::TruthParticle*) const;
        bool matchHadronIncTau(const xAOD::TruthParticle* part) const;
        bool matchQuarkIncTau(const xAOD::TruthParticle* part) const;
        bool isOrphanIncTau(const xAOD::TruthParticle* part) const;
        bool matchGenParticle(const xAOD::TruthParticle* part,
                              std::vector<int> &targetIDs, std::vector<int> &intermediateIDs,
                              bool targetsAreRange) const;
        bool isLeptonFromTau(const xAOD::TruthParticle*) const;
        bool isFromTau(const xAOD::TruthParticle*) const;
        bool isBSM(const xAOD::TruthParticle*) const;
        bool isttHFHadron(const xAOD::TruthParticle*) const;
        bool isBoson(const xAOD::TruthParticle*) const;
        bool isFsrFromLepton(const xAOD::TruthParticle*) const;


    private:
        // THE MENU

        /// Names of the truth container?
        std::string m_particlesKey;
        std::string m_verticesKey;
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

        /// for keeping trace of barcodes in order to detect loops
        mutable std::unordered_set<int> m_barcode_trace;

        // counters
        mutable unsigned int m_totpart;
        mutable unsigned int m_removedpart;
        mutable int m_particleCountSoFar;
        mutable int m_eventCount;

        /// Parameter: simulation barcode offset
        int m_geantOffset;

        // handle to the thinning service
        ServiceHandle<IThinningSvc> m_thinningSvc;
    };
}

#endif // DERIVATIONFRAMEWORK_MENUTRUTHTHINNING_H
