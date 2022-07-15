
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
                                               const std::vector< std::string >& definition,
                                               PublicToolHandle< Trig::TrigDecisionTool >& tdt )
  : AthMessaging("TrigBtagEmulationChain." + name),
    m_name( name ),
    m_trigDec( tdt )
{
  parseChainDefinition(definition);
}

void TrigBtagEmulationChain::parseChainDefinition(const std::vector< std::string >& definition)
{
  if(definition.size() > 1) {
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
  else {
    ATH_MSG_ERROR( "Couldn't parse chain definition: " << definition[0] );
  }
}

StatusCode TrigBtagEmulationChain::addJetManager(const std::string& name, 
						 ToolHandle<Trig::JetManagerTool>& jm) 
{
  if (m_jetmanagers.find(name) != m_jetmanagers.end()) {
    ATH_MSG_ERROR("Trying to store a JetManager that is already in storage!");
    return StatusCode::FAILURE;
  }

  m_jetmanagers[name] = &jm;
  return StatusCode::SUCCESS;
}

bool TrigBtagEmulationChain::isPassed() const {
  return evaluate_L1() and evaluate_HLT();
}

bool TrigBtagEmulationChain::evaluate_L1() const {
  // IMPROVE: simulate L1 as well, don't rely on TDT
  bool res = tdt()->isPassed(m_l1_requirement);
  ATH_MSG_DEBUG( name() << " passed L1: " << (res ? "YES":"NO") );
  return res;
}

bool TrigBtagEmulationChain::evaluate_HLT() const
{
  bool res = false;
  
  bool is_all_PFlow = true;
  for(const auto& is_chainPart_PFlow: m_is_PFlow) {
    if(!is_chainPart_PFlow) {
      is_all_PFlow = false;
      break;
    }
  }
  
  if(is_all_PFlow) {
    const auto& jm_pflow_cnt = *m_jetmanagers.at("JM_PFlow_CNT");
    const auto& jm_emtopo_presel = *m_jetmanagers.at("JM_EMTopo_PRESEL");
    
    bool no_preselection_cut = (m_jet_presel.size() == 0);
    bool no_dijetcut = (m_dijetmass == "None");
    if(jm_pflow_cnt && (jm_emtopo_presel || (no_preselection_cut && no_dijetcut))) {
      if((no_preselection_cut || evaluate_preselection(jm_emtopo_presel)) &&
	 (no_dijetcut || evaluate_dijetmass(jm_pflow_cnt))) {
	// if we got here, the preselection and dijet mass cuts are already satisfied
	int nChainParts = static_cast<int>( m_jet_multiplicity.size() );
	if(m_shared_idx) {
	  // chain uses the shared operator, we basically have two chains to check
	  std::vector<std::vector<bool>> chainPart_passedjets = evaluate_HLT_chainParts(jm_pflow_cnt, 0, m_shared_idx);
	  std::vector<std::vector<bool>> chainPart_passedjets_btag = evaluate_HLT_chainParts(jm_pflow_cnt, m_shared_idx, nChainParts);
	  
	  std::vector<std::string> chainPartNames_leg1 = std::vector(m_chainPartName.begin(), m_chainPartName.begin() +m_shared_idx);
	  std::vector<int> multiplicities_leg1 = std::vector(m_jet_multiplicity.begin(), m_jet_multiplicity.begin() +m_shared_idx);
	  std::vector<std::string> chainPartNames_leg2 = std::vector(m_chainPartName.begin() +m_shared_idx, m_chainPartName.end());
	  std::vector<int> multiplicities_leg2 = std::vector(m_jet_multiplicity.begin() +m_shared_idx, m_jet_multiplicity.end());
	  
	  res = allocate_jets_to_chainParts(chainPart_passedjets, chainPartNames_leg1, multiplicities_leg1) &&
	    allocate_jets_to_chainParts(chainPart_passedjets_btag, chainPartNames_leg2, multiplicities_leg2);
	}
	else {
	  std::vector<std::vector<bool>> chainPart_passedjets = evaluate_HLT_chainParts(jm_pflow_cnt, 0, nChainParts);
	  
	  std::vector<std::string> chainPartNames = std::vector(m_chainPartName.begin(), m_chainPartName.end());
	  std::vector<int> multiplicities = std::vector(m_jet_multiplicity.begin(), m_jet_multiplicity.end());
	  
	  res = allocate_jets_to_chainParts(chainPart_passedjets, chainPartNames, multiplicities);
	}
      }
    }
    else {
      ATH_MSG_ERROR( "Could not retrieve JetManagers" );
    }
  }
  else {
    ATH_MSG_ERROR( "Only PFlow jets are supported at this moment" );
  }
  
  
  return res;
}


bool TrigBtagEmulationChain::evaluate_preselection(const ToolHandle<Trig::JetManagerTool>& jm) const 
{
  bool res = false;
  int presel_multiplicity = 1;
  double presel_ptcut = 0.;
  double presel_eta_min = 0.;
  double presel_eta_max = 0.;
  
  if(m_jet_presel.size() == 1) {
    // single preselection cut
    std::string presel_part = m_jet_presel[0];
    int preseljets_passed = 0;
    
    parse_preselection(presel_part, presel_multiplicity, presel_ptcut, presel_eta_min, presel_eta_max);
    
    for( const auto& preseljet: jm->getSortedPreselJets() ) {
      if((preseljet.pt / Gaudi::Units::GeV) >= presel_ptcut &&
	 presel_eta_min < std::fabs(preseljet.eta) && std::fabs(preseljet.eta) < presel_eta_max) {
	
	ATH_MSG_DEBUG( "   - Preselection jet pt=" << (preseljet.pt / Gaudi::Units::GeV) << " eta=" << preseljet.eta << " passed.");
	preseljets_passed++;
      }
      else if((preseljet.pt / Gaudi::Units::GeV) < presel_ptcut) {
	// no more Preselection jets left with high enough pt
	break;
      }
      
      if(preseljets_passed >= presel_multiplicity) {
	res = true;
	break;
      }
    }
  }
  else {
    // multi-threshold preselection
    std::vector<std::vector<bool>> chainPart_passedjets(m_jet_presel.size());
    size_t chainPart_passedjets_lenmax = 0;
    std::vector<int> multiplicities;
    for(size_t chainPart_idx = 0; chainPart_idx < chainPart_passedjets.size(); chainPart_idx++) {
      parse_preselection(m_jet_presel[chainPart_idx], presel_multiplicity, presel_ptcut, presel_eta_min, presel_eta_max);
      multiplicities.push_back(presel_multiplicity);
      
      for( const auto& preseljet: jm->getSortedPreselJets() ) {
	if((preseljet.pt / Gaudi::Units::GeV) >= presel_ptcut &&
	   presel_eta_min < std::fabs(preseljet.eta) && std::fabs(preseljet.eta) < presel_eta_max) {
	  // flag jet as passed this part of preselection
	  chainPart_passedjets[chainPart_idx].push_back(true);
	}
	else {
	  chainPart_passedjets[chainPart_idx].push_back(false);
	}
	
	if((preseljet.pt / Gaudi::Units::GeV) < presel_ptcut) {
	  // no more Preselection jets left with high enough pt
	  if(chainPart_passedjets[chainPart_idx].size() > chainPart_passedjets_lenmax) {
	    chainPart_passedjets_lenmax = chainPart_passedjets[chainPart_idx].size();
	  }
	  break;
	}
      }
    }
    
    for(auto& chainPart_passedjets_row: chainPart_passedjets) {
      // make all rows in the chainPart_jet_matrix equal length
      while(chainPart_passedjets_row.size() < chainPart_passedjets_lenmax) {
	chainPart_passedjets_row.push_back(false);
      }
    }
    
    res = allocate_jets_to_chainParts(chainPart_passedjets, m_jet_presel, multiplicities);
  }
  
  return res;
}

bool TrigBtagEmulationChain::evaluate_dijetmass(const ToolHandle<Trig::JetManagerTool>& jm) const
{
  bool res = false;
  double dijet_mass = 0.;
  double dijet_minjetpt = 0.;
  double dijet_dphi = -1.;
  double dijet_deta = -1.;
  parse_dijetmass(dijet_mass, dijet_minjetpt, dijet_dphi, dijet_deta);
  
  const auto& preselJets = jm->getSortedPreselJets();
  for( const auto& jet1: preselJets ) {
    if( (jet1.pt / Gaudi::Units::GeV) < dijet_minjetpt ) {
      break;
    }
    
    for( const auto& jet2: preselJets ) {
      if( (jet2.pt / Gaudi::Units::GeV) < dijet_minjetpt ) {
	break;
      }
      
      double mass = (jet1.p4 + jet2.p4).M(); // MeV, since PreselJet pt is in MeV too
      double adphi = std::abs(jet1.p4.DeltaPhi(jet2.p4));
      double adeta = std::abs(jet1.eta - jet2.eta);
      
      if((mass / Gaudi::Units::GeV) >= dijet_mass &&
	 (dijet_dphi < 0 || adphi < dijet_dphi) &&
	 (dijet_deta < 0 || adeta > dijet_deta) ) {
	ATH_MSG_DEBUG( "   - Dijet system " << (jet1.pt / Gaudi::Units::GeV) << " and " << (jet2.pt / Gaudi::Units::GeV) << " passed " << m_dijetmass );
	res = true;
	break;
      }
    }
  }
  
  return res;
}

std::vector<std::vector<bool>> TrigBtagEmulationChain::evaluate_HLT_chainParts(const ToolHandle<Trig::JetManagerTool>& jm, 
									       int idx_begin, 
									       int idx_end) const {
  std::vector<std::vector<bool>> chainPart_passedjets(idx_end - idx_begin);
  
  for(size_t chainPart_idx = 0; chainPart_idx < chainPart_passedjets.size(); chainPart_idx++) {
    double jet_pt = m_jet_pt[chainPart_idx +idx_begin];
    double jet_eta_min = m_jet_eta_min[chainPart_idx +idx_begin];
    double jet_eta_max = m_jet_eta_max[chainPart_idx +idx_begin];
    double jvtcut = m_jvt[chainPart_idx +idx_begin];
    std::string tagger = m_tagger[chainPart_idx +idx_begin];
    
    ATH_MSG_DEBUG( ">-- " << m_chainPartName[chainPart_idx +idx_begin] << ":" );
    for( const auto& jet: jm->getJets() ) {
      float jvt = jet->jvt();
      bool passedJvt = jvt > jvtcut ||
	jet->pt() / Gaudi::Units::GeV > 120. ||
	std::fabs(jet->eta()) > 2.5;
      
      if (tagger == "ewTagger") tagger = "newTagger";
      bool is_bjet = (tagger != "") and (s_tagger_wp.find(tagger) != s_tagger_wp.end())
	? jet->isPassedBTagger(tagger, s_tagger_wp.at(tagger)) 
	: false;
      
      if(jet->pt() > jet_pt &&
	 std::fabs(jet->eta()) > jet_eta_min &&
	 std::fabs(jet->eta()) < jet_eta_max &&
	 passedJvt &&
	 ((tagger != "") ? is_bjet : true) ) {
	// flag jet as passing current chainPart
	chainPart_passedjets[chainPart_idx].push_back(true);
      }
      else {
	chainPart_passedjets[chainPart_idx].push_back(false);
      }
      
      ATH_MSG_DEBUG( " - pt:" << (jet->pt() / Gaudi::Units::GeV)
		     << ", eta:" << jet->eta()
		     << ", jvt:" << jvt
		     << ((tagger != "") ? ", btag:" : "") << ((tagger != "") ? (is_bjet?"Y":"N") : "")
		     << ", pass:" << (chainPart_passedjets[chainPart_idx].back()?"Y":"N")
		     );
    }
  }
  
  return chainPart_passedjets;
}

bool TrigBtagEmulationChain::allocate_jets_to_chainParts(const std::vector<std::vector<bool>>& chainPart_jet_matrix, 
							 const std::vector<std::string>& chainPartNames, 
							 const std::vector<int>& multiplicities) const
{
  bool res = false;
  std::vector<bool> chainPart_subset_passed;
  
  auto chainPartName_it = chainPartNames.begin();
  for(const auto& chainPart_passedjets: chainPart_jet_matrix) {
    std::string passedJets = "";
    passedJets.reserve(chainPart_passedjets.size());
    for(bool pass: chainPart_passedjets) {
      passedJets += pass ? '1' : '0';
      passedJets += !(passedJets.size() % 5) ? " " : "";
    }
    ATH_MSG_DEBUG( passedJets << " " << *(chainPartName_it++) );
  }
  
  if(chainPart_jet_matrix.size() == 1) {
    // simple single threshold chain
    chainPart_subset_passed.push_back(false);
    int njet_passed = 0;
    for(bool pass: chainPart_jet_matrix[0]) {
      if(pass) {
	njet_passed++;
	if(njet_passed >= multiplicities[0]) {
	  chainPart_subset_passed.back() = true;
	  break;
	}
      }
    }
  }
  else {
    // matrix looks something like this:
    // chainPart0 (mult 1) | x    | pass | x    | x    | pass | x
    // chainPart1 (mult 2) | x    | pass | pass | pass | x    | x
    // chainPart2 (mult 3) | pass | pass | pass | x    | pass | pass
    //                     | j0   | j1   | j2   | j3   | j4   | j5
    //
    // so it is easy to see that every chain _part_ passed, but not trivial to check if the _whole_ chain passed without checking all combinations
    
    // check for every subset in the powerset (chainPart0, ...., chainPartLast) if there are enough jets available that satisfy the subset's chainParts
    for (int subset_bits = 1; subset_bits < (1 << chainPart_jet_matrix.size()); ++subset_bits) {
      std::bitset<32> subset(subset_bits); // This limits the number of chain part to 32
      std::vector<bool> subset_passedJets(chainPart_jet_matrix[0].size(), false);
      int njets_required_subset = 0;
      
      std::string subset_str = "";
      for(size_t chainPart_idx = 0; chainPart_idx < chainPart_jet_matrix.size(); ++chainPart_idx) {
	if(subset[chainPart_idx]) {
	  subset_str += std::to_string(chainPart_idx);
	  njets_required_subset += multiplicities[chainPart_idx];
	  
	  for(size_t jet_idx = 0; jet_idx < chainPart_jet_matrix[chainPart_idx].size(); ++jet_idx) {
	    if(chainPart_jet_matrix[chainPart_idx][jet_idx]) {
	      subset_passedJets[jet_idx] = true;
	    }
	  }
	}
      }
      
      int njets_passed_subset = 0;
      for(bool pass: subset_passedJets) {
	if(pass) {
	  njets_passed_subset++;
	}
      }
      
      ATH_MSG_DEBUG( "Subset {" << subset_str << "} requires " << njets_required_subset << " jets, and has " << njets_passed_subset << " jets passed." );
      
      if(njets_passed_subset < njets_required_subset) {
	chainPart_subset_passed.push_back(false);
	break;
      }
      else {
	chainPart_subset_passed.push_back(true);
      }
    }
  }
  
  
  if(chainPart_subset_passed.size() == static_cast<size_t>(1 << chainPart_jet_matrix.size()) -1 && // -1 because we don't use the empty subset
     !chainPart_subset_passed.empty()) {
    // all chainpart subsets were checked, the result of the last is the overall result
    res = chainPart_subset_passed.back();
  }
  else {
    ATH_MSG_DEBUG( chainPart_subset_passed.size() << " out of " << (1 << chainPart_jet_matrix.size()) -1 << " chain part subsets checked" );
  }
  
  return res;
}

void TrigBtagEmulationChain::parse_preselection(const std::string& presel_part, 
						int& presel_multiplicity, 
						double& presel_ptcut, 
						double& presel_eta_min, 
						double& presel_eta_max) const
{
  char presel_etarange = '?';
  
  presel_multiplicity = 1;
  int tokenizer_offset = 0;
  if('0' <= presel_part[0] && presel_part[0] <= '9') {
    presel_multiplicity = (presel_part[0] - '0');
    tokenizer_offset = 1;
  }
  presel_etarange = presel_part[tokenizer_offset];
  presel_ptcut = static_cast<double>( std::stoi(&presel_part[1 +tokenizer_offset]) ); 
  
  switch(presel_etarange) {
  case 'j':
    presel_eta_max = 3.2;
    break;
  case 'a':
    presel_eta_max = 4.9;
    break;
  case 'c':
    presel_eta_max = 2.4;
    break;
  case 'f':
    presel_eta_min = 3.2;
    presel_eta_max = 4.9;
    break;
  default:
    ATH_MSG_ERROR( "Unknown eta range in jet preselection cut." );
    break;
  }
}

void TrigBtagEmulationChain::parse_dijetmass(double& dijet_mass, 
					     double& dijet_minjetpt, 
					     double& dijet_dphi, 
					     double& dijet_deta) const 
{
  // 'DJMASS500j35
  // 'DJMASS700j35
  // 'DJMASS1000j35
  // 'DJMASS900j50
  // 'DJMASS1000j50
  // 'DJMASS1000j50dphi240
  // 'DJMASS1000j50dphi200x400deta
  dijet_mass = static_cast<double>( std::stoi(m_dijetmass) ); // stoi just parses until it finds a nonnumeric, provided is something like 1000j50dphi200x400deta
  dijet_minjetpt = static_cast<double>( std::stoi(m_dijetmass.substr(m_dijetmass.find("j") +1)) );
  
  if(m_dijetmass.find("dphi") != std::string::npos) {
    dijet_dphi = static_cast<double>( (std::stoi(m_dijetmass.substr(m_dijetmass.find("dphi") + 4)))) / 1e2 ; // 4 -> strlen("dphi")
  }
  
  if(m_dijetmass.find("deta") != std::string::npos) {
    std::string deta_str = m_dijetmass.substr(m_dijetmass.find("deta") -6);
    dijet_deta = static_cast<double>( (std::stoi(m_dijetmass.substr(m_dijetmass.find("x") +1)))) / 1e2;
  }
  
  ATH_MSG_DEBUG( "DijetMass parsing result: " << m_dijetmass << " ->"
		 << " m:" << dijet_mass
		 << " j:" << dijet_minjetpt
		 << " dphi:" << dijet_dphi
		 << " deta:" << dijet_deta);
}

}
