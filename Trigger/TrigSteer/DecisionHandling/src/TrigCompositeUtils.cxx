
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

  DecisionWriteHandle createAndStore( const SG::WriteHandleKey<DecisionContainer>& key, const EventContext& ctx ) {
    SG::WriteHandle<DecisionContainer> handle( key, ctx );
    auto data = std::make_unique<DecisionContainer>() ;
    auto aux = std::make_unique<DecisionAuxContainer>() ;
    data->setStore( aux.get() );
    handle.record( std::move( data ), std::move( aux )  ).ignore();
    return handle;
  }
  
  Decision* newDecisionIn ( DecisionContainer* dc ) {
    Decision * x = new Decision;
    dc->push_back( x );
    readWriteAccessor( *x ).size(); // fake operation just to make the decsions decoration
    return x;
  }

  void addDecisionID( DecisionID id,  Decision* d ) {   
    std::vector<int>& decisions = readWriteAccessor( *d );
    if ( decisions.size() == 0 or decisions.back() != static_cast<int>(id) ) 
      decisions.push_back( id );
  }
  
  void decisionIDs( const Decision* d, DecisionIDContainer& destination ) {    
    const std::vector<int>& decisions = readOnlyAccessor( *d );    
    destination.insert( decisions.begin(), decisions.end() );
  }

  bool allFailed( const Decision* d ) {
    const std::vector<int>& decisions = readOnlyAccessor( *d );    
    return decisions.empty();
  }

  bool passingIDs( const Decision* d,  const DecisionIDContainer& required ) {
    for ( auto id : readOnlyAccessor( *d ) ) {
      if ( required.count( id ) > 0 )
	return true;
    }
    return false;
  }    


  bool passingIDs( const Decision* d,  const DecisionIDContainer& required );

  void linkToPrevious( Decision* d, const std::string& previousCollectionKey, size_t previousIndex ) {
    d->setObjectLink( "seed", ElementLink<DecisionContainer>( previousCollectionKey, previousIndex ) );
  }


  bool hasLinkToPrevious( const Decision* d ) {
    return d->hasObjectLink( "seed" );
  }

  ElementLink<DecisionContainer> linkToPrevious( const Decision* d ) {
    return d->objectLink<DecisionContainer>( "seed" );
  }

  const xAOD::TrigComposite* find( const xAOD::TrigComposite* start, const std::function<bool( const xAOD::TrigComposite* )>& filter ) {
    if ( filter( start ) ) return start;

    if ( start->hasObjectLink( "seed" ) ) {
      return find( start->object<xAOD::TrigComposite>( "seed" ), filter );
    }
    else return nullptr;
  }

  bool HasObject::operator()( const xAOD::TrigComposite* composite ) const {
    return composite->hasObjectLink( m_name );
  }

  bool recursiveGetObjectLinks( const xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer>& linkVector ){
    //recursively find in the seeds
    if ( start->hasObjectLink( "seed" ) ) {
      return recursiveGetObjectLinks( start->object<xAOD::TrigComposite>( "seed" ), linkVector );
    }

    int isComposite=0;
    start->getDetail( "IsComposite",isComposite );
    if ( !isComposite ) {
      std::cout <<"TrigCompositeTraversal ViewAlgs::getObjectLinks    WARNING  Proxy "<< start->name()<< " is not composite!"<<std::endl;
      return false;
    }

    // ElementLink<xAOD::TrigComposite> test;
    // test = start->objectLink( "childProxies" );
    ElementLinkVector<xAOD::TrigCompositeContainer> links = start->objectCollectionLinks<xAOD::TrigCompositeContainer>( "childProxies" );

    linkVector.reserve( linkVector.size() + links.size() );
    std::move( links.begin(), links.end(), std::back_inserter( linkVector ) );

    return true;
  }
  
}
