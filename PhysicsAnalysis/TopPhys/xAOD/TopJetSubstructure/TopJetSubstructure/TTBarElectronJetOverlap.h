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
        std::vector<TLorentzVector> fJetTLVs;
        std::vector<float> fJetJVFs;
        std::vector<float> fJetD3PDTrkPtPVSums;
        std::vector<float> fJetD3PDTrkPtSums;

	std::vector<bool> fGoodJets;
	std::vector<int> fSubJets;
	std::vector<std::set<int> > fJetAssocElCls;

        std::vector<TLorentzVector> fElTLVs;
	std::vector<TLorentzVector> fOrigJetTLVs;
        std::vector<TLorentzVector> fElClTLVs;
        std::vector<bool> fElGood; // this registers if the electron passed the object definition

        std::vector<bool> fGoodEls;
        std::vector<int> fElClAssocJet;

        bool fDebug;

        void FindAssocEls();
        void SubtractEls();
        void FindGoodObjects();
        void RecalcJVF();


        xAOD::JetContainer *m_jets; //!
        xAOD::ElectronContainer *m_electrons; //!

    public:
        TTBarElectronJetOverlap() : fDebug(false), m_jets(nullptr), m_electrons(nullptr) {
        }

        ~TTBarElectronJetOverlap() { }

        void AnalyzeEvent(const std::string &leptonDef);

        // sets the debug level (info will print if set true).
        void SetDebug(bool db) {
            fDebug = db;
        }

        bool GetDebug() {
            return fDebug;
        }

        // load all anti-kt 0.4 LCTopo jets in the event.
        // these variables should correspond to the *corrected* jet
        // quantities. Every jet in the D3PD should be passed.
        // load selected electrons.
        // these variables should only be filled for /selected/
        // electrons (i.e. pass ID and isolation cuts).
        void Load(xAOD::JetContainer *jets, xAOD::ElectronContainer *electrons, const std::string &leptonDef);

};

#endif
