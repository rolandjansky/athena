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
  Decision* newDecisionIn (DecisionContainer* dc, const std::string& name) {
    Decision* d = newDecisionIn( dc );
    d->setName( name );
    return d;
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
    ElementLinkVector<DecisionContainer> seeds = d->objectCollectionLinks<DecisionContainer>( "seed" );
    seeds.push_back(ElementLink<DecisionContainer>( previousCollectionKey, previousIndex ));
    d->addObjectCollectionLinks("seed", seeds);
    
    //copyLinksFromPrevious()
    //    d->setObjectLink( "seed", ElementLink<DecisionContainer>( previousCollectionKey, previousIndex ) );
  }


  bool hasLinkToPrevious( const Decision* d ) {
    return d->hasObjectLink( "seed" );
  }

  ElementLinkVector <DecisionContainer> getLinkToPrevious( const Decision* d ) {
    return d->objectCollectionLinks<DecisionContainer>( "seed" );
    //    return d->objectLink<DecisionContainer>( "seed" );
  }


  bool copyLinks(const Decision* src, Decision* dest) {
    return dest->copyAllLinksFrom(src);
  }

  // bool copyLinksFromPrevious(const Decision* src, Decision* dest){
  //   bool didCopy = false;

  //   for (const std::string& name : src->linkColNames()) {
  //     const std::string new_name = name.insert(0,"_");
  //     // Cannot copy any 'self' links as *this does not know its own location in its parent container
  //     if (name == "self") continue;
  //     // Check we don't have one (or more) entries with this raw name (raw = might be mangled).
  //     if (dest->hasObjectLink(new_name)) continue;
  //     // Check if the link is for a single object or collection of objects by looking for the mangled suffix
  //     const bool isCollection = (name.size() > s_collectionSuffix.size() && 
  // 				 std::equal(s_collectionSuffix.rbegin(), s_collectionSuffix.rend(), name.rbegin()));
  //     if (isCollection) {
  // 	// The copyLinkCollectionFrom call needs the un-mangled name as it is a public fn. It will re-mangle.
  // 	const std::string unmangledName = name.substr(0, name.size() - s_collectionSuffix.size());
  // 	const std::string new_unmangledName = unmangledName.insert(0,"_");
  // 	copyLinkCollectionFrom(other, unmangledName, new_unmangledName);
  //     } else { // !isCollection
  // 	copyLinkFrom(other, name, new_name);
  //     }
  //     didCopy = true;
  //   }
  //   return didCopy;

  // }
    
  // // for (auto link: baseLinks){
  // //   if ( src->hasObjectLink(link ) ) dest->copyLinkFrom(src,link);
  // // }

  // }


  
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
