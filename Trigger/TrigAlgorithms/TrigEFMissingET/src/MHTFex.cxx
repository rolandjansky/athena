/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "MHTFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "xAODJet/JetContainer.h"
#include "TrigInterfaces/HLTCheck.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODJet/JetAccessorMap.h"

#include <numeric>

namespace HLT { namespace MET {
  MHTFex::MHTFex(
      const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {
    declareProperty("EtaCut", m_barrelEndcapEta=2.2,
        "Where to make the split between central and forward jets.");
    declareProperty("ApplyTG3Correction", m_applyTG3Corr=false,
        "Whether or not to apply the TileGap3 correction.");
    declareProperty("CentralPtCut", m_centralPtCut = 0.,
        "pT Cut [MeV] to use for central jets");
    declareProperty("ForwardPtCut", m_forwardPtCut = 0.,
        "pT Cut [MeV] to use for forward jets");
  }

  MHTFex::~MHTFex() {}

  HLT::ErrorCode MHTFex::hltInitialize()
  {
    std::vector<std::string> componentNames;
    // Set the bounds of the different regions we consider
    // Note that later parts of the code will assume that these are exclusive
    // regions.
    // JETB1 == positive side of the barrel
    componentNames.push_back("JETB1");
    m_bounds.push_back(std::make_pair(0, m_barrelEndcapEta) );
    // JETB2 == negative side of the barrel
    componentNames.push_back("JETB2");
    m_bounds.push_back(std::make_pair(-m_barrelEndcapEta, 0) );
    // JETE1 == positive side of the endcap
    componentNames.push_back("JETE1");
    m_bounds.push_back(std::make_pair(m_barrelEndcapEta, 5) );
    // JETE2 == negative side of the endcap
    componentNames.push_back("JETE2");
    m_bounds.push_back(std::make_pair(-5, -m_barrelEndcapEta) );
    return initializeBase(componentNames);
  }

  HLT::ErrorCode MHTFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    ATH_MSG_DEBUG("Filling MHT");
    // Get the input jet container
    const xAOD::JetContainer* jets = nullptr;
    if (tes_in.size() != 1 || tes_in.at(0).size() != 1) {
      ATH_MSG_ERROR("Unexpected number of input trigger elements");
      return HLT::NAV_ERROR;
    }
    HLT_CHECK( getFeature(tes_in.at(0).at(0), jets) );
    ATH_MSG_VERBOSE("Retrieved " << jets->size() << " jets");

    // Create METComponent objects to hold the eta split values
    std::vector<METComponent> metSums(m_bounds.size() );
    
    // Now loop over the jets and add them into the relevant places.
    for (const xAOD::Jet* ijet : *jets) {
      TLorentzVector p4 = ijet->p4();
      if (m_applyTG3Corr) {
        // Get the uncalibrated energy and the energy in TileGap3
        static xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> >& acc_ePerSample =
          *xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);
        static xAOD::JetAttributeAccessor::AccessorWrapper<xAOD::JetFourMom_t> acc_uncalibrated("JetConstitScaleMomentum");
        const std::vector<float>& eInSampling = acc_ePerSample.getAttribute(*ijet);
        float e_tileGap3 = eInSampling.at(CaloSampling::TileGap3);
        // Calculate the amount to rescale the jet four-momentum
        float scale = 1 - e_tileGap3/acc_uncalibrated.getAttribute(*ijet).E();
        p4 *= scale;
      }
      float eta = p4.Eta();
      // First check pt cuts
      if (p4.Pt() < (fabs(eta) < m_barrelEndcapEta
                        ? m_centralPtCut
                        : m_forwardPtCut) )
        continue;
      for (std::size_t ii = 0; ii < m_bounds.size(); ++ii)
        if (m_bounds.at(ii).first < eta && eta <= m_bounds.at(ii).second) {
          metSums.at(ii) += p4;
          break;
        }
    }


    // Save the sum over all components
    std::accumulate(metSums.begin(), metSums.end(), METComponent{}).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < metSums.size(); ++ii)
      metSums.at(ii).fillMETComponent(ii, met);

    return HLT::OK;
  }

  HLT::ErrorCode MHTFex::hltFinalize() 
  {
    return HLT::OK;
  }
} } //> end namespace HLT::MET
