/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

/**********************************************************************
 * Authors:
 *      Carlo Varni <carlo.varni@cern.ch>
 *      Carlo Schiavi <carlo.schiavi@cern.ch>
 *      Florencia Daneri <maria.florencia.daneri@cern.ch>
 *      Gino Marceca <gino.marceca@cern.ch>
 *      Lars Beemster <lars.beemster@cern.ch>
 *
 * Description:
 *      Base tool class for bjet trigger emulation
 **********************************************************************/

#include "src/TrigBtagEmulationTool.h"

namespace Trig {  

TrigBtagEmulationTool::TrigBtagEmulationTool(const std::string& type, 
					     const std::string& name, 
					     const IInterface* parent)
  : base_class(type, name, parent) 
{}


StatusCode TrigBtagEmulationTool::initialize() {
  ATH_MSG_INFO( "Initializing " << name() );

  ATH_CHECK( m_trigDec.retrieve() );

  ATH_CHECK( m_manager_PFlow_cnt.retrieve() );  
  ATH_CHECK( m_manager_EMTopo_presel.retrieve() );
  
  for(const auto& [chain_name, definition] : m_emulatedChainDefinitions) {
    ATH_CHECK( addEmulatedChain(chain_name, definition) );
  }

  m_dl2 = std::make_unique<FlavorTagDiscriminants::DL2HighLevel>("BTagging/20210519r22/dl1d/antikt4empflow/network.json",
								 FlavorTagDiscriminants::FlipTagConfig::STANDARD,
								 m_remapping.value());
  
  return StatusCode::SUCCESS;
}

bool TrigBtagEmulationTool::isPassed(const std::string& chain) const 
{
  // This will re-compute everything - slow version
  const auto& emulContext = populateJetManagersTriggerObjects();
  return isPassed(chain, emulContext);
}

bool TrigBtagEmulationTool::isPassed(const std::string& chain,
				     const EmulContext& emulCtx) const 
{
  auto itr = m_emulatedChains.find(chain);
  if (itr == m_emulatedChains.end()) {
    ATH_MSG_WARNING( "Chain " << chain << " requested but not in the list of emulated chains" );
    return false;
  }
  return isPassed(*(itr->second.get()), emulCtx);
}

bool TrigBtagEmulationTool::isPassed(const TrigBtagEmulationChain& chain,
				     const EmulContext& emulCtx) const
{
  return evaluate_L1(chain, emulCtx) and evaluate_HLT(chain, emulCtx);
}

bool TrigBtagEmulationTool::evaluate_L1(const TrigBtagEmulationChain& chain,
					const EmulContext&) const
{ 
  // IMPROVE: simulate L1 as well, don't rely on TDT
  bool res = m_trigDec->isPassed(chain.l1_requirement());
  ATH_MSG_DEBUG( name() << " passed L1: " << (res ? "YES":"NO") );
  return res;
}


bool TrigBtagEmulationTool::evaluate_HLT(const TrigBtagEmulationChain& chain,
					 const EmulContext& emulCtx) const
{
  for ( const auto& is_chainPart_PFlow : chain.is_PFlow() ) {
    if ( not is_chainPart_PFlow ) {
      ATH_MSG_ERROR( "Only PFlow jets are supported at this moment" );
      return false;
    }
  }

  const std::vector<TrigBtagEmulationJet>& emtopo_presel_jets =  m_manager_EMTopo_presel->getSortedPreselJets(emulCtx);
  // preselection
  if (not evaluate_preselection(chain, emtopo_presel_jets) ) return false;
  // dijet mass
  if (not evaluate_dijetmass(chain, emtopo_presel_jets) ) return false;

  // computation
  const int shared_idx = chain.shared_idx();
  const std::vector<int>& jet_multiplicity = chain.jet_multiplicity();
  const std::vector<std::string>& chainPartName = chain.chainPartName();
  const std::vector<TrigBtagEmulationJet>& pflow_jets = m_manager_PFlow_cnt->getJets(emulCtx);

  int nChainParts = static_cast<int>( jet_multiplicity.size() );

  bool res = false;
  if( shared_idx ) {
    // chain uses the shared operator, we basically have two chains to check
    std::vector<std::vector<bool>> chainPart_passedjets = evaluate_HLT_chainParts(chain, pflow_jets, 0, shared_idx);
    std::vector<std::vector<bool>> chainPart_passedjets_btag = evaluate_HLT_chainParts(chain, pflow_jets, shared_idx, nChainParts);
      
    std::vector<std::string> chainPartNames_leg1(chainPartName.begin(), chainPartName.begin() + shared_idx);
    std::vector<int> multiplicities_leg1(jet_multiplicity.begin(), jet_multiplicity.begin() + shared_idx);
    std::vector<std::string> chainPartNames_leg2(chainPartName.begin() + shared_idx, chainPartName.end());
    std::vector<int> multiplicities_leg2(jet_multiplicity.begin() + shared_idx, jet_multiplicity.end());
      
    res = allocate_jets_to_chainParts(chainPart_passedjets, chainPartNames_leg1, multiplicities_leg1) &&
      allocate_jets_to_chainParts(chainPart_passedjets_btag, chainPartNames_leg2, multiplicities_leg2);
  }
  else {
    std::vector<std::vector<bool>> chainPart_passedjets = evaluate_HLT_chainParts(chain, pflow_jets, 0, nChainParts);
      
    std::vector<std::string> chainPartNames(chainPartName.begin(), chainPartName.end());
    std::vector<int> multiplicities(jet_multiplicity.begin(), jet_multiplicity.end());
      
    res = allocate_jets_to_chainParts(chainPart_passedjets, chainPartNames, multiplicities);
  }

  return res; 
}

bool TrigBtagEmulationTool::evaluate_preselection(const TrigBtagEmulationChain& chain,
						  const std::vector<TrigBtagEmulationJet>& preselJets) const
{
  const std::vector<std::string>& jet_presel = chain.jet_presel();
  if (jet_presel.size() == 0) return true;

  int presel_multiplicity = 1;
  double presel_ptcut = 0.;
  double presel_eta_min = 0.;
  double presel_eta_max = 0.;

  bool res = false;
  if( preselJets.size() == 1 ) {
    // single preselection cut
    const std::string presel_part = jet_presel[0];
    int preseljets_passed = 0;
    
    parse_preselection(presel_part, presel_multiplicity, presel_ptcut, presel_eta_min, presel_eta_max);
    
    for( const auto& preseljet: preselJets ) {
      if((preseljet.pt() / Gaudi::Units::GeV) >= presel_ptcut &&
	 presel_eta_min < std::fabs(preseljet.eta()) && std::fabs(preseljet.eta()) < presel_eta_max) {
	
	ATH_MSG_DEBUG( "   - Preselection jet pt=" << (preseljet.pt() / Gaudi::Units::GeV) << " eta=" << preseljet.eta() << " passed.");
	preseljets_passed++;
      }
      else if((preseljet.pt() / Gaudi::Units::GeV) < presel_ptcut) {
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
    std::vector<std::vector<bool>> chainPart_passedjets(jet_presel.size());
    size_t chainPart_passedjets_lenmax = 0;
    std::vector<int> multiplicities;
    
    for(size_t chainPart_idx = 0; chainPart_idx < chainPart_passedjets.size(); chainPart_idx++) {
      parse_preselection(jet_presel[chainPart_idx], presel_multiplicity, presel_ptcut, presel_eta_min, presel_eta_max);
      multiplicities.push_back(presel_multiplicity);
      
      for ( const auto& preseljet : preselJets ) {
	if((preseljet.pt() / Gaudi::Units::GeV) >= presel_ptcut &&
	   presel_eta_min < std::fabs(preseljet.eta()) && std::fabs(preseljet.eta()) < presel_eta_max) {
	  // flag jet as passed this part of preselection
	  chainPart_passedjets[chainPart_idx].push_back(true);
	}
	else {
	  chainPart_passedjets[chainPart_idx].push_back(false);
	}
	
	if((preseljet.pt() / Gaudi::Units::GeV) < presel_ptcut) {
	  // no more Preselection jets left with high enough pt
	  if(chainPart_passedjets[chainPart_idx].size() > chainPart_passedjets_lenmax) {
	    chainPart_passedjets_lenmax = chainPart_passedjets[chainPart_idx].size();
	  }
	  break;
	}
      }
    }
							
    for( auto& chainPart_passedjets_row : chainPart_passedjets ) {
      // make all rows in the chainPart_jet_matrix equal length
      while(chainPart_passedjets_row.size() < chainPart_passedjets_lenmax) {
	chainPart_passedjets_row.push_back(false);
      }
    }
    
    res = allocate_jets_to_chainParts(chainPart_passedjets, jet_presel, multiplicities);
  }

  return res;
}

bool TrigBtagEmulationTool::evaluate_dijetmass(const TrigBtagEmulationChain& chain, 
					       const std::vector<TrigBtagEmulationJet>& preselJets) const
{
  const std::string& dijetmass = chain.dijetmass();
  if ( dijetmass == "None") return true;

  double dijet_mass = 0.;
  double dijet_minjetpt = 0.;
  double dijet_dphi = -1.;
  double dijet_deta = -1.;
  parse_dijetmass(dijetmass, dijet_mass, dijet_minjetpt, dijet_dphi, dijet_deta);

  bool res = false;
  for( const auto& jet1: preselJets ) {
    if( (jet1.pt() / Gaudi::Units::GeV) < dijet_minjetpt ) {
      break;
    }
    
    for( const auto& jet2: preselJets ) {
      if( (jet2.pt() / Gaudi::Units::GeV) < dijet_minjetpt ) {
	break;
      }
      
      double mass = (jet1.p4() + jet2.p4()).M(); // MeV, since PreselJet pt is in MeV too
      double adphi = std::abs(jet1.p4().DeltaPhi(jet2.p4()));
      double adeta = std::abs(jet1.eta() - jet2.eta());
      
      if((mass / Gaudi::Units::GeV) >= dijet_mass &&
	 (dijet_dphi < 0 || adphi < dijet_dphi) &&
	 (dijet_deta < 0 || adeta > dijet_deta) ) {
	ATH_MSG_DEBUG( "   - Dijet system " << (jet1.pt() / Gaudi::Units::GeV) << " and " << (jet2.pt() / Gaudi::Units::GeV) << " passed " << dijetmass );
	res = true;
	break;
      }
    }
  }
  
  return res;
}

void TrigBtagEmulationTool::parse_preselection(const std::string& presel_part, 
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
  
void TrigBtagEmulationTool::parse_dijetmass(const std::string& dijetmass,
					    double& dijet_mass, 
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
  dijet_mass = static_cast<double>( std::stoi(dijetmass) ); // stoi just parses until it finds a nonnumeric, provided is something like 1000j50dphi200x400deta
  dijet_minjetpt = static_cast<double>( std::stoi(dijetmass.substr(dijetmass.find("j") +1)) );
  
  if(dijetmass.find("dphi") != std::string::npos) {
    dijet_dphi = static_cast<double>( (std::stoi(dijetmass.substr(dijetmass.find("dphi") + 4)))) / 1e2 ; // 4 -> strlen("dphi")
  }
  
  if(dijetmass.find("deta") != std::string::npos) {
    std::string deta_str = dijetmass.substr(dijetmass.find("deta") -6);
    dijet_deta = static_cast<double>( (std::stoi(dijetmass.substr(dijetmass.find("x") +1)))) / 1e2;
  }
  
  ATH_MSG_DEBUG( "DijetMass parsing result: " << dijetmass << " ->"
		 << " m:" << dijet_mass
		 << " j:" << dijet_minjetpt
		 << " dphi:" << dijet_dphi
		 << " deta:" << dijet_deta);
}  
  
bool TrigBtagEmulationTool::allocate_jets_to_chainParts(const std::vector<std::vector<bool>>& chainPart_jet_matrix, 
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
  
std::vector<std::vector<bool>> TrigBtagEmulationTool::evaluate_HLT_chainParts(const TrigBtagEmulationChain& chain,
									      const std::vector<TrigBtagEmulationJet>& jets,
									      int idx_begin, 
									      int idx_end) const 
{
  const std::vector<double>& jet_pt_vec = chain.jet_pt();
  const std::vector<double>& jet_eta_min_vec = chain.jet_eta_min();
  const std::vector<double>& jet_eta_max_vec = chain.jet_eta_max();
  const std::vector<double>& jvt_vec = chain.jvt();
  const std::vector<std::string>& tagger_vec = chain.tagger(); 
  const std::vector<std::string>& chainPartName_vec = chain.chainPartName();

  std::vector<std::vector<bool>> chainPart_passedjets(idx_end - idx_begin);
  
  for(size_t chainPart_idx = 0; chainPart_idx < chainPart_passedjets.size(); chainPart_idx++) {
    double jet_pt = jet_pt_vec[chainPart_idx +idx_begin];
    double jet_eta_min = jet_eta_min_vec[chainPart_idx +idx_begin];
    double jet_eta_max = jet_eta_max_vec[chainPart_idx +idx_begin];
    double jvtcut = jvt_vec[chainPart_idx +idx_begin];
    std::string tagger = tagger_vec[chainPart_idx +idx_begin];
    
    ATH_MSG_DEBUG( ">-- " << chainPartName_vec[chainPart_idx +idx_begin] << ":" );
    for( const auto& jet : jets ) {
      float jvt = jet.jvt();
      bool passedJvt = jvt > jvtcut ||
	jet.pt() / Gaudi::Units::GeV > 120. ||
	std::fabs(jet.eta()) > 2.5;
      
      if (tagger == "ewTagger") tagger = "newTagger";
      bool is_bjet = isPassedBTagger(jet, tagger);
      
      if(jet.pt() > jet_pt &&
	 std::fabs(jet.eta()) > jet_eta_min &&
	 std::fabs(jet.eta()) < jet_eta_max &&
	 passedJvt &&
	 ((tagger != "") ? is_bjet : true) ) {
	// flag jet as passing current chainPart
	chainPart_passedjets[chainPart_idx].push_back(true);
      }
      else {
	chainPart_passedjets[chainPart_idx].push_back(false);
      }
      
      ATH_MSG_DEBUG( " - pt:" << (jet.pt() / Gaudi::Units::GeV)
		     << ", eta:" << jet.eta()
		     << ", jvt:" << jvt
		     << ((tagger != "") ? ", btag:" : "") << ((tagger != "") ? (is_bjet?"Y":"N") : "")
		     << ", pass:" << (chainPart_passedjets[chainPart_idx].back()?"Y":"N")
		     );
    }
  }
  
  return chainPart_passedjets;
}


//=================================================
//		Utility functions
//=================================================
const EmulContext& TrigBtagEmulationTool::populateJetManagersTriggerObjects() const {
  EmulContext *emulCtx = new EmulContext();

  // Check if input chain exists
  if ( checkInputChainExists( m_inputChains_PFlow ).isFailure() )
    return *emulCtx;

  // Retrieve input container vectors  
  if ( retrieveTriggerObjects( *m_manager_PFlow_cnt, *emulCtx ).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve trigger objects from " << m_manager_PFlow_cnt->name());
    return *emulCtx;
  }

  if ( retrieveTriggerObjects( *m_manager_EMTopo_presel, *emulCtx ).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve trigger objects from " << m_manager_EMTopo_presel->name());
    return *emulCtx;
  }
  
  return *emulCtx;
}

StatusCode TrigBtagEmulationTool::retrieveTriggerObjects(const Trig::JetManagerTool& manager,
							 EmulContext& emulCtx) const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_CHECK(manager.retrieveByContainer(ctx, emulCtx));
  
  const std::string& jetContainerName = manager.jetContainerName();
  const unsigned int jetSize = manager.getJets(emulCtx).size();
  
  ATH_MSG_DEBUG( "Size of input containers ['" << jetContainerName << "'] :"
  		 << " jet=" << jetSize);
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagEmulationTool::checkInputChainExists( const std::string &inputChain ) const 
{
  std::vector<std::string> listOfTriggers = m_trigDec->getChainGroup( inputChain )->getListOfTriggers();
  if ( listOfTriggers.empty() ) {
    ATH_MSG_ERROR( "Input Chain ('" << inputChain << "') is not in the list of available triggers: {" << m_trigDec->getListOfTriggers(".*") << "}" );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG( "Triggers in configured ChainGroup " << inputChain << ":" );
  for (const auto& cname: listOfTriggers) {
    ATH_MSG_DEBUG( " └ " << cname );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagEmulationTool::addEmulatedChain(const std::string& triggerName, 
						   const std::vector<std::string>& definition) 
{
  if (m_emulatedChains.find(triggerName) != m_emulatedChains.end()) {
    ATH_MSG_ERROR("Chain " << triggerName << " has already been added in the Emulation List.");
    return StatusCode::FAILURE;
  }
  auto chain = std::make_unique<TrigBtagEmulationChain>( triggerName, definition );
  chain->setLevel(msgLevel());  
  m_emulatedChains.insert( std::make_pair(triggerName, std::move(chain)) );
  
  return StatusCode::SUCCESS;
}

bool TrigBtagEmulationTool::isPassedBTagger(const TrigBtagEmulationJet& jet,
					    const std::string& btagger) const
{
  if (btagger.empty()) return false;
  if (btagger == "offperf") return true;

  const auto& itr = m_tagger_wp.find(btagger);
  if (itr == m_tagger_wp.end()) return false;

  double workingPoint = itr->second;
  bool res = false;

  if( btagger.substr(0, 4) == "dl1r" ) { // 4 -> strlen("dl1r")
    res = jet.satisfy("DL1r", workingPoint);
  } else if( btagger.substr(0, 4) == "dl1d" ) { // 4 -> strlen("dl1d")
    res = jet.satisfy("DL1d20211216", workingPoint);
  } else if( btagger == "newTagger" ) {
    const xAOD::BTagging *btag = jet.btag();
    if (not btag) return false;
    m_dl2->decorate(*btag);
    
    res = jet.satisfy("DL1dEMUL", workingPoint);
  } else {
    ATH_MSG_WARNING( "Tagger " << btagger << " not supported." );
  }

  return res;
}

}
