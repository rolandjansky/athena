/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopJetSubstructure/TTBarElectronJetOverlap.h"

#include <iostream>
#include <cmath>

#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"


using namespace std;

void TTBarElectronJetOverlap::Load(xAOD::JetContainer *jets, xAOD::ElectronContainer *electrons, const std::string &leptonDef) {
    m_jets = jets;
    m_electrons = electrons;
    fDebug = false;

    size_t se = m_electrons->size();
    fElClTLVs.resize(se);
    fElTLVs.resize(se);
    fElGood.resize(se);

    size_t sj = m_jets->size();
    fJetTLVs.resize(sj);
    fOrigJetTLVs.resize(sj);
    fJetJVFs.resize(sj);
    fJetD3PDTrkPtSums.resize(sj);
    fJetD3PDTrkPtPVSums.resize(sj);

    for (size_t i = 0; i < sj; i++) {
      fJetTLVs[i].SetPtEtaPhiE(m_jets->at(i)->pt(), m_jets->at(i)->eta(), m_jets->at(i)->phi(), m_jets->at(i)->e());
      fOrigJetTLVs[i].SetPtEtaPhiE(m_jets->at(i)->pt(), m_jets->at(i)->eta(), m_jets->at(i)->phi(), m_jets->at(i)->e());
      
      //std::vector<float> JVF = m_jets->at(i)->getAttribute<std::vector<float> >("JVF");
      //fJetJVFs[i] = JVF.size() > 0 ? JVF[0] : -1; // (TODO) NOTA BENE: Assumes PV is the first vertex
      //std::vector<float> trkPtSumVec = m_jets->at(i)->getAttribute<std::vector<float> >("SumPtTrkPt500");
      //float trkPtSum = trkPtSumVec.size() > 0 ? trkPtSumVec[0] : 0; // (TODO) NOTA BENE: Assumes PV is the first vertex
      //fJetD3PDTrkPtSums[i] = fJetJVFs[i] ? trkPtSum/JVF[0] : -1;
      //fJetD3PDTrkPtPVSums[i] = trkPtSum;
      if (fDebug) {
          cout << "  - (pre-OR) Jet # " << i <<
              " Pt Eta Phi: " <<
              fJetTLVs[i].Pt() << " " <<
              fJetTLVs[i].Eta() << " " <<
              fJetTLVs[i].Phi() << endl;
      }
    }

    for (size_t i = 0; i < se; i++) {
        fElGood[i] = m_electrons->at(i)->auxdataConst<char>(leptonDef.c_str());

        if (m_electrons->at(i)->caloCluster()) {
          fElClTLVs[i].SetPtEtaPhiM(m_electrons->at(i)->caloCluster()->e()/std::cosh(m_electrons->at(i)->caloCluster()->eta()),
                  m_electrons->at(i)->caloCluster()->eta(), m_electrons->at(i)->caloCluster()->phi(), 0.511);
        } else {
          fElClTLVs[i] = m_electrons->at(i)->p4();
        }

        if (m_electrons->at(i)->trackParticle() && m_electrons->at(i)->caloCluster()) {
          fElTLVs[i].SetPtEtaPhiM(m_electrons->at(i)->caloCluster()->e()/std::cosh(m_electrons->at(i)->trackParticle()->eta()),
                                  m_electrons->at(i)->trackParticle()->eta(), m_electrons->at(i)->trackParticle()->phi(), 0.511);
        } else {
          fElTLVs[i] = m_electrons->at(i)->p4();
        }
        if (fDebug) {
            cout << "  - (pre-OR) El # " << i <<
                " Pt Eta Phi: " <<
                fElTLVs[i].Pt() << " " <<
                fElTLVs[i].Eta() << " " <<
                fElTLVs[i].Phi() << endl;
        }
    }
}

void TTBarElectronJetOverlap::FindAssocEls() {
    size_t nJets = m_jets->size();
    fJetAssocElCls = vector<set<int> >(nJets);
    size_t nEls = m_electrons->size();
    fElClAssocJet = vector<int>(nEls, -1);

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
    size_t nEls = m_electrons->size();
    int ElIdx;

    fSubJets.resize(nJets);    
    for (int k=0; k<nJets; ++k) {
      fSubJets[k] = 0;
    }
    
    for (size_t iJet = 0; iJet < nJets; iJet++) {
      TLorentzVector originalJet = fOrigJetTLVs[iJet];
      for (set<int>::iterator iEl = fJetAssocElCls[iJet].begin(); iEl != fJetAssocElCls[iJet].end(); iEl++) {
	ElIdx = *iEl;
	TLorentzVector elcorr = fElTLVs[ElIdx];
	fJetTLVs[iJet] -= elcorr;
	fSubJets[iJet] = 1;
      }
    }
}

