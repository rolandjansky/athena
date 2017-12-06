/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/PhysicalConstants.h"
#include "GeneratorFilters/TruthJetWeightFilter.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "ParticleEvent/ParticleBaseContainer.h"
#include "xAODJet/JetContainer.h"
#include <TMath.h>
#include <algorithm>

TruthJetWeightFilter::TruthJetWeightFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("weightAlgorithm",      m_weightAlgorithm = 0);
  declareProperty("rescaleEm",            m_rescaleEm = 2.);
  declareProperty("rescaleHad",           m_rescaleHad = 0.5);

  declareProperty("minPtLeadUpweight",    m_minPtLeadUpweight = 20.);

  declareProperty("minPt",         m_minPt = 20.);
  declareProperty("maxPt",         m_maxPt = 80.);
  declareProperty("minEfficiency", m_minEfficiency = 0.01);
  declareProperty("lowPtTune",     m_lowPtTune = 5.);
  declareProperty("maxEtaRange",   m_maxEtaRange = 2.7);

  declareProperty("deactivateFilter", m_deactivateFilter = false);
  declareProperty("samplingRandomSeed", m_samplingRandomSeed = 0);

  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthJets");
  m_MLPReader = 0;
  m_rand = 0;
}


StatusCode TruthJetWeightFilter::filterInitialize() {
  ATH_MSG_INFO("weightAlgorithm = " << m_weightAlgorithm);
  if (m_weightAlgorithm == 0) ATH_MSG_INFO("\tJetSubinfo");
  else if (m_weightAlgorithm == 1) ATH_MSG_INFO("\tStandard Leading Jet Pt");
  else if (m_weightAlgorithm == 2) ATH_MSG_INFO("\tRescaled Leading Jet Pt");
  else if (m_weightAlgorithm == 3) ATH_MSG_INFO("\tStandard Leading Jet Pt + Leading Particle upweight");
  else if (m_weightAlgorithm == 4) ATH_MSG_INFO("\tRescaled Leading Jet Pt + Leading Particle upweight");
  else ATH_MSG_ERROR("\tUnknown Algorithm specified!");
  ATH_MSG_INFO("rescaleEm = " << m_rescaleEm);
  ATH_MSG_INFO("rescaleHad = " << m_rescaleHad);
  ATH_MSG_INFO("minPtLeadUpweight = " << m_minPtLeadUpweight);

  ATH_MSG_INFO("minPt = " << m_minPt);
  ATH_MSG_INFO("maxPt = " << m_maxPt);
  ATH_MSG_INFO("minEfficiency = " << m_minEfficiency);
  ATH_MSG_INFO("lowPtTune = " << m_lowPtTune);
  ATH_MSG_INFO("maxEtaRange = " << m_maxEtaRange);

  ATH_MSG_INFO("deactivateFilter = " << m_deactivateFilter);
  ATH_MSG_INFO("xAOD::JetContainer=" << m_TruthJetContainerName);

  ATH_MSG_INFO("samplingRandomSeed = " << m_samplingRandomSeed);
  m_rand = new TRandom3(m_samplingRandomSeed);
  ATH_MSG_INFO("\tused random seed = " << m_rand->GetSeed());

  const char* inputVars[] = { "ptj", "egcore", "hadcore", "leadpt", "leadptchg", "highm5pt"};
  std::vector<std::string> inputVars2(inputVars, inputVars+(sizeof(inputVars)/sizeof(inputVars[0])));
  m_MLPReader = new ReadMLP(inputVars2);

  return StatusCode::SUCCESS;
}


