/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#include "src/TrigBtagEmulationChain.h"

#define CHAINPART_VARSEP "|"
#define CHAINPART_VARDEF ":"
#define CHAINPART_INDEX(cp,v) ((cp).find(v) +strlen(v))
#define CHAINPART_LENGTH(cp,v) ((cp).substr((cp).find(v) +strlen(v)).find(CHAINPART_VARSEP))
#define CHAINPART_VARIABLE(cp,v) (cp).substr(CHAINPART_INDEX((cp), (std::string(CHAINPART_VARSEP) + v + CHAINPART_VARDEF).c_str()), \
											CHAINPART_LENGTH((cp), (std::string(CHAINPART_VARSEP) + v + CHAINPART_VARDEF).c_str()))


namespace Trig {

TrigBtagEmulationChain::TrigBtagEmulationChain(const std::string& name,
                                               const std::vector< std::string >& definition)
  : AthMessaging("TrigBtagEmulationChain." + name),
    m_name( name )
{
  parseChainDefinition(definition);
}

void TrigBtagEmulationChain::parseChainDefinition(const std::vector< std::string >& definition)
{
  if (definition.size() == 0) {
    ATH_MSG_ERROR( "Chain definition has not been provided" );
    return;
  }

  if (definition.size() == 1) {
    ATH_MSG_ERROR( "Couldn't parse chain definition: " << definition[0] );
    return;
  }
  
  m_l1_requirement = definition[0].substr(7); // 7 -> strlen("L1item:")
  ATH_MSG_DEBUG( name() << ":");
  ATH_MSG_DEBUG( " -            L1 item: " << m_l1_requirement);
  
  std::size_t n_def_size = definition.size();
  m_chainPartName.reserve(n_def_size);
  m_jet_multiplicity.reserve(n_def_size);
  m_jet_pt.reserve(n_def_size);
  m_jet_eta_min.reserve(n_def_size);
  m_jet_eta_max.reserve(n_def_size);
  m_jvt.reserve(n_def_size);
  m_tagger.reserve(n_def_size);
  m_is_PFlow.reserve(n_def_size);
  
  for(size_t chainPart_idx = 1; chainPart_idx < definition.size(); chainPart_idx++) {
    // L1threshold:J20_multiplicity:1_threshold:45_etaRange:0eta290_tagger:bdl1r70_isPFlow:True
    std::string chainPart = definition[chainPart_idx];
    ATH_MSG_DEBUG( " > " << chainPart);
    
    std::string L1threshold = CHAINPART_VARIABLE(chainPart, "L1threshold");
    int jet_multiplicity = std::stoi(CHAINPART_VARIABLE(chainPart, "multiplicity"));
    std::string chainPartName = CHAINPART_VARIABLE(chainPart, "name");
    double jet_pt = static_cast<double>( std::stoi(CHAINPART_VARIABLE(chainPart, "threshold")) * 1e3 );
    std::string etaRange = CHAINPART_VARIABLE(chainPart, "etaRange");
    double jvt = std::stod(CHAINPART_VARIABLE(chainPart, "jvt")) / 1e2;
    std::string tagger = (CHAINPART_VARIABLE(chainPart, "tagger")).size() ? (CHAINPART_VARIABLE(chainPart, "tagger")).substr(1) : "";
    std::string jetpresel = (CHAINPART_VARIABLE(chainPart, "jetpresel")).size() ? (CHAINPART_VARIABLE(chainPart, "jetpresel")).substr(6) : "";
    std::string dijetmass = (CHAINPART_VARIABLE(chainPart, "dijetmass")).size() ? CHAINPART_VARIABLE(chainPart, "dijetmass") : "";
    bool is_PFlow = (CHAINPART_VARIABLE(chainPart, "isPFlow") == "True");
    bool is_shared = (CHAINPART_VARIABLE(chainPart, "isShared") == "True");
    
    double jet_eta_min = static_cast<double>( std::stoi(etaRange) / 100. );
    double jet_eta_max = static_cast<double>( std::stoi(etaRange.substr(etaRange.find("eta") + 3)) / 100. ); // 3 -> strlen("eta")
    
    // jet preselection parsing
    if(jetpresel == "el") { // nopresel (got cut to "el" by substr(6))
      jetpresel = "";
    }
    else {
      auto presel_separator = jetpresel.find("XX");
      while(presel_separator != std::string::npos) {
	m_jet_presel.push_back(jetpresel.substr(0, presel_separator));
	jetpresel = jetpresel.substr(presel_separator + 2); // 2 -> strlen("XX")
	presel_separator = jetpresel.find("XX");
      }
      m_jet_presel.push_back(jetpresel);
    }
    
    
    ATH_MSG_DEBUG( " -       L1 threshold: " << L1threshold);
    ATH_MSG_DEBUG( " -   jet multiplicity: " << jet_multiplicity);
    ATH_MSG_DEBUG( " -      jet threshold: " << jet_pt);
    ATH_MSG_DEBUG( " -          eta range: [" << jet_eta_min << "," << jet_eta_max << "]");
    ATH_MSG_DEBUG( " -            JVT cut: " << jvt);
    ATH_MSG_DEBUG( " - BTagging algorithm: " << tagger);
    ATH_MSG_DEBUG( " -   Jet preselection: " << m_jet_presel);
    ATH_MSG_DEBUG( " -         Dijet Mass: " << dijetmass);
    ATH_MSG_DEBUG( " -           is PFlow: " << (is_PFlow ? "YES":"NO"));
    
    m_chainPartName.push_back(chainPartName);
    m_jet_multiplicity.push_back(jet_multiplicity);
    m_jet_pt.push_back(jet_pt);
    m_jet_eta_min.push_back(jet_eta_min);
    m_jet_eta_max.push_back(jet_eta_max);
    m_jvt.push_back(jvt);
    m_tagger.push_back(tagger);
    m_is_PFlow.push_back(is_PFlow);
    
    if(is_shared) {
      m_shared_idx = chainPart_idx;
    }
    
    if(m_dijetmass.empty() || dijetmass != "None") {
      m_dijetmass = dijetmass;
    }
  }
  
  ATH_MSG_DEBUG( " ^^^         DijetMass: " << m_dijetmass);
  ATH_MSG_DEBUG( " ^^^         is SHARED: " << (m_shared_idx ? "YES":"NO"));
}

}

