/*
    Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopJetSubstructure/TTBarElectronJetOverlap.h"
#include <iostream>
#include <cmath>
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"

void TTBarElectronJetOverlap::Load(xAOD::JetContainer *jets, xAOD::ElectronContainer *electrons, const std::string &leptonDef) {
    m_jets = jets;
    m_electrons = electrons;

    size_t se = m_electrons->size();
    fElClTLVs.resize(se);
    fElTLVs.resize(se);
    fElGood.resize(se);

    size_t sj = m_jets->size();
    fJetTLVs.resize(sj);
    fOrigJetTLVs.resize(sj);

    for (size_t i = 0; i < sj; i++) {
        fJetTLVs[i].SetPtEtaPhiE(m_jets->at(i)->pt(), m_jets->at(i)->eta(), m_jets->at(i)->phi(), m_jets->at(i)->e());
        fOrigJetTLVs[i].SetPtEtaPhiE(m_jets->at(i)->pt(), m_jets->at(i)->eta(), m_jets->at(i)->phi(), m_jets->at(i)->e());
    }

    for (size_t i = 0; i < se; i++) {
        fElGood[i] = m_electrons->at(i)->auxdataConst<char>(leptonDef.c_str());

        if (m_electrons->at(i)->caloCluster()) {
            fElClTLVs[i].SetPtEtaPhiM(
                m_electrons->at(i)->caloCluster()->e()/std::cosh(m_electrons->at(i)->caloCluster()->eta()),
                m_electrons->at(i)->caloCluster()->eta(),
                m_electrons->at(i)->caloCluster()->phi(),
                0.511
            );
        } else {
            fElClTLVs[i] = m_electrons->at(i)->p4();
        }

        if (m_electrons->at(i)->trackParticle() && m_electrons->at(i)->caloCluster()) {
            fElTLVs[i].SetPtEtaPhiM(
                m_electrons->at(i)->caloCluster()->e()/std::cosh(m_electrons->at(i)->trackParticle()->eta()),
                m_electrons->at(i)->trackParticle()->eta(),
                m_electrons->at(i)->trackParticle()->phi(),
                0.511
            );
        } else {
            fElTLVs[i] = m_electrons->at(i)->p4();
        }
    }
}

void TTBarElectronJetOverlap::FindAssocEls() {
    size_t nJets = m_jets->size();
    fJetAssocElCls = std::vector<std::set<int> >(nJets);
    size_t nEls = m_electrons->size();
    fElClAssocJet = std::vector<int>(nEls, -1);

    // find the associated electron clusters in each jet
    double drmin, dr;
    TLorentzVector elcl, jet;
    for (size_t z = 0 ; z < nEls; ++z) {
        if (!fElGood[z]) continue;
        elcl = fElClTLVs[z];
        drmin = 0.4;
        int drmin_idx = -1;
        for (size_t y = 0 ; y < nJets; ++y) {
            jet = fJetTLVs[y];
            dr = elcl.DeltaR(jet);
            // attempt to match the jet to the electron.
            if (dr < drmin) {
                drmin = dr;
                drmin_idx = y;
            }
        }
        if (drmin_idx == -1)
            continue;
        fJetAssocElCls[drmin_idx].insert(z);
        fElClAssocJet[z] = drmin_idx;
    }
}

void TTBarElectronJetOverlap::SubtractEls() {
    size_t nJets = m_jets->size();
    int ElIdx;

    fSubJets.resize(nJets);    
    for (size_t k=0; k < nJets; ++k) {
        fSubJets[k] = 0;
    }
    
    for (size_t iJet = 0; iJet < nJets; iJet++) {
        TLorentzVector originalJet = fOrigJetTLVs[iJet];
        for (std::set<int>::iterator iEl = fJetAssocElCls[iJet].begin(); iEl != fJetAssocElCls[iJet].end(); iEl++) {
            ElIdx = *iEl;
            TLorentzVector elcorr = fElTLVs[ElIdx];
            fJetTLVs[iJet] -= elcorr;
            fSubJets[iJet] = 1;
        }
    }
}

void TTBarElectronJetOverlap::FindGoodObjects(float pt_cut_value, float pt_bias_value) {
    size_t nEls = m_electrons->size();
    fGoodEls = std::vector<bool>(nEls, true);

    size_t nJets = fJetTLVs.size();
    fGoodJets = std::vector<bool>(nJets,true);
    std::vector<TLorentzVector> TmpJetTLVs = fJetTLVs;
    std::vector<TLorentzVector> TmpOrigJetTLVs = fOrigJetTLVs;
    TLorentzVector el, jet, jetOrig;
    // step 1: subtracted jets failing the pT cut are assumed to be a result of electron energy deposits => remove them
    for (size_t iJet = 0; iJet < nJets; iJet++) {
        jet = fJetTLVs[iJet];
        jetOrig = fOrigJetTLVs[iJet];
        // pT cut implemented as cut on the ratio 
        if ( fSubJets[iJet] == 1 && jet.Pt() / (jetOrig.Pt() + pt_bias_value) < pt_cut_value ) {
            fGoodJets[iJet] = false;
        }
    }
    // step 2: if surviving jet still within 0.2 of electron in delta-R, remove electron and re-add it to jet
    for (size_t iEl = 0; iEl < nEls; iEl++) {
        if (!fElGood[iEl]) continue;
        el = fElTLVs[iEl];
        for (size_t iJet = 0; iJet < nJets; iJet++) {
            if(!fGoodJets[iJet]) continue;
            jet = fJetTLVs[iJet];
            if (jet.DeltaR(el) < 0.2) {
                // remove from good electrons list.
                fGoodEls[iEl] = false;
                if (fElClAssocJet[iEl] >= 0) {
                    // add electron 4-vector back to jet.
                    TmpJetTLVs[fElClAssocJet[iEl]] += el; 
                    // remove from subtracted jets list
                    fSubJets[iJet] = 0;
                    // remove this electron from the association.
                    fJetAssocElCls[iJet].erase(iEl);
                }
            }
        }
    }
        
    fJetTLVs = TmpJetTLVs;
    fOrigJetTLVs = TmpOrigJetTLVs;
}

void TTBarElectronJetOverlap::AnalyzeEvent(float pt_cut_value, float pt_bias_value, const std::string &leptonDef) {
    FindAssocEls();
    SubtractEls();
    FindGoodObjects(pt_cut_value, pt_bias_value);

    // put the results back in place
    size_t se = m_electrons->size();
    size_t sj = m_jets->size();
    for (size_t i = 0; i < sj; i++) {
        m_jets->at(i)->setJetP4(xAOD::JetFourMom_t(fJetTLVs[i].Perp(), fJetTLVs[i].Eta(), fJetTLVs[i].Phi(), fJetTLVs[i].M()));
      
        if (fGoodJets[i]) m_jets->at(i)->auxdata<char>("passesFancyOR") = 1;       // Subtracted jets that pass the OR test and are kept.
        else              m_jets->at(i)->auxdata<char>("passesFancyOR") = 0;
        
        if (fSubJets[i]==1) m_jets->at(i)->auxdata<char>("subtractedJet") = 1;     // Jets close to electrons that undergo the OR test.
        else                m_jets->at(i)->auxdata<char>("subtractedJet") = 0;
    }


    for (size_t j = 0; j < se; j++) {
        if (!fGoodEls[j] || !fElGood[j]) {
            m_electrons->at(j)->auxdata<char>(leptonDef.c_str()) = 0;
        } else {
            m_electrons->at(j)->auxdata<char>(leptonDef.c_str()) = 1;
        }
    }
}
