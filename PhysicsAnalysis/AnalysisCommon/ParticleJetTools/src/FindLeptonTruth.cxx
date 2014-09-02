/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/FindLeptonTruth.h"
#include "ParticleJetTools/HadronUtils.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GeneratorObjects/McEventCollection.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include <algorithm>

// new
#include "JetEvent/Jet.h"
#include <set>

namespace Analysis {

static const InterfaceID IID_IFindLeptonTruth("Analysis::FindLeptonTruth", 1, 0);
 
FindLeptonTruth::FindLeptonTruth(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t,n,p)
{  
  declareInterface<FindLeptonTruth>(this);
  declareProperty("McEventCollection", m_mcEventCollection = "TruthEvent");
  declareProperty("mudeltaRCut",       m_deltaRCutMu       = 1.0);
  declareProperty("muPTmin",           m_ptCutMu           = 0.*Gaudi::Units::GeV);
  declareProperty("muPmin",            m_pCutMu            = 3.*Gaudi::Units::GeV);
  declareProperty("eldeltaRCut",       m_deltaRCutEl       = 0.4);
  declareProperty("elPTmin",           m_ptCutEl           = 3.*Gaudi::Units::GeV);
  declareProperty("elPmin",            m_pCutEl            = 0.*Gaudi::Units::GeV);
  declareProperty("elRpmax",           m_rProdmax          = 90.); // before 3rd pixel layer...
  declareProperty("NSLTmax",           m_NSLTmax           = 4);
}

FindLeptonTruth::~FindLeptonTruth() {}

const InterfaceID& FindLeptonTruth::interfaceID() {
  return IID_IFindLeptonTruth;
}

StatusCode FindLeptonTruth::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode FindLeptonTruth::finalize() {
  return StatusCode::SUCCESS;
}
  
std::vector<SLTrueInfo> FindLeptonTruth::FindTruth(const Jet& myJet) 
{

  ATH_MSG_VERBOSE("In FindTruth");

  std::vector<SLTrueInfo> truelVec;
  std::set<SLTrueInfo,SLTrueInfoSortCriterion> truel;

  // Do not try if jet not taggable
  if (fabs(myJet.eta()) > 2.7) return truelVec;

  //
  const McEventCollection* myMcEventCollection(0);
  StatusCode sc = evtStore()->retrieve(myMcEventCollection, m_mcEventCollection);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( m_mcEventCollection << " not found in StoreGate." );
  } else {
    const HepMC::GenEvent* GenEvent = *(myMcEventCollection->begin());
    HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();
    for (; pitr != GenEvent->particles_end(); ++pitr) {
      int pdg = (*pitr)->pdg_id();
      int bar = (*pitr)->barcode();
      int sta = (*pitr)->status();
      CLHEP::HepLorentzVector part_momentum_lv((*pitr)->momentum().px(),
					       (*pitr)->momentum().py(),
					       (*pitr)->momentum().pz(),
					       (*pitr)->momentum().e());
      
      if (sta == 1 && (abs(pdg) == 11 || abs(pdg) == 13)) {
	double pt  = (*pitr)->momentum().perp();
	double p   = (*pitr)->momentum().rho();
	double dR  = part_momentum_lv.deltaR(myJet.hlv());
	double ptc = m_ptCutMu; 
	double pc  = m_pCutMu; 
	double drc = m_deltaRCutMu; 
	if (abs(pdg) == 11) {
	  ptc = m_ptCutEl;
	  pc  = m_pCutEl; 
	  drc = m_deltaRCutEl; 
	}
	if (pt > ptc && p > pc && dR < drc) {
	  double rpc = 1.e6; if (abs(pdg) == 11) rpc = m_rProdmax;
	  HepMC::GenVertex *pvtx = (*pitr)->production_vertex();
	  if (pvtx != NULL) {
	    double rp = pvtx->position().perp();
	    if (rp < rpc) {
	      bool cascade  = false;
	      int pdgMother = 0;
	      bool isFB     = false, isFD = false, isFW = false;
	      HepMC::GenVertex::particle_iterator firstParent = pvtx->particles_begin(HepMC::parents);
	      HepMC::GenVertex::particle_iterator endParent   = pvtx->particles_end(HepMC::parents);
	      HepMC::GenVertex::particle_iterator thisParent  = firstParent;
	      for(; thisParent != endParent; ++thisParent){
		if ( (*thisParent)->pdg_id() == pdg && (*thisParent)->status() == 1 ) cascade = true;
		pdgMother = (*thisParent)->pdg_id();
	      }
	      if (cascade) continue;
	      Amg::Vector3D pospvtx(pvtx->position().px(),
					pvtx->position().py(),
					pvtx->position().pz());

	      Amg::Vector3D moment((*pitr)->momentum().px(),
				       (*pitr)->momentum().py(),
				       (*pitr)->momentum().pz());

	      firstParent = pvtx->particles_begin(HepMC::ancestors);
	      endParent   = pvtx->particles_end(HepMC::ancestors);
	      thisParent  = firstParent;
	      for(; thisParent != endParent; ++thisParent){
		int pdg = (*thisParent)->pdg_id();
		short typeP = HadronClass::type(pdg).second;
		if (typeP == 5) isFB = true;
		if (typeP == 4) isFD = true;
		if ( (abs((*thisParent)->pdg_id()) >= 23 && abs((*thisParent)->pdg_id()) <= 25) ||
		     (abs((*thisParent)->pdg_id()) >= 32 && abs((*thisParent)->pdg_id()) <= 37) ) isFW = true;
	      }
	      SLTrueInfo alepton(bar,pdg,pdgMother,isFB,isFD,isFW,moment,pospvtx);
	      truel.insert(alepton);
	    }
	  } else {
	    ATH_MSG_WARNING("No production vertex !");
	    if (msgLvl(MSG::VERBOSE)) (*pitr)->print();
	  }
	}
      }
    }
  }
  std::set<SLTrueInfo,SLTrueInfoSortCriterion>::iterator setFirst = truel.begin(), setEnd = truel.end();
  uint nSLT = 0;
  uint iel  = 0;
  for (; setFirst != setEnd; setFirst++) {
    ATH_MSG_DEBUG( "A true lepton " << iel << " " << (*setFirst) <<
                   "pT = " << (*setFirst).momentum().perp() );
    if (nSLT < m_NSLTmax) {
      truelVec.push_back(*setFirst);
      nSLT++;
    } else {
      break;
    }
    iel++;
  }
  ATH_MSG_DEBUG( "Number of true leptons in this jet " << truel.size() << " stored " << truelVec.size() );
  return truelVec;
}


}
