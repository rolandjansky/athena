/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "GaudiKernel/Property.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSignatureMoniMT.h"



TrigSignatureMoniMT::TrigSignatureMoniMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ) 
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

    m_passHistogram = new TH2I( "SignatureAcceptance", "Raw acceptance of signatures in;chain;step",
				  x, 1, x + 1,
				  y, 1, y + 1 ); // Fill and GetBinContent use the same indexing then

    m_countHistogram = (TH2*)m_passHistogram->Clone("DecisionCount");
    m_countHistogram->SetTitle("Positive decisions count per step;chain;step");
    

    ATH_CHECK( m_histSvc->regHist( m_bookingPath + "/"+m_passHistogram->GetName(), m_passHistogram ) );
    ATH_CHECK( m_histSvc->regHist( m_bookingPath + "/"+m_countHistogram->GetName(), m_countHistogram ) );


    

  }
  ATH_CHECK( initHist( m_passHistogram ) );
  ATH_CHECK( initHist( m_countHistogram ) );
  

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::finalize() {
  
  auto collToString = [&]( int xbin, const TH2* hist, int startOfset=0, int endOffset=0){ 
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
  
  ATH_MSG_INFO( "Chains passing step (1st row events & 2nd row decision counts" );  
  ATH_MSG_INFO( "Chain name                   L1,      AfterPS, [... steps ...], Output"  );


  auto fixedWidth = [](const std::string& s, size_t sz) {
		      return s.size() < sz ?
					s+ std::string(sz - s.size(), ' ') : s; };
  
  for ( int bin = 1; bin <= m_passHistogram->GetXaxis()->GetNbins(); ++bin ) {
    const std::string chainName = m_passHistogram->GetXaxis()->GetBinLabel(bin);
    ATH_MSG_INFO( fixedWidth(chainName, 30)  << collToString( bin, m_passHistogram ) );
    if ( chainName.find("HLT") == 0 ) { // print only for chains
      ATH_MSG_INFO( fixedWidth(chainName +" decisions", 30) << collToString( bin, m_countHistogram, 2, 1 ) );
    }
  }		 
    
  
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillPass(const TrigCompositeUtils::DecisionIDContainer& dc, int row) {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() ) {
      ATH_MSG_WARNING( "HLT chain " << HLT::Identifier(id) << " not configured to be monitored" );
    } else {
      m_passHistogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillCount(const std::vector<TrigCompositeUtils::DecisionID>& dc, int row) {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() ) {
    } else {
      m_countHistogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigSignatureMoniMT::execute()  {  

  auto l1Decisions = SG::makeHandle( m_l1DecisionsKey );

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
    ATH_CHECK( fillPass( ids, index + 1 ) );
    if ( not ids.empty() )
      m_passHistogram->Fill( 1, double(index + 1) );
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
    ATH_CHECK( fillPass( stepUniqueSum, 3+step ) );
    ATH_CHECK( fillCount( stepSum, 3+step ) );
    
    ++step;
  }
   
  const int row = m_passHistogram->GetYaxis()->GetNbins();
  auto finalDecisionsHandle = SG::makeHandle( m_finalDecisionKey );
  ATH_CHECK( finalDecisionsHandle.isValid() );
  TrigCompositeUtils::DecisionIDContainer finalIDs;
  for (const TrigCompositeUtils::Decision* decisionObject : *finalDecisionsHandle) {
    if (decisionObject->name() == "HLTPassRaw") {
      TrigCompositeUtils::decisionIDs(decisionObject, finalIDs);
    }
  }
  ATH_CHECK( fillPass( finalIDs, row ) );
  
  if ( not finalIDs.empty() ) {
    m_passHistogram->Fill( 1, double( row ) );
  }
    
  return StatusCode::SUCCESS;
}




int TrigSignatureMoniMT::nBinsY() const {     
  return m_collectorTools.size()+3; // in, after ps, out
}

StatusCode TrigSignatureMoniMT::initHist(TH2* hist) {

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