void TTBarElectronJetOverlap::FindGoodObjects() {
    size_t nEls = m_electrons->size();
    fGoodEls = vector<bool>(nEls, true);

    size_t nJets = fJetTLVs.size();
    vector<TLorentzVector> TmpJetTLVs = fJetTLVs;
    vector<TLorentzVector> TmpOrigJetTLVs = fOrigJetTLVs;
    TLorentzVector el, jet, jetOrig;
    
    for (size_t iEl = 0; iEl < nEls; iEl++) {
        if (!fElGood[iEl]) continue;

        el = fElTLVs[iEl];

        if (fDebug) {
            cout << "  - (good) El # " << iEl <<
                " Pt Eta Phi: " <<
                el.Pt() << " " <<
                el.Eta() << " " <<
                el.Phi() << endl;
        }

        for (size_t iJet = 0; iJet < nJets; iJet++) {
            jet = fJetTLVs[iJet];
	    jetOrig = fOrigJetTLVs[iJet];
            if (fDebug) {
                cout << "  - (good) Jet # " << iJet <<
                    " Pt Eta Phi M: " <<
                    jet.Pt() << " " <<
                    jet.Eta() << " " <<
                    jet.Phi() << " " <<
                    jet.M() << endl;
            }
	
            //if (jet.Pt() < 25e3)
	    //continue;	
	
	    // New overlap removal procedure:
            // subtracted ets failing the pT cut are assumed to be a result of electron energy deposits. Continue to the next jet.

	    if ((jetOrig.Pt() >= 30e3 && jetOrig.Pt() < 50e3) && jet.Pt() < 20e3) {    
              continue;                                                                                                                                                                                    
            } else if (( jetOrig.Pt() >= 50e3 && jetOrig.Pt() < 100e3) && jet.Pt() <30e3) {
	      continue;                                                                                                                                                                                   
            } else if ((jetOrig.Pt() >= 100e3 && jetOrig.Pt() < 400e3) && jet.Pt() <90e3) { 
	      continue;                                                                                                                                                                                   
            } else if ((jetOrig.Pt() >= 400e3 && jetOrig.Pt() < 800e3) && jet.Pt() <130e3) {
	      continue;                                                                                                                                                                                   
            } else if ((jetOrig.Pt() >= 800e3 && jetOrig.Pt() <1200e3) && jet.Pt() <230e3) {
	      continue;                                                                                                                                                                                   
            }   else if ((jetOrig.Pt() >=1200e3 && jetOrig.Pt() <1600e3) && jet.Pt() <700e3) {
	      continue;
	    } else if ((jetOrig.Pt() >=1600e3 && jetOrig.Pt() <2000e3) && jet.Pt() < 1250e3) {
	      continue;
	    } else if ((jetOrig.Pt() >=2000e3 && jetOrig.Pt()<2200e3) && jet.Pt() < 1300e3) {
	      continue;
	    } else if (jetOrig.Pt()>2200e3 && jet.Pt()<1350e3) { 
	      continue;
	    }
	    
	    // if the electron is too close to a subtracted jet...
	    if (jet.DeltaR(el) < 0.2) {
	      
	      if (fDebug)
		cout << "    El too close to jet. Removing." << endl;
	      
	      // remove from good electrons list.
	      fGoodEls[iEl] = false;
	      
	      if (fElClAssocJet[iEl] >= 0) {
		// add electron 4-vector back to jet.
		TmpJetTLVs[fElClAssocJet[iEl]] += el; 
		//remove from subtracted jets list
		fSubJets[iJet] = 0;
		// remove this electron from the association.
		fJetAssocElCls[iJet].erase(iEl);
	      }
	    }  
	}
    }
    
    fJetTLVs = TmpJetTLVs;
    fOrigJetTLVs = TmpOrigJetTLVs;
    fGoodJets = vector<bool>(nJets,true);
    
    for (size_t iJet = 0; iJet < nJets; iJet++) {
      jetOrig = fOrigJetTLVs[iJet];
       
       if (jetOrig.Pt() >= 30e3 && jetOrig.Pt() < 50e3) {                                                                                                                                                  
         fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 20e3;                                                                                                                                                     
       } else if (jetOrig.Pt() >= 50e3 && jetOrig.Pt() < 100e3) {                                                                                                                                          
         fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 30e3;                                                                                                                                                     
       } else if (jetOrig.Pt() >= 100e3 && jetOrig.Pt() < 400e3) {                                                                                                                                         
         fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 90e3;       
       } else if (jetOrig.Pt() >= 400e3 && jetOrig.Pt() < 800e3) {                                                                                                                                         
         fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 130e3; 
       } else if (jetOrig.Pt() >= 800e3 && jetOrig.Pt() < 1200e3) {                                                                                                                                                                 
         fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 230e3; 
       } else if (jetOrig.Pt() >= 1200e3 && jetOrig.Pt() < 1600e3) {
	 fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 700e3; 
       } else if (jetOrig.Pt() >= 1600e3 && jetOrig.Pt() < 2000e3) {
	 fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 1250e3; 
       } else if (jetOrig.Pt() >= 2000e3 && jetOrig.Pt() < 2200e3) { 
	 fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 1300e3;
       } else if (jetOrig.Pt() > 2200e3) {
	 fGoodJets[iJet] = fJetTLVs[iJet].Pt() > 1350e3;  
       }
    }
}

