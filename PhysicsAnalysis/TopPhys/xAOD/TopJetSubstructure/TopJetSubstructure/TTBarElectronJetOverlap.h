/*
    Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TTBARELECTRONJETOVERLAP_H__
#define __TTBARELECTRONJETOVERLAP_H__

#include <vector>
#include <set>
#include <utility>
#include "TLorentzVector.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"

class TTBarElectronJetOverlap {
    private:
        std::vector<TLorentzVector> fJetTLVs; // all jet 4-vectors
        std::vector<TLorentzVector> fOrigJetTLVs; // original jet 4-vectors
        std::vector<bool> fGoodJets; // flags for jets passing overlap removal
        std::vector<int> fSubJets; // flags for jets that undergo subtraction
        std::vector<std::set<int> > fJetAssocElCls; // for each jet, filled with associated electron indexes (even more than one, -1 if no associated electron)

        std::vector<TLorentzVector> fElTLVs; // all electron 4-vectors (track-based direction)
        std::vector<TLorentzVector> fElClTLVs; // all electron 4-vectors (calo-based direction)
        std::vector<bool> fElGood; // this registers if the electron passed the object definition (before this overlap removal)
        std::vector<bool> fGoodEls; // flags for electrons passing overalp removal
        std::vector<int> fElClAssocJet; // for each electron, filled with associated jet index (-1 otherwise)

        void FindAssocEls();
        void SubtractEls();
        void FindGoodObjects(float pt_cut_value, float pt_bias_value);

        xAOD::JetContainer *m_jets; //!
        xAOD::ElectronContainer *m_electrons; //!


    public:
        TTBarElectronJetOverlap() : m_jets(nullptr), m_electrons(nullptr)
        {
        }

        ~TTBarElectronJetOverlap() {
        }

        void AnalyzeEvent(float pt_cut_value, float pt_bias_value, const std::string &leptonDef);

        // Load all jets in the event (with variables corresponding to the *corrected* jet quantities)
        // Load selected electrons (only *selected* electrons - i.e. pass ID and isolation cuts)
        void Load(xAOD::JetContainer *jets, xAOD::ElectronContainer *electrons, const std::string &leptonDef);

};

#endif