StatusCode TruthJetWeightFilter::filterEvent() {
  double weight = 1.;
  double prob = 0.;

  StatusCode sc = StatusCode::FAILURE;
  if (m_weightAlgorithm == 0) sc = getEffJetSubinfo(prob);
  else if (m_weightAlgorithm >= 1 && m_weightAlgorithm >= 4) sc = getEffJetPt(prob);
  if (prob < 0.) weight = 0.;
  else if (prob < 1.) weight = 1./prob;

  double rnd = m_rand->Uniform();
  ATH_MSG_DEBUG("prob rnd weight sel " << prob << " " << rnd << " " << weight << " " << (rnd < prob));
  if (rnd < prob) {
    ATH_MSG_INFO("Pass filter weight "  << weight);
    saveWeight(weight);
    return sc;
  } else {
    setFilterPassed(m_deactivateFilter || false);
    saveWeight(0.);
    ATH_MSG_INFO("Fail filter weight "  << weight);
    return sc;
  }
}


StatusCode TruthJetWeightFilter::getEffJetPt(double &prob) {
  StatusCode sc = StatusCode::SUCCESS;

  const xAOD::JetContainer* truthjetTES;
  CHECK(evtStore()->retrieve(truthjetTES, m_TruthJetContainerName));
  ATH_MSG_DEBUG("xAOD::JetContainer size = " << truthjetTES->size());
  double leadpartupw = 1.;
  xAOD::JetContainer::const_iterator it_truth = truthjetTES->begin();
  const xAOD::Jet* leadingJet = 0;
  double leadingJetPt = m_minPt;

  for (int i = 0; it_truth != truthjetTES->end(); ++it_truth, i++) {
    ATH_MSG_DEBUG("TruthJet #" << i << " pt "  <<(*it_truth)->pt()/Gaudi::Units::GeV << " eta "  <<(*it_truth)->eta());
    if (m_maxEtaRange > 0. && TMath::Abs((*it_truth)->eta()) > m_maxEtaRange) continue;
    double jetpt = (*it_truth)->pt()/Gaudi::Units::GeV;

    TLorentzVector jet_tlv = (*it_truth)->p4();
    CLHEP::HepLorentzVector jet_hlv(jet_tlv.Px(), jet_tlv.Py(), jet_tlv.Pz(), jet_tlv.E());

    if (m_weightAlgorithm == 2 || m_weightAlgorithm == 4) jetpt = getRescaledJetPt(jet_hlv)/Gaudi::Units::GeV;
    if (leadingJet == 0 || jetpt > leadingJetPt) {
      leadingJet = (*it_truth);
      leadingJetPt = jetpt;
    }

    if (m_weightAlgorithm == 3 || m_weightAlgorithm == 4) {
      CLHEP::HepLorentzVector lead, sublead, leadchg, highm5, highm10;
      double newleadpartupw = 1.;
      getLeadTruthParts(jet_hlv, lead, sublead, leadchg, highm5, highm10);
      if (lead.perp()/Gaudi::Units::GeV > m_minPtLeadUpweight) {
        if (sublead.perp() <= 0. || lead.perp()/sublead.perp() > 10.) {
          newleadpartupw = 10.;
        } else if (lead.perp()/sublead.perp() > 5.) {
          newleadpartupw = 4.;
        } else if (lead.perp()/sublead.perp() > 3.5) {
          newleadpartupw = 2.;
        }
      }
      if (newleadpartupw > leadpartupw) leadpartupw = newleadpartupw;
    }

  }

  if (leadingJetPt > m_minPt && leadingJet) {
    ATH_MSG_DEBUG("Leading TruthJet pt " << leadingJetPt << " eta " << leadingJet->eta() << " phi " << leadingJet->phi());
  } else {
    ATH_MSG_DEBUG("No TruthJet with pt > "  << m_minPt);
  }

  const double xmineff = m_minEfficiency/m_lowPtTune;
  const double a1 = TMath::Log(xmineff)/TMath::Log(m_minPt/m_maxPt);
  const double a0 = 1./TMath::Power(m_maxPt, a1);
  const double a2 = m_minEfficiency-xmineff;

  prob = m_minEfficiency;
  if (leadingJetPt > m_minPt ) prob = a0*TMath::Power(leadingJetPt, a1) + a2;

  if (m_weightAlgorithm == 3 || m_weightAlgorithm == 4) {
    ATH_MSG_DEBUG("Upscale for large pt leading particle by " << leadpartupw);
    prob *= leadpartupw;
  }

  return sc;
}


