/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TrigNavigation/RoICacheHelper.h"

/*****************************************************************************
 *
 * CACHING
 *
 *****************************************************************************/


//#define PCI(__msg) std::cout << "-- caching info: " <<  __msg << std::endl;
#define PCI(__msg) 




using namespace HLT;
/*
void xx_print( const TriggerElement::FeatureVec& fe, const std::string& prefix) {  
  TriggerElement::FeatureVec::const_iterator i;
  for ( i = fe.begin(); i != fe.end(); ++i ) {
    PCI (prefix << " " << *i );
    //    log << prefix << " " << *i << endreq;
  }
}
*/

enum RoICacheHelper::CachingMode RoICacheHelper::s_cachingMode=RoICacheHelper::HistoryBased;

bool RoICacheHelper::cache ( TriggerElement* te ) {
  std::vector<class CacheEntry>::iterator it;

  // look into cache entries for cache holding same RoI
  it = find(m_caches.begin(), m_caches.end(), te); // we have overloaded operator==

  // nothing found, we run on this RoI for the first time (while we shouldn't)
  if ( m_caches.end() == it ) { // we need to start up new cache but we can't do so here
    return false;
  } else {
    if ( it->holder() == 0 ) {
      return false;
    }
    // do caching
    // real caching happens here
    //std::vector< TriggerElement::FeatureAccessHelper >::const_iterator b = it->holder()->getFeatureAccessHelpers().begin();
    //    advance(b, it->begin());
    
    //    std::vector< TriggerElement::FeatureAccessHelper >::const_iterator e = it->holder()->getFeatureAccessHelpers().begin();
    //    advance(e, it->end());



    
    //    std::cerr << "caching now: " << it->begin() << " " << it->end() << std::endl;
    const TriggerElement::FeatureVec& fvec = it->holder()->getFeatureAccessHelpers();

    if ( fvec.size() < it->end()  || it->begin() > it->end()  ) {
      return false;
    }

    unsigned int begin = it->begin();
    unsigned int end = it->end();   
    TriggerElement::FeatureVec temp(it->holder()->getFeatureAccessHelpers().begin()+begin,
				    it->holder()->getFeatureAccessHelpers().begin()+end);

    for ( auto fea: temp){
      te->addFeature(fea);
    }
    
    //    te->getFeatureAccessHelpers().insert(te->getFeatureAccessHelpers().end(), b, e);
    //    te->featuresKeyPrint().insert(te->featuresKeyPrint().end(), b, e);
    //    std::cerr << "after caching size: " << te->getFeatureAccessHelpers().size() << std::endl;
  }

  return true;
}


bool RoICacheHelper::isInputAccepted ( const TriggerElement* te ) {
  std::vector<class CacheEntry>::iterator it;
  it = find(m_caches.begin(), m_caches.end(), te);
  if ( m_caches.end() != it ) {
    if (it->holder())
      return true;
    return false;
  }
  return false;
}

bool RoICacheHelper::needsExecution ( const TriggerElement* te ) {
  std::vector<class CacheEntry>::iterator it;
  PCI( "needsExecution ? looking for caches" );
  it = find(m_caches.begin(), m_caches.end(), te);
  if ( m_caches.end() == it ) {
    m_caches.push_back( CacheEntry(te) );
    PCI( "Execution needed" );
    return true;
  }
  PCI( "Found appropriate cache - can avoid execution" );
  return false;
}


bool RoICacheHelper::startCaching( const TriggerElement* te ) {
  //  we have to start caching, last entry (back()) in m_caches is the right place to do that
  if ( te != m_caches.back().holder() )
    return false;

  RoICacheHistory::instance().startHistory(te);

  m_caches.back().start( te->getFeatureAccessHelpers().size() );
  return true;
}

