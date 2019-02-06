/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthContainers/AuxElement.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/WriteHandle.h"
#include "DecisionHandling/TrigCompositeUtils.h"



static const SG::AuxElement::Accessor< std::vector< int > >   readWriteAccessor( "decisions" );
static const SG::AuxElement::ConstAccessor< std::vector<int> > readOnlyAccessor( "decisions" );

namespace TrigCompositeUtils {  

  SG::WriteHandle<DecisionContainer> createAndStore( const SG::WriteHandleKey<DecisionContainer>& key, const EventContext& ctx ) {
    SG::WriteHandle<DecisionContainer> handle( key, ctx );
    auto data = std::make_unique<DecisionContainer>() ;
    auto aux = std::make_unique<DecisionAuxContainer>() ;
    data->setStore( aux.get() );
    handle.record( std::move( data ), std::move( aux )  ).ignore();
    return handle;
  }

  void createAndStore( SG::WriteHandle<DecisionContainer>& handle ) {
    auto data = std::make_unique<DecisionContainer>() ;
    auto aux = std::make_unique<DecisionAuxContainer>() ;
    data->setStore( aux.get() );
    handle.record( std::move( data ), std::move( aux )  ).ignore();
  }

  Decision* newDecisionIn ( DecisionContainer* dc, const std::string& name, const EventContext& ctx ) {
    Decision * x = new Decision;
    dc->push_back( x );
    readWriteAccessor( *x ).size(); // fake operation just to make the decisions decoration
    size_t index = dc->size() - 1;
    // make self link, useful to copy for seed link in a successor, but requires that DecisionContainer is already recorded in SG.
    ElementLink<DecisionContainer> el(*dc, index, ctx);
    if ( ! x->setObjectLink( "self", el ) ){ 
      std::cerr << "TrigCompositeUtils::newDecisionIn ERROR failed to set self EL with key, maybe the DecisionContainer is not yet recorded in the event store? " << el.key() << std::endl;
      throw GaudiException(" failed to set self EL ", "TrigCompositeUtils::newDecisionIn", StatusCode::FAILURE);
    }
    if ( ! name.empty() ) {
      x->setName( name );
    }
    return x;
  }

  Decision* newDecisionIn ( DecisionContainer* dc, const Decision* dOld, const std::string& name, const EventContext& ctx ) {
    Decision* dNew =  newDecisionIn( dc, name, ctx);
   if ( dOld->hasObjectLink("roi" ) ){
     dNew->copyLinkFrom(dOld,"roi");
   }
   else if ( dOld->hasObjectLink("initialRoI") ){
     dNew->copyLinkFrom(dOld,"initialRoI","roi");
   }
   if ( dOld->hasObjectLink("view" ) ){
     dNew->copyLinkFrom(dOld,"view");
   }
   if ( dOld->hasObjectLink("feature" ) ){
     dNew->copyLinkFrom(dOld,"feature");
   }
   if ( dOld->hasObjectLink("self" ) ){
     dNew->copyLinkFrom(dOld,"self","seed"); // make use of self-link 
   }
   return dNew;
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

  const std::vector<int>& decisionIDs( const Decision* d ) {    
    return readOnlyAccessor( *d );    
  }

  std::vector<int>& decisionIDs( Decision* d ) {
    return readWriteAccessor( *d );
  }

  void insertDecisionIDs( const Decision* src, Decision* dest ){
    DecisionIDContainer ids;
    decisionIDs( src, ids );
    decisionIDs( dest, ids );
    decisionIDs( dest ).clear(); 
    decisionIDs( dest ).insert( decisionIDs(dest).end(), ids.begin(), ids.end() );
  }

  void uniqueDecisionIDs( Decision* dest){
    DecisionIDContainer ids;
    decisionIDs( dest, ids );
    decisionIDs( dest ).clear(); 
    decisionIDs( dest ).insert( decisionIDs(dest).end(), ids.begin(), ids.end() );
  }

  bool allFailed( const Decision* d ) {
    const std::vector<int>& decisions = readOnlyAccessor( *d );    
    return decisions.empty();
  }

  bool isAnyIDPassing( const Decision* d,  const DecisionIDContainer& required ) {
    for ( auto id : readOnlyAccessor( *d ) ) {
      if ( required.count( id ) > 0 )
	return true;
    }
    return false;
  }    

  bool passed( DecisionID id, const DecisionIDContainer& idSet ) {
    return idSet.count( id ) != 0;
  }

  void linkToPrevious( Decision* d, const std::string& previousCollectionKey, size_t previousIndex ) {
    ElementLinkVector<DecisionContainer> seeds;
    ElementLink<DecisionContainer> new_seed= ElementLink<DecisionContainer>( previousCollectionKey, previousIndex );
    // do we need this link to self link?
    if ( (*new_seed)->hasObjectLink("self" ) )
      seeds.push_back( (*new_seed)->objectLink<DecisionContainer>("self")); // make use of self-link 
    else
      seeds.push_back(ElementLink<DecisionContainer>( previousCollectionKey, previousIndex ));
    
    if (hasLinkToPrevious(d) ){
      ElementLinkVector<DecisionContainer> oldseeds = d->objectCollectionLinks<DecisionContainer>( "seed" );
      seeds.reserve( seeds.size() + oldseeds.size() );
      std::move( oldseeds.begin(), oldseeds.end(), std::back_inserter( seeds ) );
    }

    d->addObjectCollectionLinks("seed", seeds);
    
  }

  bool hasLinkToPrevious( const Decision* d ) {
    return d->hasObjectLink( "seed" );
  }

  ElementLinkVector <DecisionContainer> getLinkToPrevious( const Decision* d ) {
    return d->objectCollectionLinks<DecisionContainer>( "seed" );
  }


  bool copyLinks(const Decision* src, Decision* dest) {
    return dest->copyAllLinksFrom(src);
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

  std::string dump( const xAOD::TrigComposite*  tc, std::function< std::string( const xAOD::TrigComposite* )> printerFnc ) {
    std::string ret; 
    ret += printerFnc( tc );
    if ( tc->hasObjectLink("seed") ) {
      const xAOD::TrigComposite* seedTc = tc->object<xAOD::TrigComposite>( "seed" );
      if ( seedTc ) {
	ret += " -> " + dump( seedTc, printerFnc );
      }
    }
    return ret;
  }
  
}