StatusCode TruthJetWeightFilter::getEffJetSubinfo(double &prob) {
  StatusCode sc = StatusCode::SUCCESS;
  const xAOD::JetContainer* truthjetTES;
  CHECK(evtStore()->retrieve(truthjetTES, m_TruthJetContainerName));
  ATH_MSG_DEBUG("xAOD::JetContainer size = " << truthjetTES->size());

  prob = m_minEfficiency;
  double maxmva = -999.;
  xAOD::JetContainer::const_iterator it_truth = truthjetTES->begin();
  for (int i = 0; it_truth != truthjetTES->end(); ++it_truth, i++) {
    ATH_MSG_DEBUG("TruthJet #" << i << " pt "  <<(*it_truth)->pt()/Gaudi::Units::GeV << " eta "  <<(*it_truth)->eta());

    // Hard-coded cut values: these correspond to the TMVA training and are fixed!
    if (TMath::Abs((*it_truth)->eta()) > 2.7) continue;
    if ((*it_truth)->pt()/Gaudi::Units::GeV < 17.) continue;

    std::vector <double> jetvalues;
    jetvalues.reserve(15);
    jetvalues.push_back((*it_truth)->pt()/Gaudi::Units::GeV);

    TLorentzVector jet_tlv = (*it_truth)->p4();
    CLHEP::HepLorentzVector jet_hlv(jet_tlv.Px(), jet_tlv.Py(), jet_tlv.Pz(), jet_tlv.E());
    getJetSubinfo(jet_hlv, jetvalues);

    ATH_MSG_DEBUG("ptj, egcore, hadcore," << " leadpt, leadptchg, highmom5");
    for (unsigned int ii = 0; ii < jetvalues.size(); ii++) ATH_MSG_DEBUG(jetvalues[ii]);
    // High pt (ptjet > 100 or egcore > 25) are not prescaled at all otherwise use MLP to decide on prescale factor
    double mva = (jetvalues[0] > 100. || jetvalues[1] > 25.) ? 1.5 :  m_MLPReader->GetMvaValue(jetvalues);
    ATH_MSG_DEBUG("MVA value " << mva);
    if (mva > maxmva) maxmva = mva;
  }

  // further hard-coded values specific to TMVA training
  if (maxmva < -0.996) prob = 1./50.;
  else if (maxmva < -0.863098) prob = 1./30.;
  else if (maxmva < -0.460535) prob = 1./20.;
  else if (maxmva < -0.126538) prob = 1./10.;
  else if (maxmva <  0.136516) prob = 1./5.;
  else if (maxmva <  0.395319) prob = 1./2.;
  else prob = 1.;
  ATH_MSG_DEBUG("Maximal MVA prob " << maxmva << " " << prob);

  return sc;
}


double TruthJetWeightFilter::getRescaledJetPt(const CLHEP::HepLorentzVector &jet) {
  double empart = 0;
  double hadpart = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ((*pitr)->status() != 1) continue;
      HepMC::FourVector tmp = (*pitr)->momentum();
      CLHEP::HepLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
      if (jet.deltaR(genpart) > 0.4) continue;

      // Separate EM particles (including K0short here!) and hadronic
      if (abs((*pitr)->pdg_id()) == 11 || abs((*pitr)->pdg_id()) == 22 || abs((*pitr)->pdg_id()) == 310) {
        empart += genpart.perp();
      } else if (abs((*pitr)->pdg_id()) > 100) {
        hadpart += genpart.perp();
      }
    }
  }

  double emfrac = empart/(empart+hadpart);
  double hadfrac = hadpart/(empart+hadpart);
  double scale = m_rescaleEm*emfrac + m_rescaleHad*hadfrac;
  double pt = jet.perp();
  double ptrescaled = pt*scale;
  ATH_MSG_DEBUG("All/rescaled Et EM/HAD fractions " << pt/Gaudi::Units::GeV << " " << ptrescaled/Gaudi::Units::GeV << " " << emfrac << " " << hadfrac);
  return ptrescaled;
}