void TTBarElectronJetOverlap::RecalcJVF() {

    size_t nJets = m_jets->size();
    int ElIdx;
    //int TrkIdx;
    TLorentzVector trk;

    for (size_t iJet = 0; iJet < nJets; iJet++) {

        // if jvf == 0 or -1, no need to recalulate...
        if (!fJetJVFs[iJet] ||
                fJetJVFs[iJet] < 0)
            continue;

        for (set<int>::iterator iEl = fJetAssocElCls[iJet].begin();
                iEl != fJetAssocElCls[iJet].end(); iEl++) {

            ElIdx = *iEl;

            trk.SetPtEtaPhiE(m_electrons->at(ElIdx)->trackParticle()->pt(),
                             m_electrons->at(ElIdx)->trackParticle()->eta(), m_electrons->at(ElIdx)->trackParticle()->phi(), m_electrons->at(ElIdx)->trackParticle()->e());

            // not in the associated tracks.
            bool foundMatch = false;
            std::vector<const xAOD::TrackParticle*> jetTracks;
            //bool haveJetTracks = m_jets->at(iJet)->getAssociatedObjects(xAOD::JetAttribute::GhostTrack, jetTracks);
            for (size_t t = 0; t < jetTracks.size(); ++t) {
                TLorentzVector jet_trk;
                jet_trk.SetPtEtaPhiE(jetTracks[t]->pt(), jetTracks[t]->eta(), jetTracks[t]->phi(), jetTracks[t]->e());
                if (jet_trk.DeltaR(trk) < 0.01) {
                    foundMatch = true;
                }
            }

            if (!foundMatch)
                continue;

            // recompute JVF.
            fJetD3PDTrkPtSums[iJet] -= trk.Pt();

            if (std::fabs(std::sin(m_electrons->at(ElIdx)->trackParticle()->theta())*m_electrons->at(ElIdx)->trackParticle()->z0()) < 1.0 &&
                std::fabs(m_electrons->at(ElIdx)->trackParticle()->d0()) < 1.0)
                fJetD3PDTrkPtPVSums[iJet] -= trk.Pt();
        }

        // we subtracted too much for some reason?
        if (fJetD3PDTrkPtSums[iJet] < 0 ||
                fJetD3PDTrkPtPVSums[iJet] < 0)
            fJetJVFs[iJet] = 0;
        // no tracks associated with the jet.
        else if (fJetD3PDTrkPtSums[iJet] == 0)
            fJetJVFs[iJet] = -1;
        // all good. recalculate JVF.
        else
            fJetJVFs[iJet] = fJetD3PDTrkPtPVSums[iJet]/fJetD3PDTrkPtSums[iJet];
    }
}

void TTBarElectronJetOverlap::AnalyzeEvent(const std::string &leptonDef) {
  FindAssocEls();
  SubtractEls();
  FindGoodObjects();
  //RecalcJVF();

    // put the results back in place
    size_t se = m_electrons->size();
    size_t sj = m_jets->size();
    for (size_t i = 0; i < sj; i++) {
        m_jets->at(i)->setJetP4(xAOD::JetFourMom_t(fJetTLVs[i].Perp(), fJetTLVs[i].Eta(), fJetTLVs[i].Phi(), fJetTLVs[i].M()));
      
	if (fGoodJets[i]) m_jets->at(i)->auxdata<char>("passesFancyOR") = 1;    // Subtracted jets that pass the OR test and are kept.
	else              m_jets->at(i)->auxdata<char>("passesFancyOR") = 0;
	
	if (fSubJets[i]==1) m_jets->at(i)->auxdata<char>("subtractedJet") = 1;     // Jets close to electrons that undergo the OR test.
        else              m_jets->at(i)->auxdata<char>("subtractedJet") = 0;


	//std::vector<float> newJVF;
        //newJVF.push_back(fJetJVFs[i]);
        //m_jets->at(i)->setAttribute<std::vector<float> >("JVF", newJVF);
        if (fDebug) {
            cout << "  - (final) Jet # " << i <<
                " Pt Eta Phi M: " <<
                m_jets->at(i)->pt() << " " <<
                m_jets->at(i)->eta() << " " <<
                m_jets->at(i)->phi() << " " <<
                m_jets->at(i)->m()   << "   good " << (int) m_jets->at(i)->auxdata<char>(leptonDef.c_str())<< endl;
        }
    }


    for (size_t j = 0; j < se; j++) {
        if (!fGoodEls[j] || !fElGood[j]) {
          m_electrons->at(j)->auxdata<char>(leptonDef.c_str()) = 0;
       } else {
          m_electrons->at(j)->auxdata<char>(leptonDef.c_str()) = 1;
	}

	if (fDebug) {
            cout << "  - (final) El # " << j <<
                " Pt Eta Phi M: " <<
                m_electrons->at(j)->pt() << " " <<
                m_electrons->at(j)->eta() << " " <<
                m_electrons->at(j)->phi() << " " <<
                m_electrons->at(j)->m()   << "   good " << (int) m_electrons->at(j)->auxdata<char>(leptonDef.c_str())  << endl;
        }
    }
}