bool RoICacheHelper::stopCaching( const TriggerElement* te ) {
  // we have to save cache, last entry (back()) in m_caches is the started cache
  CacheEntry &cache = m_caches.back();

  if ( te != cache.holder() )
    return false;

  cache.add(RoICacheHistory::instance().getFeatureCalls());

  /*
  // print the history
  std::vector<RoICacheHistory::Question>::const_iterator histIt;
  for ( histIt = RoICacheHistory::instance().getQuestions().begin(); histIt != RoICacheHistory::instance().getQuestions().end(); ++histIt ) {
  std::cout << "Objects access HISTORY: label: " << histIt->label() 
  << " work: "  << histIt->getWorkTE() << std::endl;
  if (histIt->getAnswers().empty()) 
  std::cout << "Objects access HISTORY: got back nothing for clid: " << histIt->clid() << " and label " << histIt->label() << std::endl;
  TriggerElement::FeatureVec::const_iterator feaIt;
  for ( feaIt = histIt->getAnswers().begin(); feaIt != histIt->getAnswers().end(); ++feaIt ) {
  const TriggerElement::FeatureAccessHelper& fea = *feaIt;
  std::cout << "Objects access HISTORY: got back: " << fea << std::endl;
  }
  }
  */
  RoICacheHistory::instance().stopHistory();

  cache.finish( te->getFeatureAccessHelpers().size() );
  return true;
}

void RoICacheHelper::reset() {
  m_caches.clear();
}

/*
void printObjects(MsgStream* mlog, const TriggerElement* cache, const TriggerElement* te) {
  if (mlog && mlog->level( ) <= MSG::DEBUG) {    
    std::string l1; 
    TrigConf::HLTTriggerElement::getLabel(te->getId(), l1);
    std::string l2; 
    TrigConf::HLTTriggerElement::getLabel(cache->getId(), l2);

    (*mlog) << MSG::DEBUG << "found objects in querried TE: " << l1 << " in already executed (cache): " << l2 << endreq;
    (*mlog) << MSG::DEBUG << "cache had : " <<cache->getPreviousFeatures().size() 
	    << " while TE has: " << te->getPreviousFeatures().size() << endreq;
    unsigned int i;
    for ( i = 0 ; i < te->getPreviousFeatures().size(); ++i ) {
      (*mlog) << MSG::DEBUG 
	      << (te->getPreviousFeatures()[i] == cache->getPreviousFeatures()[i] ? "==": "!!")
	      << "this TE " << te->getPreviousFeatures()[i] << " in cache: "  << cache->getPreviousFeatures()[i] 
	      << endreq;
    }  
  }
}
*/
// cache entry methods

bool RoICacheHelper::CacheEntry::operator== (const std::vector<TriggerElement*>& roite) const {
  return m_rois == std::set<const TriggerElement*>(roite.begin(), roite.end());
}


