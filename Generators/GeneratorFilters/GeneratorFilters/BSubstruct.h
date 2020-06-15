/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_BSUBSTRUCTURE_H
#define GENERATORFILTERS_BSUBSTRUCTURE_H

#include "GeneratorModules/GenFilter.h"
#include "xAODJet/Jet.h"
class TH1;

class BSubstruct : public GenFilter{
public:

  BSubstruct(const std::string &name, ISvcLocator *pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  /**
   *  Returns true if a pdg id corresponds to a particle containing a c or b quark
   *  pdg ids are constructed such that one of the 2nd, 3rd or 4th digits of
   *  the pdg id is 5 for a b flavoured particle, 4 for a c flavoured particle.
   */
  bool hasCBQuark(int pdgId)const;

  /**
   *  Recursively calls itself to return a list of all particles earlier in the
   *  decay chain that are c or b-flavoured.
   */
  std::vector<HepMC::ConstGenParticlePtr> ancestorCBs(HepMC::ConstGenParticlePtr p)const;

  /// the delta-r between two vectors at y, phi
  double deltaR(double y1, double phi1, double y2, double phi2)const;
  /// the delta-r between a jet an a hadron
  double deltaR(HepMC::ConstGenParticlePtr particle, const xAOD::Jet *jet)const;
  /// the delta-r between two hadrons
  double deltaR(HepMC::ConstGenParticlePtr particle1, HepMC::ConstGenParticlePtr particle2)const;
  /// the delta-r between two jets
  double deltaR(const xAOD::Jet*, const xAOD::Jet*)const;
  // the delta-phi of two angles, returned in the range 0-Pi
  double deltaPhi(double phi1, double phi2)const;

  void fillHistos(TH1 *phihisto, TH1 *rHisto, TH1 *nHisto, TH1 *bHisto,
                  const std::vector<HepMC::ConstGenParticlePtr> &bHadrons, double weight);

  /// Count of events input for filtering
  size_t m_nEvents;
  /// Count of events passing the filter
  size_t m_nAccepted;
  /// Count of events rejected fromt the filter
  size_t m_nRejected;

  // The storegate key for the jets
  std::string m_jetContainerName;

  /// The minimum pT for a jet to be considered.
  double m_jetPTMin;

  /// The maximum pT for a jet
  double m_jetPTMax;

  /// Shoule we check the jet pT against the upper cut
  bool m_doJetMaxPTCut;

  /// Do we match b-hadrons to jets
  bool m_doBHadrons;

  /// Do we match c-hadrons to jets
  bool m_doCHadrons;

  /// Do we match either B or C hadrons
  bool m_doHeavyHadrons;

  // The maximum delta-r between a b-hadron and a jet for them to be considered matched
  // This should normally be equal to or slightly larger than the jet radius used
  double m_drMatch;

  // The minimum delta R between two b-jets
  //double m_drMin;

  // Do we store histograms
  bool m_doHistos;


  TH1 *m_h_nBPass;
  TH1 *m_h_nBAll;

  TH1 *m_h_dRPass;
  TH1 *m_h_dRAll;

  TH1 *m_h_dPhiPass;
  TH1 *m_h_dPhiAll;

  TH1 *m_h_bPhiPass;

};

#endif
