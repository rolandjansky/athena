/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "FexSum.h"
#include "TrigInterfaces/HLTCheck.h"
#include "TrigEFMissingET/METComponent.h"
#include "xAODTrigMissingET/TrigMissingET.h"

namespace HLT { namespace MET {
  FexSum::FexSum(const std::string& name, ISvcLocator* pSvcLocator) :
    HLT::AllTEAlgo(name, pSvcLocator)
  {
    declareProperty("MissingETOutputKey", m_teName, "The output TE name");
    declareProperty("SaveComponents", m_sumComponents=true,
        "Whether to save the components or only save the sum");
  }

  HLT::ErrorCode FexSum::hltInitialize() 
  {
    return HLT::OK;
  }
  HLT::ErrorCode FexSum::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode FexSum::hltExecute(
      std::vector<std::vector<HLT::TriggerElement*>>& tes_in,
      unsigned int type_out)
  {
    // Keep track of the TEs that seeded our output
    HLT::TEVec seeds;
    std::vector<std::string> names;
    std::vector<METComponent> components;
    METComponent sum;
    for (const std::vector<HLT::TriggerElement*>& te_in : tes_in) {
      if (te_in.size() != 1) {
        ATH_MSG_ERROR("Unexpected number of input TEs - expect fullscan RoI");
        return HLT::NAV_ERROR;
      }
      const xAOD::TrigMissingET* met = nullptr;
      HLT_CHECK( getFeature(te_in.at(0), met) );
      seeds.push_back(te_in.at(0) );
      sum += METComponent(*met);
      if (m_sumComponents) {
        for (std::size_t ii = 0; ii < met->getNumberOfComponents(); ++ii) {
          std::string name = met->nameOfComponent(ii);
          auto itr = std::find(names.begin(), names.end(), name);
          if (itr == names.end() ) {
            // New component - need to make a new object to hold it
            names.push_back(name);
            components.emplace_back(ii, *met);
          }
          else {
            auto compItr = components.begin() + std::distance(names.begin(), itr);
            *compItr += METComponent(ii, *met);
          }
        }
      }
    }

    // Now create the output
    auto met = std::make_unique<xAOD::TrigMissingET>();
    // Create a private store as it never meets its real auxstore in this class.
    met->makePrivateStore();
    // If we asked for components, make sure we make space for them.
    if (m_sumComponents)
      met->defineComponents(names);
    sum.fillMET(*met);
    for (std::size_t ii = 0; ii < components.size(); ++ii)
      components.at(ii).fillMETComponent(ii, *met);

    // Attach the feature to the output
    HLT::TriggerElement* outputTE =
      config()->getNavigation()->addNode(seeds, type_out);
    HLT_CHECK(attachFeature(outputTE, met.release(), m_teName) );
    return HLT::OK;
  }


} } //> end namespace HLT::MET
