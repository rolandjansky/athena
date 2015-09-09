/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_ROICACHEHELPER_H
#define TRIGNAVIGATION_ROICACHEHELPER_H

#include <iostream>
#include <vector>
#include <set>

#include "TrigNavigation/RoICacheHistory.h"
#include "TrigNavigation/TriggerElement.h"

namespace HLT {
    /**
     * \brief class to support in RoI caching
     * If algo wants to profit from caching it should create private objects
     * of HLT::TriggerElement::CacheHelper or inherit from it.
     *
     * Each time when the algo wants to run on TE given as an argument it should
     * check if needsExecution() returns true. If so, then the algorithms needs to be executed.
     * The execution needs to be wrapped by calls to start/stop caching
     * startCaching(TE)
     * ... algo execution
     * stopCaching(TE)
     *
     * If false is returned then caching has heppened. Nothing else needs to be done.
     *
     * After event processing the cahce needs to be reset.
     *
     *
     */


  class RoICacheHelper {
  public:

    /**
     * \brief adds objects from cache to this TE as if the algorithm would have run on this TE
     * \param te is trigger element, non-const since eventually it will be updated from cache
     * \return true if all worked correctly, false if failed, this should never ahppen, if it does it shoudl be severe error
     */
    bool cache ( TriggerElement* te );
    



    /**
     * \brief checks if there was an execution on RoI already
     * \param te is trigger element
     * \return true if running is needed, false if not
     */    
    bool needsExecution ( const TriggerElement* te );



    /**
     * \brief checks if there was an execution on RoI already
     * \param te is trigger element
     * \return true if running is needed, false if not
     */        
    bool isInputAccepted ( const TriggerElement* te );
    

    /**
     * \brief starts collecting cache
     * \param te the same te as for needsExecution
     * \warning For optimization reasons this calls assume that is done only after needsExecution returned true. If that is not the case it will make mess.
     * \return true if all went OK, false if there is logica failure, (this method can only be called after needsExecution(te) == true)
     */
    bool startCaching( const TriggerElement* te );

    /**
     * \brief stop collecting cache
     * \see startCaching(const TriggerElement*)
     */
    bool stopCaching( const TriggerElement* te );

    /**
     * \brief reset caches for this event
     */
    void reset();

    enum CachingMode { None=0, RoIOnly=1, AllObjects=2, HistoryBased=3 };
    static enum CachingMode s_cachingMode;
   
  private:
    /**
     * \brief Helper class for caching.
     * This class keeps cache for all RoIs.
     * That means it remembers if the algo was run on this RoIs and whcih objects reated for the first time.
     **/

    class CacheEntry {
    public:

      typedef std::vector<HLT::RoICacheHistory::FeatureCall> FeatureCallVec;
      typedef HLT::RoICacheHistory::FeatureCall FeatureCall;

      typedef std::vector<HLT::TriggerElement::FeatureAccessHelper> FeatureVec;
      typedef HLT::TriggerElement::FeatureAccessHelper Feature;

      bool operator== (const std::vector<TriggerElement*>& roite) const;
      bool operator== (const TriggerElement* te) const;

      CacheEntry(const TriggerElement* cacheHolder ) 
	:  m_rois(TrigNavStructure::getRoINodes(cacheHolder).begin(), TrigNavStructure::getRoINodes(cacheHolder).end()), 
	m_cacheHolder(cacheHolder), 
	m_begin(0),
	m_end(0) {
      }

      /*
	CacheEntry(const std::vector<TriggerElement*>& rois, const TriggerElement* cacheHolder=0 ) 
	:  m_rois(rois.begin(), rois.end()), 
	m_cacheHolder(cacheHolder), 
	m_begin(0),
	m_end(0) {
	
	//	std::cout << "setsize" << m_rois.size() << "\n";
	//	std::set<const TriggerElement*>(m_rois.begin(), m_rois.end());
	//	for ( std::set<const TriggerElement*>::const_iterator is = m_rois.begin(); is != m_rois.end(); ++is)
	//	  std::cout << *is << " creset \n";
	}
      */

      
      void start(unsigned int begin) { m_begin = m_end = begin; } //!< record moment just before execution when no feature was yet attached
      void finish(unsigned int end) { m_end = end; }              //!< record moment just after execution when no more featuresd are attached

      void add(const FeatureCallVec &qvec) { m_questions = qvec; }

      unsigned int begin() const { return m_begin; }              //!< 
      unsigned int end() const { return m_end; }

      const TriggerElement* holder() { return m_cacheHolder; }  
      void holder(const TriggerElement* te) { m_cacheHolder = te; }
      unsigned int size() const { return m_end - m_begin; } //!< gets number of objects attaced by algo

    private:

      std::set<const TriggerElement*> m_rois;  //!< handy varaible to keep all RoIs of given TE
      const TriggerElement* m_cacheHolder;     //!< the first TE processed in given RoI (from it the objects will be copied to all cached TEs)
      FeatureCallVec m_questions;

      unsigned int m_begin;
      unsigned int m_end;
    };

    std::vector<class CacheEntry> m_caches;   //!< caches for all RoIs
  };
}

#endif //#ifndef TRIGNAVIGATION_ROICACHEHELPER_H
