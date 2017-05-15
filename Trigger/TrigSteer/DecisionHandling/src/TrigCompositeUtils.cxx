/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthContainers/AuxElement.h"
#include "StoreGate/WriteHandle.h"
#include "DecisionHandling/TrigCompositeUtils.h"



static SG::AuxElement::Accessor< std::vector< int > >   readWriteAccessor( "decisions" );
static SG::AuxElement::ConstAccessor< std::vector<int> > readOnlyAccessor( "decisions" );

namespace TrigCompositeUtils {  

  DecisionWriteHandle createAndStore(const SG::WriteHandleKey<DecisionContainer>& key, const EventContext& ctx) {
    SG::WriteHandle<DecisionContainer> handle(key, ctx);
    auto data = std::make_unique<DecisionContainer>() ;
    auto aux = std::make_unique<DecisionAuxContainer>() ;
    data->setStore(aux.get());
    handle.record( std::move( data ), std::move( aux )  ).ignore();
    return handle;
  }
  
  Decision* newDecisionIn (DecisionContainer* dc) {
    Decision * x = new Decision;
    dc->push_back( x );
    readWriteAccessor( *x ).size(); // fake operation just to make the decsions decoration
    return x;
  }

  void addDecisionID( DecisionID id,  Decision* d) {   
    readWriteAccessor( *d ).push_back( id );
  }
  
  void decisionIDs( const Decision* d, DecisionIDContainer& destination ) {    
    const std::vector<int>& decisions = readOnlyAccessor( *d );    
    destination.insert( decisions.begin(), decisions.end() );
  }

  bool passingIDs( const Decision* d,  const DecisionIDContainer& required ) {
    for ( auto id : readOnlyAccessor( *d ) ) {
      if ( required.count( id ) > 0 )
	return true;
    }
    return false;
  }    

  bool passingIDs( const Decision* d,  const DecisionIDContainer& required);

  void linkToPrevious(Decision* d, const std::string& previousCollectionKey, size_t previousIndex) {
    d->setObjectLink( "seed", ElementLink<DecisionContainer>(previousCollectionKey, previousIndex) );
  }


  bool hasLinkToPrevious(const Decision* d) {
    return d->hasObjectLink("seed");
  }

  ElementLink<DecisionContainer> linkToPrevious(const Decision* d) {
    return d->objectLink<DecisionContainer>("seed");
  }
  


}