void TruthJetWeightFilter::getLeadTruthParts(const CLHEP::HepLorentzVector& jet,
                                             CLHEP::HepLorentzVector& lead, CLHEP::HepLorentzVector& sublead, CLHEP::HepLorentzVector& leadchg,
                                             CLHEP::HepLorentzVector& highm5, CLHEP::HepLorentzVector& highm10) {
  lead.set(0., 0., 0., 0.);
  sublead.set(0., 0., 0., 0.);
  leadchg.set(0., 0., 0., 0.);
  highm5.set(0., 0., 0., 0.);
  highm10.set(0., 0., 0., 0.);

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr ) {
      if ((*pitr)->status() != 1) continue;
      int pdgid = (*pitr)->pdg_id();
      if (abs(pdgid) == 12 || abs(pdgid) == 14 || abs(pdgid) == 16) continue;

      HepMC::FourVector tmp = (*pitr)->momentum();
      CLHEP::HepLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
      if (jet.deltaR(genpart) > 0.4) continue;

      // No muons in "leading particle for energy deposition"
      if (abs(pdgid) != 13) {
        if (genpart.perp() > lead.perp()) {
          sublead = lead;
          lead = genpart;
        } else if (genpart.perp() > sublead.perp()) {
          sublead = genpart;
        }
      }

      // Particles able to make a track without large impact parameter: e, mu, CLHEP::pi+-, K+-, p/pbar
      // Leave muons in, only consider particles with ctau >~1m and charge.
      // A couple of strange charmed baryons with ctau=2~4cm are also not taken...
      if (genpart.perp() > leadchg.perp() &&
          (abs(pdgid) == 11 || abs(pdgid) == 13 ||
           abs(pdgid) == 211 || abs(pdgid) == 321 || abs(pdgid) == 2212)) leadchg = genpart;

      // Fixed threshold sums
      if (genpart.perp() > 5*Gaudi::Units::GeV) highm5 += genpart;
      if (genpart.perp() > 10*Gaudi::Units::GeV) highm10 += genpart;
    }
  }

  ATH_MSG_DEBUG("Leading/subleading/leading charged pt particles " << lead.perp()/Gaudi::Units::GeV << " " << sublead.perp()/Gaudi::Units::GeV << " " << leadchg.perp()/Gaudi::Units::GeV);
  ATH_MSG_DEBUG("Fixed threshold sums pt>5/10 " << highm5.perp()/Gaudi::Units::GeV << " " << highm10.perp()/Gaudi::Units::GeV);
}


void TruthJetWeightFilter::getJetSubinfo(const CLHEP::HepLorentzVector &axis, std::vector<double> &jetvalues) {
  CLHEP::HepLorentzVector subcomp(0., 0., 0., 0.);
  CLHEP::HepLorentzVector egall = getJetSubcomponent(axis, 1, 0.4);
  CLHEP::HepLorentzVector hadall = getJetSubcomponent(axis, 2, 0.4);
  CLHEP::HepLorentzVector k0sall = getJetSubcomponent(axis, 3, 0.4);
  hadall -= k0sall;

  double egallpt = egall.perp();
  double hadallpt = hadall.perp();
  double k0sallpt = k0sall.perp();
  double egcorept = getJetSubcomponent(axis, 1, 0.1).perp();
  // Perform "recovery" procedure for "core" contributions:
  // Maximise potential core pt
  if (egallpt > 0) egcorept = std::max(egcorept, getJetSubcomponent(egall, 1, 0.1).perp());
  // Test if K0s direction will increase eg core contribution
  if (egallpt > 0 && k0sallpt > 0) egcorept = std::max(egcorept, getJetSubcomponent(egall+0.3*k0sall, 1, 0.1).perp());

  double hadcorept = getJetSubcomponent(axis, 2, 0.1).perp();
  double k0scorept = getJetSubcomponent(axis, 3, 0.1).perp();
  if (k0sallpt > 0) k0scorept = std::max(k0scorept, getJetSubcomponent(k0sall, 3, 0.1).perp());
  // Test if eg direction will increase K0s core contribution
  if (k0sallpt > 0 && egallpt > 0) k0scorept = std::max(k0scorept, getJetSubcomponent(egall+0.3*k0sall, 3, 0.1).perp());
  hadcorept = std::max(0., hadcorept-k0scorept);

  // Immediately merge the K0s into eg and had here according to naive branching
  egallpt += 0.3*k0sallpt;
  egcorept += 0.3*k0scorept;
  hadallpt += 0.7*k0sallpt;
  hadcorept += 0.7*k0scorept;

  // Get leading truth particles info
  CLHEP::HepLorentzVector lead, sublead, leadchg, highm5, highm10;
  getLeadTruthParts(axis, lead, sublead, leadchg, highm5, highm10);

  // Store all used values in output vector
  jetvalues.push_back(egcorept/Gaudi::Units::GeV);
  jetvalues.push_back(hadcorept/Gaudi::Units::GeV);
  jetvalues.push_back(lead.perp()/Gaudi::Units::GeV);
  jetvalues.push_back(leadchg.perp()/Gaudi::Units::GeV);
  jetvalues.push_back(highm5.perp()/Gaudi::Units::GeV);
}


