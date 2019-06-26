/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "GaudiKernel/Property.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSignatureMoniMT.h"



TrigSignatureMoniMT::TrigSignatureMoniMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) 
{}

StatusCode TrigSignatureMoniMT::initialize() {

  ATH_CHECK( m_l1DecisionsKey.initialize() );
  ATH_CHECK( m_finalDecisionKey.initialize() );
  ATH_CHECK( m_collectorTools.retrieve() );
  ATH_CHECK( m_histSvc.retrieve() );
      
  
  {
    const int x = nBinsX();
    const int y = nBinsY();
    ATH_MSG_DEBUG( "Histogram " << x << " x " << y << " bins");
    // this type to be replaced by LBN tagged hist available for MT
    // the new way, (does not work)
    /*
    auto hist = std::make_unique<TH2I>( "SignatureAcceptance", "Raw acceptance of signatures in;chain;step",
					x, -0.5, x -0.5,
					y, -0.5, y - 0.5 );

    std::string fullName = m_bookingPath + "/SignatureAcceptance";

    m_histSvc->regHist( fullName, std::move( hist ) );
    m_outputHistogram = m_histSvc.getHist( fullName );
    */

    std::unique_ptr<TH2> h1 = std::make_unique<TH2I>("SignatureAcceptance", "Raw acceptance of signatures in;chain;step", x, 1, x + 1, y, 1, y + 1);
    std::unique_ptr<TH2> h2 = std::make_unique<TH2I>("DecisionCount", "Positive decisions count per step;chain;step", x, 1, x + 1, y, 1, y + 1);
    std::unique_ptr<TH2> h3 = std::make_unique<TH2I>("RateCount", "Rate of positive decisions", x, 1, x + 1, y, 1, y + 1);

    ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/SignatureAcceptance", std::move(h1), m_passHistogram));
    ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/DecisionCount", std::move(h2), m_countHistogram));
    ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/RateCount", std::move(h3), m_rateHistogram));
  
  }
  
  ATH_CHECK( initHist( m_passHistogram ) );
  ATH_CHECK( initHist( m_countHistogram ) );
  ATH_CHECK( initHist( m_rateHistogram ) );

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::finalize() {

  if (m_chainIDToBinMap.empty()) {
    ATH_MSG_INFO( "No chains configured, no counts to print" );
    return StatusCode::SUCCESS;
  }
  
  auto collToString = [&]( int xbin, const LockedHandle<TH2>& hist, int startOfset=0, int endOffset=0){ 
    std::string v;
    for ( int ybin = 1; ybin <= hist->GetYaxis()->GetNbins()-endOffset; ++ybin ) {
      if ( ybin > startOfset ) {
	v += std::to_string( int(hist->GetBinContent( xbin, ybin )) );
	v += std::string( 10*ybin - v.size(),  ' ' ); // fill with spaces
      } else {
	v += std::string( 10, ' ');
      }
    }
    
    return v;
  };
  
  
  auto fixedWidth = [](const std::string& s, size_t sz) {
    return s.size() < sz ?
    s+ std::string(sz - s.size(), ' ') : s; };


  std::string v;
  for ( int bin = 1; bin <= m_passHistogram->GetYaxis()->GetNbins()-3; ++bin ) {
    v += "step";
    v += std::to_string(bin);
    v += std::string( 10*bin - v.size(),  ' ' );
  }
  
  ATH_MSG_INFO( "Chains passing step (1st row events & 2nd row decision counts):" );  
  ATH_MSG_INFO( "Chain name                   L1,      AfterPS,  "<<v<<"Output"  );


  /*
    comment for future dev:
    for combined chains we find x2 the number of decisions, because we count both the HypoAlg and the combo Alg decisions
  */
  
  for ( int bin = 1; bin <= (*m_passHistogram)->GetXaxis()->GetNbins(); ++bin ) {
    const std::string chainName = m_passHistogram->GetXaxis()->GetBinLabel(bin);
    ATH_MSG_INFO( fixedWidth(chainName, 30)  << collToString( bin, m_passHistogram) );
    if ( chainName.find("HLT") == 0 ) { // print only for chains
      ATH_MSG_INFO( fixedWidth(chainName +" decisions", 30) << collToString( bin, m_countHistogram , 2, 1 ) );
    }
  }		 

  
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillPassEvents(const TrigCompositeUtils::DecisionIDContainer& dc, int row, LockedHandle<TH2>& histogram) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() ) {
      ATH_MSG_WARNING( "HLT chain " << HLT::Identifier(id) << " not configured to be monitored" );
    } else {
      histogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
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

StatusCode TrigSignatureMoniMT::fillRate(const TrigCompositeUtils::DecisionIDContainer& dc, int row) const {
  return fillPassEvents(dc, row, m_rateHistogram); 
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
    ATH_CHECK( fillPassEvents( ids, index + 1, m_passHistogram ));
    ATH_CHECK( fillRate( ids, index + 1) );
    if ( not ids.empty() ){
      m_passHistogram->Fill( 1, double(index + 1) );
      m_rateHistogram->Fill( 1, double(index + 1) );
    }
    return StatusCode::SUCCESS;
  };

  ATH_CHECK( fillL1(0) );
  ATH_CHECK( fillL1(1) );

  int step = 0;
  for ( auto& ctool: m_collectorTools ) {
    std::vector<TrigCompositeUtils::DecisionID> stepSum;
    ctool->getDecisions( stepSum );
    ATH_MSG_DEBUG( " Step " << step << " decisions " << stepSum.size() );
    TrigCompositeUtils::DecisionIDContainer stepUniqueSum( stepSum.begin(), stepSum.end() );
    ATH_CHECK( fillPassEvents( stepUniqueSum, 3+step, m_passHistogram ) );
    ATH_CHECK( fillDecisionCount( stepSum, 3+step ) );
    ATH_CHECK( fillRate( stepUniqueSum, 3+step ) );    

    ++step;
  }

  const int row = m_passHistogram->GetYaxis()->GetNbins();
  auto finalDecisionsHandle = SG::makeHandle( m_finalDecisionKey, context );
  ATH_CHECK( finalDecisionsHandle.isValid() );
  TrigCompositeUtils::DecisionIDContainer finalIDs;
  for (const TrigCompositeUtils::Decision* decisionObject : *finalDecisionsHandle) {
    if (decisionObject->name() == "HLTPassRaw") {
      TrigCompositeUtils::decisionIDs(decisionObject, finalIDs);
      break;
    }
  }
  ATH_CHECK( fillPassEvents( finalIDs, row, m_passHistogram ) );
  ATH_CHECK( fillRate( finalIDs, row ) );

  if ( not finalIDs.empty() ) {
    m_passHistogram->Fill( 1, double( row ) );
    m_rateHistogram->Fill( 1, double( row ) );
  }
  
  return StatusCode::SUCCESS;
}

int TrigSignatureMoniMT::nBinsY() const {     
  return m_collectorTools.size()+3; // in, after ps, out
}

StatusCode TrigSignatureMoniMT::initHist(LockedHandle<TH2>& hist) {

  TAxis* x = hist->GetXaxis();
  x->SetBinLabel(1, "All");
  int bin = 2; // 1 is for total count, (remember bins numbering in ROOT start from 1)

  std::vector<std::string> sortedChainsList( m_allChains );
  std::sort( sortedChainsList.begin(), sortedChainsList.end() );
  
  for ( auto chainName:  sortedChainsList ) {
    x->SetBinLabel( bin, chainName.c_str() );
    m_chainIDToBinMap[ HLT::Identifier( chainName ).numeric() ] = bin;
    bin++;
  }

  TAxis* y = hist->GetYaxis();
  y->SetBinLabel( 1, "L1" );
  y->SetBinLabel( 2, "AfterPS" );
  for ( size_t i = 0; i < m_collectorTools.size(); ++i ) {
    y->SetBinLabel( 3+i, ("Step "+std::to_string(i)).c_str() );
  }
  y->SetBinLabel( y->GetNbins(), "Output" ); // last bin

  // loop until the last bin and assign labels Step #
  for ( int bin = 3; std::string( y->GetBinLabel( bin ) ) != "Output" ; ++bin ) {
    y->SetBinLabel(bin, ( "Step " + std::to_string( bin - 2 ) ).c_str() );
  }
 
  return StatusCode::SUCCESS;
}
