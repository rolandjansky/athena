/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSignatureMoniMT.h"



TrigSignatureMoniMT::TrigSignatureMoniMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ) 
{}

StatusCode TrigSignatureMoniMT::initialize() {

  ATH_MSG_INFO ("Initializing " << name() << "...");
  CHECK( m_l1DecisionsKey.initialize() );
  CHECK( m_finalDecisionsKey.initialize() );
  renounceArray( m_finalDecisionsKey );

  CHECK( m_histSvc.retrieve() );

  
  {
    const int x = nBinsX();
    const int y = nBinsY();
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
    m_outputHistogram = new TH2I( "SignatureAcceptance", "Raw acceptance of signatures in;chain;step",
				  x, -0.5, x -0.5,
				  y, -0.5, y - 0.5 );
    m_histSvc->regHist( m_bookingPath + "/SignatureAcceptance", m_outputHistogram );
    
  }
  CHECK( initHist() );
  

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::finalize() {
  
  auto collToVec = [&]( int xbin){ 
    std::vector<int> v;
    for ( int ybin = 1; ybin <= m_outputHistogram->GetYaxis()->GetNbins(); ++ybin )
      v.push_back( m_outputHistogram->GetBinContent( xbin, ybin ) );
    return v;
  };

  ATH_MSG_DEBUG("Signatures count chain name L1, After PS [... steps ...] Output"  );
  for ( int bin = 1; bin <= m_outputHistogram->GetXaxis()->GetNbins(); ++bin ) {
    ATH_MSG_DEBUG( m_outputHistogram->GetXaxis()->GetBinLabel(bin) << " " << collToVec( bin ) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillChains(const TrigCompositeUtils::DecisionIDContainer& dc, int row) {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() ) {
      ATH_MSG_WARNING( "HLT chain " << HLT::Identifier(id) << " not configured to be monitred" );
    } else {
      ATH_MSG_DEBUG( "Passed " <<  id2bin->second << " " << row );
      m_outputHistogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigSignatureMoniMT::execute()  {  

  auto l1Decisions = SG::makeHandle( m_l1DecisionsKey );
  CHECK( l1Decisions->at( 0 )->name() == "l1seeded" ); 
  CHECK( l1Decisions->at( 1 )->name() == "unprescaled" ); // see L1Decoder implementation

  auto fillL1 = [&]( int index ) -> StatusCode {    
    TrigCompositeUtils::DecisionIDContainer ids;    
    TrigCompositeUtils::decisionIDs( l1Decisions->at( index ), ids );
    ATH_MSG_DEBUG( "L1 " << index << " N positive decisions " << ids.size()  );
    CHECK( fillChains( ids, index ) );
    if ( not ids.empty() )
      m_outputHistogram->Fill( 0., double( index+1 ) );
    return StatusCode::SUCCESS;
  };

  CHECK( fillL1(0) );
  CHECK( fillL1(1) );
  
 
  const int row = m_outputHistogram->GetYaxis()->GetNbins();
  bool anyPassed = false;
  for ( auto d: m_finalDecisionsKey ) {
    auto decisions = SG::makeHandle( d );
    if ( decisions.isValid() )  { // may be invalid and that is perfectly correct (early rejection
      ATH_MSG_DEBUG( "Decision for " << decisions->size() << " objects available in " << d.key() );
      // we need one entry per chain only and the "sum" is used for that
      TrigCompositeUtils::DecisionIDContainer sum;
      
      for ( auto decisionObj : *decisions.get() ) {
	TrigCompositeUtils::DecisionIDContainer ids;
	TrigCompositeUtils::passingIDs( decisionObj, ids );	  
	sum.insert( ids.begin(), ids.end() ); // merge with so far passing chains
      }
      CHECK( fillChains( sum, row ) );
      anyPassed = anyPassed or ( not sum.empty() );
    } else {
      ATH_MSG_DEBUG( "Final decision " << d.key() << " absent, possibly early rejected" );
    }      
  } 
  if ( anyPassed ) 
    m_outputHistogram->Fill( 0., double( row ) );
  // missing intermediate steps monitoring

    
  return StatusCode::SUCCESS;
}




int TrigSignatureMoniMT::nBinsY() const { 
  typedef StringToStringVectorMap::value_type v;
  if ( m_steps.empty() ) {
    ATH_MSG_INFO( "Step decisions are not specified, output histogram will only contain input, HLT PS, and output" );
    return 3; // in, PS, out
  }

  
  const int maxSteps = std::max_element( m_steps.begin(), m_steps.end(), 
					 [](const v& el1, const v& el2){ return el1.second.size() < el2.second.size(); }
					 ) ->second.size();
  if ( maxSteps == 0 )
    ATH_MSG_INFO( "Step of depth 0  specified, output histogram will only contain input, HLT PS, and output" );
  else
    ATH_MSG_DEBUG( "Longest step of " << maxSteps );
  
  
  return 3+maxSteps;
}

StatusCode TrigSignatureMoniMT::initHist() {

  TAxis* x = m_outputHistogram->GetXaxis();
  x->SetBinLabel(1, "All");
  int bin = 2;
  for ( auto chainName:  m_allChains ) {
    x->SetBinLabel( bin, chainName.c_str() );
    m_chainIDToBinMap[ HLT::Identifier( chainName ).numeric() ] = bin;
    bin++;
  }


  TAxis* y = m_outputHistogram->GetYaxis();
  y->SetBinLabel(1, "L1");
  y->SetBinLabel(2, "AfterPS");
  y->SetBinLabel( y->GetNbins(), "Output" ); // last bin

  // loop until the last bin and assign labels Step #
  for ( int bin = 3; std::string( y->GetBinLabel( bin ) ) != "Output" ; ++bin ) {
    y->SetBinLabel(bin, ( "Step " + std::to_string( bin - 2 ) ).c_str() );
  }
 
  return StatusCode::SUCCESS;
}