CLHEP::HepLorentzVector TruthJetWeightFilter::getJetSubcomponent(const CLHEP::HepLorentzVector &axis,
                                                                 int idcomp, double maxdr, double ptcutoff) {
  CLHEP::HepLorentzVector subcomp(0., 0., 0., 0.);

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ((*pitr)->status() != 1) continue;
      int pdgid = (*pitr)->pdg_id();

      // Only add specific components
      /// @todo Check that it's ok to use loop continues in a switch -- "break" certainly wouldn't work.
      switch (idcomp) {
      case 1:
        // e-gamma components
        if (abs(pdgid) != 11 && abs(pdgid) != 22) continue;
        break;
      case 2:
        // All hadrons
        if (abs(pdgid) <= 100) continue;
        break;
      case 3:
        // K0s
        if (abs(pdgid) != 310) continue;
        break;
      case 4:
        // Longer-lived Kaons
        if (abs(pdgid) != 130 && abs(pdgid) != 321) continue;
        break;
      default:
        continue;
      }

      HepMC::FourVector tmp = (*pitr)->momentum();
      CLHEP::HepLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
      if (genpart.perp()/Gaudi::Units::GeV < ptcutoff) continue;
      if (axis.deltaR(genpart) > maxdr) continue;

      // Finally: add subcomponent
      subcomp += genpart;
    }
  }

  ATH_MSG_DEBUG("subcomponent id maxdr pt " << idcomp << " " << maxdr << " " << subcomp.perp()/Gaudi::Units::GeV);
  return subcomp;
}


StatusCode TruthJetWeightFilter::saveWeight(double weight) {
  // Prepare write access to put weights
  McEventCollection* mymccoll;
  CHECK(evtStore()->retrieve(mymccoll, "GEN_EVENT"));
  McEventCollection::iterator itr = mymccoll->begin();
  for (int ii = 0; itr!=mymccoll->end(); ++itr, ii++) {
    HepMC::GenEvent* genEvt = (*itr);
    HepMC::WeightContainer& wgtsC = genEvt->weights();

    // Only the event weight #0 is read by CopyEventWeight.
    // Remaining (Pythia) weights are usually all one, but anyway multiply them all here.
    for (size_t iw = 0; iw < (size_t) wgtsC.size(); iw++) {
      if (wgtsC[iw] != 0) weight *= wgtsC[iw];
    }

    // Then put my weight on first place
    wgtsC[0] = weight;
    ATH_MSG_DEBUG("McEventCollection #" << ii << " WeightContainer size " << wgtsC.size());
    for (size_t iii = 0; iii < (size_t) wgtsC.size(); iii++) {
      ATH_MSG_DEBUG("Weight #" << iii << " " << wgtsC[iii]);
    }
  }
  return StatusCode::SUCCESS;
}
