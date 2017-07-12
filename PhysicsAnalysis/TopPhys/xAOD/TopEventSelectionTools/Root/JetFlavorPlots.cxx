/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/JetFlavorPlots.h"

#include <cmath>
#include <array>
#include <string>

#include "TopEvent/EventTools.h"
#include "TopCorrections/ScaleFactorRetriever.h"

#include "TH1.h"
#include "TH2D.h"
#include "TFile.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

namespace top {

const double JetFlavorPlots::toGeV = 0.001;

JetFlavorPlots::JetFlavorPlots(const std::string& name,
                               TFile* outputFile, EL::Worker* wk) :
    m_hists(name, outputFile, wk), m_nominalHashValue(0) {
    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();

    m_hists.addHist("gluon_jets", ";pT [GeV];Eta;Events",
                    50, 0., 500., 10, 0, 5);
    m_hists.addHist("quark_jets", ";pT [GeV];Eta;Events",
                    50, 0., 500., 10, 0, 5);
}

bool JetFlavorPlots::apply(const top::Event& event) const {
    // only nominal
    if (event.m_hashValue != m_nominalHashValue)
        return true;

    // only plot tight selection
    // before, we were plotting tight and loose together
    if (event.m_isLoose)
        return true;

    double eventWeight = 1.;
    if (top::isSimulation(event))
//       eventWeight = event.m_info->mcEventWeight();
      eventWeight = event.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
    else  // Only do this for MC
      return true;

    for (const auto* const jetPtr : event.m_jets) {
      int jet_flavor = -99;
      bool status = jetPtr->getAttribute<int>("PartonTruthLabelID", jet_flavor);
      if (!status) continue;
      if (jet_flavor == 21) {  // PDG ID for gluon is 21
        static_cast<TH2D*>(m_hists.hist("gluon_jets"))->Fill(jetPtr->pt()*toGeV,
                                                             jetPtr->eta(),
                                                             eventWeight);
      } else {
        static_cast<TH2D*>(m_hists.hist("quark_jets"))->Fill(jetPtr->pt()*toGeV,
                                                             jetPtr->eta(),
                                                             eventWeight);
      }
    }

    return true;
}

std::string JetFlavorPlots::name() const {
    return "JETFLAVORPLOTS";
}

}  // namespace top
