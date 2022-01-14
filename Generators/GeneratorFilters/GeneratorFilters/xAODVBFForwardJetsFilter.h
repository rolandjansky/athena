/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERSXAODVBFFORWARDJETSFILTER_H
#define GENERATORFILTERSXAODVBFFORWARDJETSFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <vector>
#include "xAODJet/Jet.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"

/// Filter of the type of VBF forward jets
/// @author Junichi TANAKA
class xAODVBFForwardJetsFilter : public GenFilter {
public:

  xAODVBFForwardJetsFilter(const std::string & name, ISvcLocator * pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_JetMinPt;
  double m_JetMaxEta;
  int m_NJets;
  double m_Jet1MinPt;
  double m_Jet1MaxEta;
  double m_Jet2MinPt;
  double m_Jet2MaxEta;
  bool m_UseOppositeSignEtaJet1Jet2;
  double m_DeltaEtaJJ;
  double m_MassJJ;
  bool m_UseLeadingJJ;
  double m_LGMinPt;
  double m_LGMaxEta;
  double m_DeltaRJLG;
  double m_RatioPtJLG;

  std::string m_TruthJetContainerName;

  CLHEP::HepLorentzVector sumDaughterNeutrinos(const xAOD::TruthParticle* tau);
  void removePseudoJets(std::vector<const xAOD::Jet*>& jetList,
                        std::vector<const xAOD::TruthParticle*>& MCTruthPhotonList,
                        std::vector<const xAOD::TruthParticle*>& MCTruthElectronList,
                        std::vector<CLHEP::HepLorentzVector>  & MCTruthTauList);
  double getMinDeltaR(const xAOD::Jet* jet, std::vector<const xAOD::TruthParticle*>& list);
  double getMinDeltaR(const xAOD::Jet* jet, std::vector<CLHEP::HepLorentzVector>& list);
};

#endif
