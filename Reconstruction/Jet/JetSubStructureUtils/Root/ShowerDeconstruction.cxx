/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/ShowerDeconstruction.h"

#ifndef NO_SHOWERDECONSTRUCTION
#include <Deconstruct.h>
#include <AnalysisParameters.h>
#include <TopGluonModel.h>
#include <WDecayModel.h>
#include <BackgroundModel.h>
#include <ISRModel.h>
#endif // not NO_SHOWERDECONSTRUCTION

#include <fastjet/ClusterSequence.hh>
#include "xAODJet/Jet.h"
#include <fastjet/PseudoJet.hh>
#include "JetEDM/JetConstituentFiller.h"

using namespace std;
using namespace JetSubStructureUtils;

ShowerDeconstruction::ShowerDeconstruction(SignalModel signalModel)
{
  // disable warning due to conditionally compiled code
  (void) signalModel;

  m_param = 0;
  m_WModel = 0;
  m_topModel = 0;
  m_bkgModel = 0;
  m_isrModel = 0;
  m_deconstruct = 0;

#ifndef NO_SHOWERDECONSTRUCTION
  m_param = new AnalysisParameters();
  (*m_param)["R"] = 1.0;
  (*m_param)["lambda_mu_ext"] = 1.0;
  (*m_param)["lambda_thetha_ext"] = 1.0;
  (*m_param)["fractionISR_of_hardInt_PT"] = 0.5;
  (*m_param)["noISR"] = 0.0;
  (*m_param)["topmass"] = 172.0;
  (*m_param)["topwindow"] = 50.0;
  (*m_param)["wmass"] = 80.403;
  (*m_param)["wwindow"] = 20.0;
  (*m_param)["Higgsmass"] = 125.0;
  (*m_param)["delta_Higgsmass"] = 20.0;
  (*m_param)["br_wqq"] = 0.6666666666;
  (*m_param)["useBtag"] = 0;
  (*m_param)["tagprob"] = 0.7;
  (*m_param)["fakeprob"] = 0.01;

  m_WModel = new Deconstruction::WDecayModel(*m_param);
  m_topModel = new Deconstruction::TopGluonModel(*m_param);
  m_bkgModel = new Deconstruction::BackgroundModel(*m_param);
  m_isrModel = new Deconstruction::ISRModel(*m_param);

  Deconstruction::Model *sigModelPtr = 0;
  if(signalModel == WDecayModel) {
    sigModelPtr = m_WModel;
  }
  else if(signalModel == TopGluonModel) {
    sigModelPtr = m_topModel;
  }
  else {
    cout << "No valid model set" << endl;
  }

  m_deconstruct = new Deconstruction::Deconstruct(*m_param, *sigModelPtr, *m_bkgModel, *m_isrModel);
#endif // not NO_SHOWERDECONSTRUCTION
}

ShowerDeconstruction::~ShowerDeconstruction()
{
#ifndef NO_SHOWERDECONSTRUCTION
  if (m_param) delete m_param;
  if (m_topModel) delete m_topModel;
  if (m_WModel) delete m_WModel;
  if (m_bkgModel) delete m_bkgModel;
  if (m_isrModel) delete m_isrModel;
  if (m_deconstruct) delete m_deconstruct;
#endif // not NO_SHOWERDECONSTRUCTION
}

double ShowerDeconstruction::result(const xAOD::Jet &jet)
{
  std::vector<fastjet::PseudoJet> constit_pseudojets = jet::JetConstituentFiller::constituentPseudoJets(jet);
  fastjet::PseudoJet pjet = fastjet::join(constit_pseudojets);
  return result(pjet, jet.getSizeParameter());
}

double ShowerDeconstruction::result(const fastjet::PseudoJet &jet, const float R)
{
  // disable warning due to conditionally compiled code
  (void) jet;
  (void) R;
#ifndef NO_SHOWERDECONSTRUCTION
  if(jet.constituents().size() == 0) return -999;

  (*m_param)["R"] = R;
  fastjet::JetDefinition microjet_def(fastjet::cambridge_algorithm, 0.2);
  fastjet::ClusterSequence microjet_cs(jet.constituents(), microjet_def);
  float pt_cut = 20e3;
  vector<fastjet::PseudoJet> microjets = fastjet::sorted_by_pt(microjet_cs.inclusive_jets(pt_cut));

  if(microjets.size() > 9) {
    microjets.erase(microjets.begin() + (int) 9, microjets.begin() + microjets.size());
  }
  for (unsigned int k = 0; k < microjets.size(); ++k) {
    microjets[k] *= 1e-3;
    // apply an 100 MeV shift, to avoid singularities in SD for some cases
    // the calibration usually solves this problem
    double shift = 0;
    if (microjets[k].m() < 0.1) {
      shift = 0.1;
    }
    microjets[k].reset_momentum(microjets[k].px(), microjets[k].py(), microjets[k].pz(), microjets[k].e()+shift);
  }

  double chi = -100;
  double wSignal = -100, wBackground = -100;
  chi =m_deconstruct->deconstruct(microjets, wSignal, wBackground);
  double lchi = -100;
  if (chi > 0) lchi = std::log(chi);

  return lchi;
#else
  return -999;
#endif // not NO_SHOWERDECONSTRUCTION
}