bool RoICacheHelper::CacheEntry::operator== (const TriggerElement* curr) const {
  PCI("   Comparing this TE to the next cache entry")

  if (s_cachingMode == None) {
    PCI ( "caching disabled" );
    return false;
  }
  

  // check if set of RoIs is identical
  const std::vector<TriggerElement*> &tvec = TrigNavStructure::getRoINodes(curr);
  if ( m_rois != std::set<const TriggerElement*>(tvec.begin(), tvec.end()) ) {
    PCI ( "distinct set of RoIs" );
    return false;
  }
  PCI ( "Seen this RoI already, checking further" );
  if ( s_cachingMode == RoIOnly )
    return true;

  // Strict check: use all features in previous TEs
  if ( s_cachingMode == AllObjects ) {
    if (curr->getPreviousFeatures().size() != m_cacheHolder->getPreviousFeatures().size() - (m_cacheHolder->getFeatureAccessHelpers().size() - begin()) ) {
      return false;
    }
    
    if ( std::equal(curr->getPreviousFeatures().begin(), curr->getPreviousFeatures().end(),
                    m_cacheHolder->getPreviousFeatures().begin()) ) {
      return true;
    }

    return false;
  }
  
  // History based check
  if ( s_cachingMode != HistoryBased ) {
    PCI ( "history based check is the last to try but the option is different" );
    return false;
  }
  
  // No history... no future
  if(m_questions.empty()) {
    //    MsgStream log(Athena::getMessageSvc(), "RoICacheHelper");
    //    log << MSG::DEBUG << "No history available in this RoI, algorithms did not request any objecrs, safer not to cache" << endreq;
    return false;
  }

  // The big loop below is repeating the same querries (getFeture(s) calls)
  //   as on RoI as were done originaly by the running algorithm.
  // This querries were recorded together with answers.
  // When repeated now, any divergence in answering the same questions is good reason to run algorithm again.
  // When the loop goes to the end it means all the answers were identical as when we run for the first time.
  // Then thre is no reason to run algorithm again and only objects should be reattached.


  // Need navigation to answer questions
  NavigationCore *navig = RoICacheHistory::instance().getNavigation();
  if(!navig) return false;

  const std::vector<TriggerElement*> &pvec = TrigNavStructure::getDirectPredecessors(curr);

  // Start of the loop over previously asked questions.
  for(FeatureCallVec::const_iterator it = m_questions.begin(); it != m_questions.end(); ++it) {
    if ( !it->isget() )
      continue;

    FeatureVec fvec;


    if(it->getInitTE() == it->getWorkTE()) {
      // History starts at the same TE (i.e. getFeature was called on outputTE in Fex)
      // We ask again the same question (giving the same clid and label)
      navig->getFeatureAccessors(curr, it->clid(), it->label(), it->issingle(), fvec, false, true);
    } else if(pvec.size() == 1) {
      // Check relationship at cached TE
      const std::vector<TriggerElement*> &iwvec = TrigNavStructure::getDirectPredecessors(it->getWorkTE());
      if(iwvec.size() != 1 || iwvec[0] != it->getInitTE()) {
        // Should never happen - answers always come from single pred.
	PCI("caching does not happen because of iwvec size !- 1 or init TE used");
        return false;
      }
      
      // History starts at the previous TE (i.e. getFeature was called on inputTE in Fex)
      navig->getFeatureAccessors(pvec[0], it->clid(), it->label(), it->issingle(), fvec, false, true);
    } else {
      // Previous history not unique (i.e. the navigaiton tree was traversed in some custom way in Fex)
      PCI("previous history not unique");
      return false;
    }

    // check if getFeature was called rather than getFeature*s*
    if ( it->issingle()) {
      
      if(  it->getAnswers().empty()  ) {
        // the answer in first case was empty - it means no feature was found
        // It is fine if the answer was also empty this time.
        if ( fvec.empty() ) {
          continue;
        } else {
          // Else we need running. In first pass Fex could not get this object while now can. Clearly it would do different result now.
	  PCI("Now can get the object");
          return false;
        }
      } else {
        if (fvec.empty()) {
	  PCI("Object was available before but now is not");
          return false; // This is inverted situation, first time we got answer, but not this case, again we need to to run.
	}
        // Now we need to do fancy check
        // In first pass and now we got some answers (features found). Now we need to check if they would be the same ...
        // but not completly, i.e. only as many objects as were of interest first time (i.e. one)
        if ( !std::equal(it->getAnswers().begin(), it->getAnswers().end(), fvec.begin()) ) {
	  PCI("result of getFeature (i.e. single obj request) and currently available set of objects differ in content " << fvec.size() << " " <<  it->getAnswers().size());
	  //	  xx_print(fvec, "questions");
	  //	  xx_print(it->getAnswers(), "in cache");
          return false;

        }
      }
    } else {
      // This is case for getFeature*s* (many features requested)
      // First of all need to check if the number of objects is the same.
      if( fvec.size() !=  it->getAnswers().size() ) {
	PCI("history of getFeatures (i.e. multiple objects) and currently available set of objects differ in size " << fvec.size() << " " <<  it->getAnswers().size());
	//	xx_print(fvec, "questions");
	//	xx_print(it->getAnswers(), "in cache");

        return false;
      }
      
      // if number of objects is the same we need to check if the answers (features) we obtain now woudl be the same as in first pass
      if ( fvec !=  it->getAnswers() ) {

	PCI("result of getFeatures differs form the one available in the cache");
	//	xx_print(fvec, "questions");
	//	xx_print(it->getAnswers(), "in cache");

        return false;
      }
    }
    PCI("Verification of that call against cached history sucessful");
    //xx_print(fvec, "questions");
    //    xx_print(it->getAnswers(), "in cache");
    
  }
  //     MsgStream log(Athena::getMessageSvc(), "RoICacheHelper");
  //     log << MSG::DEBUG << "history the same, yesss" << endreq;
  PCI("          History is the same - can cache");	
  return true; 
}
