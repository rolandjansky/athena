/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include <regex>

#include<boost/algorithm/string.hpp>

#include "GaudiKernel/IIncidentSvc.h"
#include "Gaudi/Property.h"
#include "AthenaInterprocess/Incidents.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigSignatureMoniMT.h"

TrigSignatureMoniMT::TrigSignatureMoniMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  base_class( name, pSvcLocator )
{}

StatusCode TrigSignatureMoniMT::initialize() {
  ATH_CHECK( m_l1DecisionsKey.initialize() );
  ATH_CHECK( m_finalDecisionKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );
  ATH_CHECK( m_L1MenuKey.initialize() );
  ATH_CHECK( m_collectorTools.retrieve() );
  ATH_CHECK( m_histSvc.retrieve() );

  ATH_CHECK( m_incidentSvc.retrieve() );
  m_incidentSvc->addListener(this, AthenaInterprocess::UpdateAfterFork::type());

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::start() {
  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );

  SG::ReadHandle<TrigConf::L1Menu>  l1MenuHandle = SG::makeHandle( m_L1MenuKey );
  bool gotL1Menu =  l1MenuHandle.isValid();

  // reset the state
  m_groupToChainMap.clear();
  m_streamToChainMap.clear();
  m_chainIDToBunchMap.clear();

  //retrieve chain information from menus
  std::vector<std::string> bcidChainNames;
  for ( const TrigConf::Chain& chain: *hltMenuHandle ){
    for ( const std::string& group : chain.groups() ){
      if ( group.find("RATE") == 0 ){
        m_groupToChainMap[group].insert( HLT::Identifier(chain.name()) );
      }
      else if (group == "MON:BCID"){
        bcidChainNames.push_back(chain.name());
      }
    }

    for ( const std::string& stream : chain.streams() ){
      m_streamToChainMap[stream].insert( HLT::Identifier(chain.name()) );
    }

    if( gotL1Menu && !chain.l1item().empty() ) {
      bool isMultiItemSeeded = chain.l1item().find(',') != std::string::npos;
      try {
        std::vector<std::string> seedingItems{};
        if( isMultiItemSeeded ) {
          boost::split(seedingItems, chain.l1item(), boost::is_any_of(","));
        } else {
          seedingItems = { chain.l1item() };
        }
        for( const std::string & itemName : seedingItems ) {
          TrigConf::L1Item item = l1MenuHandle->item(itemName);
          for ( const std::string & group : item.bunchgroups() ) {
            if ( group != "BGRP0" ) {
              m_chainIDToBunchMap[HLT::Identifier(chain.name())].insert(group);
            }
          }
        }
      } catch(std::exception & ex) {
        if( isMultiItemSeeded ) {
          ATH_MSG_INFO("The L1 seed to multi-item-seeded chain " << chain.name() << " could not be completely resolved. This is currently OK. Exception from menu access: " << ex.what());
        } else {
          ATH_MSG_WARNING("The L1 seed to chain " << chain.name() << " could not be resolved. Exception from menu access: " << ex.what());
        }
      }
    }
  }

  auto l1Decisions = SG::makeHandle( m_l1DecisionsKey );
  std::set<std::string> sequencesSet;
  for ( auto& ctool: m_collectorTools ){
    ctool->getSequencesNames(sequencesSet);
  }

  const int x = nBinsX(hltMenuHandle);
  const int xb = nBunchBinsX(hltMenuHandle);
  const int xbc = nBCIDbinsX();
  const int xc = sequencesSet.size();
  const int y = nBinsY();
  const int yr = nRateBinsY();
  const int yb = gotL1Menu ? nBunchBinsY(l1MenuHandle) : 16;
  const int ybc = bcidChainNames.size();
  const int yc = 1; //Rate

  ATH_MSG_DEBUG( "Histogram " << x << " x " << y << " bins");

  std::unique_ptr<TH2> hSA = std::make_unique<TH2I>("SignatureAcceptance", "Raw acceptance of signatures in;chain;step", x, 1, x + 1, y, 1, y + 1);
  std::unique_ptr<TH2> hDC = std::make_unique<TH2I>("DecisionCount", "Positive decisions count per step;chain;step", x, 1, x + 1, y, 1, y + 1);
  std::unique_ptr<TH2> hBG = std::make_unique<TH2I>("BunchGroupCount", "Bunch group count per chain;chain;bunchgroup", xb, 1, xb + 1, yb, 1, yb + 1);

  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + "/SignatureAcceptance", std::move(hSA), m_passHistogram));
  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + "/DecisionCount", std::move(hDC), m_countHistogram));
  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + "/BunchGroupCount", std::move(hBG), m_bunchHistogram));

  ATH_CHECK( initHist( m_passHistogram, hltMenuHandle ) );
  ATH_CHECK( initHist( m_countHistogram, hltMenuHandle ) );
  ATH_CHECK( initBunchHist( m_bunchHistogram, hltMenuHandle, l1MenuHandle ) );

  std::string outputRateName ("Rate" + std::to_string(m_duration) + "s");
  std::string outputBCIDName ("DecisionsPerBCID" + std::to_string(m_duration) + "s");
  std::string outputSequenceName ("SequencesExecutionRate" + std::to_string(m_duration) + "s");

  if ( x > 0 ){
    m_rateHistogram.init(outputRateName, "Rate of positive decisions;chain;step",
                         x, yr, m_bookingPath + "/" + name() + '/' + outputRateName.c_str(), m_histSvc).ignore();
    ATH_CHECK( initHist( m_rateHistogram.getHistogram(), hltMenuHandle, false ) );
    ATH_CHECK( initHist( m_rateHistogram.getBuffer(), hltMenuHandle, false ) );
  }

  if (xc > 0){
    m_sequenceHistogram.init(outputSequenceName, "Rate of sequences execution;sequence;rate",
                             xc, yc, m_bookingPath + "/" + name() + '/' + outputSequenceName.c_str(), m_histSvc).ignore();
    ATH_CHECK( initSeqHist( m_sequenceHistogram.getHistogram(), sequencesSet ) );
    ATH_CHECK( initSeqHist( m_sequenceHistogram.getBuffer(), sequencesSet ) );
  }

  if ( ybc > 0 ){
    m_bcidHistogram.init(outputBCIDName, "Number of positive decisions per BCID per chain;BCID;chain",
                         xbc, ybc, m_bookingPath + "/" + name() + '/' + outputBCIDName.c_str(), m_histSvc).ignore();
    ATH_CHECK( initBCIDhist( m_bcidHistogram.getHistogram(), bcidChainNames ) );
    ATH_CHECK( initBCIDhist( m_bcidHistogram.getBuffer(), bcidChainNames ) );
  }
  else {
    ATH_MSG_DEBUG("No chains configured for BCID monitoring.");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::stop() {
  m_rateHistogram.stopTimer();
  m_bcidHistogram.stopTimer();
  m_sequenceHistogram.stopTimer();

  if (m_chainIDToBinMap.empty()) {
    ATH_MSG_INFO( "No chains configured, no counts to print" );
    return StatusCode::SUCCESS;
  }
  
  auto fixedWidth = [](const std::string& s, size_t sz) {
    std::ostringstream ss;
    ss << std::setw(sz) << std::left << s;
    return ss.str();
  };

  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );

  // retrieve information whether chain was active in Step
  std::map<std::string, std::set<std::string>> chainToSteps;
  std::map<std::string, std::set<int>> chainToStepsId;
  for ( const TrigConf::Chain& chain: *hltMenuHandle ){
    int nstep=1; //let's start from step=1
    for ( const auto& seq : chain.getList("sequencers", true) ){
      // example sequencer name is "Step1_FastCalo_electron", we need only information about Step + number
      const std::string seqName = seq.getValue();
      std::smatch stepNameMatch;
      std::regex_search(seqName.begin(), seqName.end(), stepNameMatch, std::regex("[Ss]tep[0-9]+"));

      std::string stepName = stepNameMatch[0];
      stepName[0] = std::toupper(stepName[0]); // fix for "step1" names
      chainToSteps[chain.name()].insert( stepName );
      chainToStepsId[chain.name()].insert( nstep );
      //check that the step name is set with the same position in the execution
      if ("Step"+std::to_string(nstep) != stepName)
	ATH_MSG_INFO("Sequence "<<seqName<<" (step "<<stepName<<") used at step "<<nstep<<" in chain "<<chain.name());
      nstep++;
    }
  }

  auto collToString = [&]( int xbin, const LockedHandle<TH2>& hist, int startOfset=0, int endOffset=0){ 
    std::string v;
    const int stepsSize = hist->GetYaxis()->GetNbins()-3; //L1, AfterPS, Output
    for ( int ybin = 1; ybin <= hist->GetYaxis()->GetNbins()-endOffset; ++ybin ) {
      if ( ybin > startOfset ) {
        // skip steps where chain wasn't active
        // ybins are for all axis labes, steps are in bins from 3 to stepsSize + 2
        const std::string chainName = m_passHistogram->GetXaxis()->GetBinLabel(xbin);
        ybin < 3 || ybin > stepsSize + 2 || chainToStepsId[chainName].find(ybin - 2) != chainToStepsId[chainName].end() ?
	  v += fixedWidth( std::to_string( int(hist->GetBinContent( xbin, ybin ))) , 11 )
          : v += fixedWidth( "-", 11 );
      } else {
        v += fixedWidth( " ", 11 );
      }
    }
    return v;
  };
  
  std::string v;
  v += fixedWidth( "L1", 11 );
  v += fixedWidth( "AfterPS", 11 );
  for ( int bin = 1; bin <= m_passHistogram->GetYaxis()->GetNbins()-3; ++bin ) {
    v += fixedWidth( "Step" + std::to_string(bin), 11 );
  }
  v += fixedWidth( "Output", 11 );
  
  ATH_MSG_INFO( "Chains passing step (1st row events & 2nd row decision counts):" );  
  ATH_MSG_INFO( fixedWidth("ChainName", 30) << v );

  /*
    comment for future dev:
    for combined chains we find x2 the number of decisions, because we count both the HypoAlg and the combo Alg decisions
  */
  
  for ( int bin = 1; bin <= (*m_passHistogram)->GetXaxis()->GetNbins(); ++bin ) {
    const std::string chainName = m_passHistogram->GetXaxis()->GetBinLabel(bin);
    const std::string chainID = std::to_string( HLT::Identifier(chainName) );
    if ( chainName.find("HLT") == 0 ) { // print only for chains
      ATH_MSG_INFO( chainName + " #" + chainID);
      ATH_MSG_INFO( fixedWidth("-- #" + chainID + " Events", 30)  << collToString( bin, m_passHistogram) );
      ATH_MSG_INFO( fixedWidth("-- #" + chainID + " Features", 30) << collToString( bin, m_countHistogram , 2, 1 ) );
    }
    if ( chainName.find("All") == 0 ){
      ATH_MSG_INFO( fixedWidth(chainName, 30)  << collToString( bin, m_passHistogram) );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillHistogram(const TrigCompositeUtils::DecisionIDContainer& dc, int row, LockedHandle<TH2>& histogram) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() && HLT::Identifier(id).name().find("leg") != 0 ) {
      ATH_MSG_WARNING( "HLT chain " << HLT::Identifier(id) << " not configured to be monitored" );
    } else {
      histogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillRate(const TrigCompositeUtils::DecisionIDContainer& dc, int row) const {
  return fillHistogram(dc, row, m_rateHistogram.getBuffer());
}

StatusCode TrigSignatureMoniMT::fillPassEvents(const TrigCompositeUtils::DecisionIDContainer& dc, int row) const {
  return fillHistogram(dc, row, m_passHistogram);
}

StatusCode TrigSignatureMoniMT::fillDecisionCount(const std::vector<TrigCompositeUtils::DecisionID>& dc, int row) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() ) {
    } else {
      m_countHistogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillBunchGroups(const TrigCompositeUtils::DecisionIDContainer& dc ) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    auto bunchGroups = m_chainIDToBunchMap.find( id );
    if ( id2bin != m_chainIDToBinMap.end() && bunchGroups != m_chainIDToBunchMap.end() ) {
      for (auto group : bunchGroups->second){
        m_bunchHistogram->Fill( id2bin->second, double(m_nameToBinMap.at(group)) );
        m_bunchHistogram->Fill( 1, double(m_nameToBinMap.at(group)) );
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillBCID(const TrigCompositeUtils::DecisionIDContainer& dc , int bcid) const {
  if ( nBCIDbinsY() > 0 ){
    for ( auto id : dc )  {
      auto id2bin = m_BCIDchainIDToBinMap.find( id );
      if ( id2bin != m_BCIDchainIDToBinMap.end() ) {
        m_bcidHistogram.fill( bcid, id2bin->second );
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillSequences(const std::set<std::string>& sequences) const {
  for ( auto seq : sequences ) {
    m_sequenceHistogram.fill(m_sequenceToBinMap.at(seq), 1);
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillStreamsAndGroups(const std::map<std::string, TrigCompositeUtils::DecisionIDContainer>& map, const TrigCompositeUtils::DecisionIDContainer& dc) const {
  const double row = nBinsY();
  const double rateRow = nRateBinsY();
  for ( auto m : map ) {
    for ( auto id : dc ) {
      if ( m.second.find(id) != m.second.end() ){
        double bin = m_nameToBinMap.at(m.first);
        m_countHistogram->Fill( bin, row );
        m_rateHistogram.fill( bin, rateRow );
        m_passHistogram->Fill( bin, row );
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

void TrigSignatureMoniMT::handle( const Incident& incident ) {
  // Create and start timer after fork
  if (incident.type() == AthenaInterprocess::UpdateAfterFork::type()) {
    if (m_rateHistogram.getTimer() || m_sequenceHistogram.getTimer() || m_bcidHistogram.getTimer()) {
      ATH_MSG_WARNING("Timer is already running. UpdateAfterFork incident called more than once?");
      return;
    }

    //prevent from publishing empty histograms
    if ( nBinsX() > 0 ) {
      m_rateHistogram.startTimer(m_duration, m_intervals);
    }
    
    if ( nSequenceBinsX() > 0 ) {
      m_sequenceHistogram.startTimer(m_duration, m_intervals);
    }    

    if ( nBCIDbinsY() > 0 ){
      m_bcidHistogram.startTimer(m_duration, m_intervals);
    }
    
    ATH_MSG_DEBUG("Started rate timer");
  }
}

StatusCode TrigSignatureMoniMT::execute( const EventContext& context ) const {

  auto l1Decisions = SG::makeHandle( m_l1DecisionsKey, context );

  const TrigCompositeUtils::Decision* l1SeededChains = nullptr; // Activated by L1
  const TrigCompositeUtils::Decision* unprescaledChains = nullptr; // Activated and passed prescale check
  for (const TrigCompositeUtils::Decision* d : *l1Decisions) {
    if (d->name() == "l1seeded") {
      l1SeededChains = d;
    } else if (d->name() == "unprescaled") {
      unprescaledChains = d;
    }
  }

  if (l1SeededChains == nullptr || unprescaledChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the summary from the L1Decoder.");
    return StatusCode::FAILURE;
  }

  auto fillL1 = [&]( int index ) -> StatusCode {    
    TrigCompositeUtils::DecisionIDContainer ids;    
    TrigCompositeUtils::decisionIDs( l1Decisions->at( index ), ids );
    ATH_MSG_DEBUG( "L1 " << index << " N positive decisions " << ids.size()  );
    ATH_CHECK( fillPassEvents( ids, index + 1 ));
    ATH_CHECK( fillRate( ids, index + 1) );
    if ( not ids.empty() ){
      m_passHistogram->Fill( 1, double(index + 1) );
      m_rateHistogram.fill( 1, double(index + 1) );
    }
    return StatusCode::SUCCESS;
  };

  ATH_CHECK( fillL1(0) );
  ATH_CHECK( fillL1(1) );

  int step = 0;
  for ( auto& ctool: m_collectorTools ) {
    std::vector<TrigCompositeUtils::DecisionID> stepSum;
    std::set<std::string> stepSequences;
    ctool->getDecisions( stepSum );
    ctool->getSequencesPerEvent( stepSequences );
    ATH_MSG_DEBUG( " Step " << step << " decisions " << stepSum.size() );
    TrigCompositeUtils::DecisionIDContainer stepUniqueSum( stepSum.begin(), stepSum.end() );
    ATH_CHECK( fillPassEvents( stepUniqueSum, 3+step ) );
    ATH_CHECK( fillDecisionCount( stepSum, 3+step ) );
    ATH_CHECK( fillSequences( stepSequences ) );
    ++step;
  }

  const int row = nBinsY();
  const int rateRow = nRateBinsY();
  auto finalDecisionsHandle = SG::makeHandle( m_finalDecisionKey, context );
  ATH_CHECK( finalDecisionsHandle.isValid() );
  TrigCompositeUtils::DecisionIDContainer finalIDs;
  for (const TrigCompositeUtils::Decision* decisionObject : *finalDecisionsHandle) {
    if (decisionObject->name() == "HLTPassRaw") {
      TrigCompositeUtils::decisionIDs(decisionObject, finalIDs);
      break;
    }
  }
  
  ATH_CHECK( fillStreamsAndGroups( m_streamToChainMap, finalIDs ) );
  ATH_CHECK( fillStreamsAndGroups( m_groupToChainMap, finalIDs ) );
  ATH_CHECK( fillPassEvents( finalIDs, row ) );
  ATH_CHECK( fillRate( finalIDs, rateRow ) );
  ATH_CHECK( fillBunchGroups( finalIDs ) );
  ATH_CHECK( fillBCID( finalIDs , context.eventID().bunch_crossing_id()) );

  if ( not finalIDs.empty() ) {
    m_passHistogram->Fill( 1, double( row ) );
    m_rateHistogram.fill( 1, double( rateRow ) );
  }

  return StatusCode::SUCCESS;
}

int TrigSignatureMoniMT::nBinsX(SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle) const {
  return hltMenuHandle->size() + m_groupToChainMap.size() + m_streamToChainMap.size() + 1;
}

int TrigSignatureMoniMT::nBinsX() const {
  return m_chainIDToBinMap.size() + m_groupToChainMap.size() + m_streamToChainMap.size() + 1;
}

int TrigSignatureMoniMT::nBunchBinsX(SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle) const {
  return hltMenuHandle->size() + 1; //only chains
}

int TrigSignatureMoniMT::nBCIDbinsX() const {
  return m_bcidNumber + 1; //only chains
}

int TrigSignatureMoniMT::nSequenceBinsX() const {
  return m_sequenceToBinMap.size();
}

int TrigSignatureMoniMT::nBinsY() const {
  return m_collectorTools.size() + 3; // in, after ps, out
}

int TrigSignatureMoniMT::nRateBinsY() const {
  return 3; // in, after ps, out
}

int TrigSignatureMoniMT::nBCIDbinsY() const {
  return m_BCIDchainIDToBinMap.size();
}

int TrigSignatureMoniMT::nBunchBinsY(SG::ReadHandle<TrigConf::L1Menu>& l1MenuHandle) const {
  return l1MenuHandle->getObject("bunchGroups").getKeys().size() - 1;
}

StatusCode TrigSignatureMoniMT::initHist(LockedHandle<TH2>& hist, SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle, bool steps) {
  TAxis* x = hist->GetXaxis();
  x->SetBinLabel(1, "All");
  int bin = 2; // 1 is for total count, (remember bins numbering in ROOT start from 1)

  std::vector<std::string> sortedChainsList;
  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    sortedChainsList.push_back( chain.name() );
  }
  std::sort( sortedChainsList.begin(), sortedChainsList.end() );
  
  for ( const std::string& chainName: sortedChainsList ) {
    x->SetBinLabel( bin, chainName.c_str() );
    m_chainIDToBinMap[ HLT::Identifier( chainName ).numeric() ] = bin;
    bin++;
  }

  for ( const auto& stream : m_streamToChainMap){
    x->SetBinLabel( bin, ("str_"+stream.first).c_str());
    m_nameToBinMap[ stream.first ] = bin;
    bin++;
  }

  for ( const auto& group : m_groupToChainMap){
    x->SetBinLabel( bin, ("grp_"+group.first.substr(group.first.find(':')+1)).c_str() );
    m_nameToBinMap[ group.first ] = bin;
    bin++;
  }


  TAxis* y = hist->GetYaxis();
  y->SetBinLabel( 1, steps ? "L1" : "Input" );
  y->SetBinLabel( 2, "AfterPS" );
  for ( size_t i = 0; steps && i < m_collectorTools.size(); ++i ) {
    y->SetBinLabel( 3+i, ("Step "+std::to_string(i)).c_str() );
  }
  y->SetBinLabel( y->GetNbins(), "Output" ); // last bin

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::initSeqHist(LockedHandle<TH2>& hist, std::set<std::string>& sequenceSet) {
  TAxis* x = hist->GetXaxis();
  int bin = 1;

  for ( auto seqName : sequenceSet ) {
    x->SetBinLabel( bin, seqName.c_str() );
    m_sequenceToBinMap[ seqName ] = bin;
    bin++;
  }

  TAxis* y = hist->GetYaxis();
  y->SetBinLabel( 1, "Rate" );

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::initBCIDhist(LockedHandle<TH2>& hist, const std::vector<std::string>& chainNames){
  std::vector<std::string> sortedChainNames(chainNames);
  std::sort( sortedChainNames.begin(), sortedChainNames.end() );

  TAxis* y = hist->GetYaxis();
  int bin = 1;

  for ( const std::string& chainName : sortedChainNames ){
    y->SetBinLabel( bin, chainName.c_str() );
    m_BCIDchainIDToBinMap[HLT::Identifier(chainName).numeric()] = bin;
    ++bin;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::initBunchHist(LockedHandle<TH2>& hist, SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle, SG::ReadHandle<TrigConf::L1Menu>& l1MenuHandle) {

  bool gotL1Menu = l1MenuHandle.isValid() && l1MenuHandle->isInitialized();

  TAxis* x = hist->GetXaxis();
  x->SetBinLabel(1, "All");
  int bin = 2;

  std::vector<std::string> sortedChainsList;
  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    sortedChainsList.push_back( chain.name() );
  }
  std::sort( sortedChainsList.begin(), sortedChainsList.end() );

  for ( const std::string& chainName: sortedChainsList ) {
    x->SetBinLabel( bin, chainName.c_str() );
    ++bin;
  }

  std::vector<std::string> sortedBunchGroups;
  if( gotL1Menu ) {
    sortedBunchGroups = l1MenuHandle->getObject("bunchGroups").getKeys();
    std::sort( sortedBunchGroups.begin(), sortedBunchGroups.end() );
    sortedBunchGroups.erase(std::remove(sortedBunchGroups.begin(), sortedBunchGroups.end(), "BGRP0"), sortedBunchGroups.end());
  } else {
    for(size_t i = 1; i<=16; ++i) {
      sortedBunchGroups.emplace_back("BGRP" + std::to_string(i));
    }
  }

  bin = 1;
  TAxis* y = hist->GetYaxis();
  for ( const std::string& group : sortedBunchGroups ){
    std::string bgname = gotL1Menu ? l1MenuHandle->getAttribute( "bunchGroups." + group + ".name", true) : group;
    y->SetBinLabel( bin, bgname.c_str() );
    m_nameToBinMap[group] = bin;
    ++bin;
  }

  return StatusCode::SUCCESS;
}

TrigSignatureMoniMT::RateHistogram::~RateHistogram(){
  delete m_bufferHistogram.get();
}

StatusCode TrigSignatureMoniMT::RateHistogram::init( const std::string histoName, const std::string histoTitle,
  const int x, const int y, const std::string registerPath, ServiceHandle<ITHistSvc> histSvc ){
  std::unique_ptr<TH2> h = std::make_unique<TH2F>(histoName.c_str(), histoTitle.c_str(), x, 1, x + 1, y, 1, y + 1);
  ATH_CHECK( histSvc->regShared( registerPath.c_str(), std::move(h), m_histogram));
  
  TH2I * hB = new TH2I( (histoName + "Buffer").c_str(), histoTitle.c_str(), x, 1, x + 1, y, 1, y + 1);
  m_bufferHistogram.set(hB, &m_mutex);
  m_bufferHistogram->SetDirectory(0);

  return StatusCode::SUCCESS;
}

LockedHandle<TH2> & TrigSignatureMoniMT::RateHistogram::getHistogram() const {
  return m_histogram;
}

LockedHandle<TH2> & TrigSignatureMoniMT::RateHistogram::getBuffer() const {
  return m_bufferHistogram;
}

std::unique_ptr<Athena::AlgorithmTimer> & TrigSignatureMoniMT::RateHistogram::getTimer() {
  return m_timer;
}

void TrigSignatureMoniMT::RateHistogram::fill(const double x, const double y) const {
  m_bufferHistogram->Fill( x, y );
}

void TrigSignatureMoniMT::RateHistogram::startTimer(unsigned int duration, unsigned int intervals) {
  m_duration = duration;
  m_timeDivider = std::make_unique<TimeDivider>(intervals, duration, TimeDivider::seconds);
  m_timer = std::make_unique<Athena::AlgorithmTimer>(duration*50, boost::bind(&RateHistogram::callback, this), Athena::AlgorithmTimer::DELIVERYBYTHREAD);
}

void TrigSignatureMoniMT::RateHistogram::stopTimer() {
  if (m_timer) {
    m_timer.reset();
    time_t t = time(0);
    unsigned int interval;
    unsigned int duration = m_timeDivider->forcePassed(t, interval);
    updatePublished(duration); //divide by time that really passed not by interval duration
  }

}

void TrigSignatureMoniMT::RateHistogram::updatePublished(unsigned int duration) const {
  m_histogram->Reset("ICES");
  m_histogram->Add(m_bufferHistogram.get(), 1./duration);
  m_bufferHistogram->Reset("ICES");
}


void TrigSignatureMoniMT::RateHistogram::callback() const {
  //ask time divider if we need to switch to new interval
  time_t t = time(0);
  unsigned int newinterval;
  unsigned int oldinterval;

  if ( m_timeDivider->isPassed(t, newinterval, oldinterval) ) {
    updatePublished(m_duration);
  }

  //schedule itself in another 1/20 of the integration period in milliseconds
  if ( m_timer ) m_timer->start(m_duration*50);
}
