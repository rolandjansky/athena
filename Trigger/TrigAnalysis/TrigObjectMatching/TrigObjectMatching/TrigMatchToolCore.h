// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGOBJECTMATCHING_TRIGMATCHTOOLCORE_H
#define TRIGOBJECTMATCHING_TRIGMATCHTOOLCORE_H

#define TRIG_OBJECT_MATCHING_VERSION_1

// STL include(s):
#include <vector>
#include <unordered_map>
#include <mutex>

// includes for trigger matching framework
#include "TrigObjectMatching/ObjectMatching.h"
#include "TrigObjectMatching/DistanceFunctor.h"
#include "TrigObjectMatching/TraitDefs.h"
#include "TrigObjectMatching/TrigMatchDefs.h"

// includes for navigation access
#include "TrigDecisionTool/TrigDecisionToolCore.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"

namespace Trig {
   class FeatureContainer;
}

/**
 * @brief The TrigMatchToolCore defines the core functionality of
 *        the TrigMatchTool and is used for matching trigger and
 *        reconstructed objects.
 */

class TrigMatchToolCore : public ObjectMatching {

private:
  struct SlotCache;


public:
   // Default constructor and destructor
   TrigMatchToolCore();
   virtual ~TrigMatchToolCore();

   /**************************************************
    *          Trigger Access Functions              *
    **************************************************/

   /**
    * @brief getTriggerObjects returns the objects of the specified
    *        type from the specified chain.  Note that if
    *        onlyPassedFeatures is true, then only features from
    *        passed chains will be returned.
    */
   template< typename trigType >
   std::vector< const trigType* >
   getTriggerObjects( const std::string& chainName,
                      bool onlyPassedFeatures ) const;

   /**************************************************
    *     Offline -> Trigger Object matching         *
    **************************************************/

   /**
    * @brief matchToTriggerObjects returns the objects of type trigType
    *        from chain chainName, which match to baseObject, sorted by
    *        distance from baseObject according to metric.
    */
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchToTriggerObjects( const baseType* baseObject,
                          const std::string& chainName, 
                          float maxDistance,
                          bool onlyPassedFeatures,
                          const DistanceFunctor< trigType, baseType >* metric ) const;

   // matching using the default metric - same as above except
   // will match using deltaR, so no metric is needed
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchToTriggerObjects( const baseType *baseObject,
                          const std::string& chainName, 
                          float maxDistance = 0.1,
                          bool onlyPassedFeatures = false ) const;

   /**
    * @brief matchToTriggerObject returns the object of type trigType
    *        from chain chainName, which has the lowest distance
    *        to baseObject according to metric.
    */
   template< typename trigType, typename baseType >
   const trigType*
   matchToTriggerObject( const baseType* baseObject,
                         const std::string& chainName, 
                         float maxDistance,
                         bool onlyPassedFeatures,
                         const DistanceFunctor< trigType, baseType >* metric ) const;

   // matching using the default metric - same as above except
   // will use match using deltaR, so no metric is needed
   template< typename trigType, typename baseType >
   const trigType*
   matchToTriggerObject( const baseType* baseObject,
                         const std::string& chainName, 
                         float maxDistance = 0.1,
                         bool onlyPassedFeatures = false ) const;

   // C++ will happily cast a bool to a float, which means you can
   // call the above like matchToTriggerObject(object, chain, false)
   // which is clearly not intended.  This will warn people
   template< typename trigType, typename baseType >
   const trigType*
   matchToTriggerObject( const baseType * /*baseObject*/,
                         const std::string& /*chainName*/, 
                         bool /*onlyPassedFeatures*/ ) const {

      this->warning( "You have called matchToTriggerObject incorrectly.");
      this->warning( "Note that the correct use is: object to match to, "
                     "chainName, maxDistance, onlyPassedFeatures");
      this->warning( "Your current call is of the form: object to match "
                     "to, chainName, onlyPassedFeatures");

      return 0;
   }

   /**
    * @brief matchedTriggerObjects returns a vector of trigger
    *        objects from chain chainName that match to one or
    *        more of the objects passed according to the matching
    *        criteria specified.  If onlyBestMatch is true, then
    *        trigger objects will only be returned if they are the
    *        closest match to one of the baseObjects.
    */
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchedTriggerObjects( const DataVector<baseType>& baseObjects,
                          const std::string& chainName, 
                          float maxDistance,
                          bool onlyBestMatch,
                          bool onlyPassedFeatures,
                          const DistanceFunctor< trigType, baseType >* metric ) const;

   // matching using the default metric - same as above except
   // will use match using deltaR, so no metric is needed
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchedTriggerObjects( const DataVector<baseType>& baseObjects,
                          const std::string& chainName, 
                          float maxDistance = 0.1,
                          bool onlyBestMatch = true,
                          bool onlyPassedFeatures = false ) const;

   // Versions with std::vector instead of DataVector
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchedTriggerObjects( const std::vector< const baseType* >& baseObjects,
                          const std::string& chainName, 
                          float maxDistance,
                          bool onlyBestMatch,
                          bool onlyPassedFeatures,
                          const DistanceFunctor< trigType, baseType > *metric ) const;

   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchedTriggerObjects( const std::vector< const baseType* >& baseObjects,
                          const std::string& chainName, 
                          float maxDistance = 0.1,
                          bool onlyBestMatch = true,
                          bool onlyPassedFeatures = false ) const;

   /**
    * @brief unmatchedTriggerObjects returns a vector of trigger
    *        objects from chain chainName that do not match to
    *        any of the objects passed according to the matching
    *        criteria specified.  If onlyBestMatch is true, then
    *        trigger objects will only be considered matched if
    *        they are the closest object to the baseObject.
    */
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   unmatchedTriggerObjects( const DataVector<baseType>& baseObjects,
                            const std::string& chainName, 
                            float maxDistance,
                            bool onlyBestMatch,
                            bool onlyPassedFeatures,
                            const DistanceFunctor< trigType, baseType >* metric ) const;

   // matching using the default metric - same as above except
   // will use match using deltaR, so no metric is needed
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   unmatchedTriggerObjects( const DataVector<baseType>& baseObjects,
                            const std::string& chainName, 
                            float maxDistance = 0.1,
                            bool onlyBestMatch = true,
                            bool onlyPassedFeatures = false ) const;

   // versions with std::vectors instead of DataVectors
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   unmatchedTriggerObjects( const std::vector<const baseType*>& baseObjects,
                            const std::string& chainName, 
                            float maxDistance,
                            bool onlyBestMatch,
                            bool onlyPassedFeatures,
                            const DistanceFunctor< trigType, baseType >* metric ) const;

   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   unmatchedTriggerObjects( const std::vector< const baseType* >& baseObjects,
                            const std::string& chainName, 
                            float maxDistance = 0.1,
                            bool onlyBestMatch = true,
                            bool onlyPassedFeatures = false ) const;

   /**************************************************
    *      Offline -> Trigger Chain matching         *
    **************************************************/

   /**
    * @brief chainPassedByObject returns true if the supplied object
    *        matches a trigger object of type trigType in the
    *        supplied chain, associated with an active TE, and 
    *        false otherwise.  Please note that this is a bit
    *        tricky with multi-object triggers.  This function
    *        will return true if the object you supply passes
    *        its portion of the trigger requirements, which
    *        can happen even if the trigger as a whole was not passed.
    */
   template< typename trigType, typename baseType >
   bool
   chainPassedByObject( const baseType* baseObject,
                        const std::string& chainName,
                        float maxDistance,
                        const DistanceFunctor< trigType, baseType > *metric ) const;

   // default metric version - matching will be done via deltaR,
   // so it is not necessary to supply a metric
   template< typename trigType, typename baseType >
   bool
   chainPassedByObject( const baseType *baseObject,
                        const std::string &chainName,
                        float maxDistance = 0.1 ) const;

   /**
    * @brief chainsPassedByObject returns a list of the chains
    *        which are marked as passed in the navigation, and in which
    *        the supplied recoObject matches an object of type trigType
    *        with distance, as calculated by metric, of less than maxDistance.
    *        Please note that this can be a bit tricky with multi-object
    *        triggers.  A chain will be added to this list if the object you
    *        supply passes its portion of the trigger requirements, which can
    *        happen even if the trigger as a whole was not passed.
    */
   template< typename trigType, typename baseType >
   std::vector< std::string >
   chainsPassedByObject( const baseType* baseObject,
                         float maxDistance,
                         const DistanceFunctor< trigType, baseType >* metric ) const;

   // default metric version - matching will be done via
   // deltaR, so a metric is not required
   template< typename trigType, typename baseType >
   std::vector< std::string >
   chainsPassedByObject( const baseType *baseObject,
                         float maxDistance = 0.1 ) const;

   /**************************************************
    *      Trigger Chain -> Offline matching         *
    **************************************************/

   /**
    * @brief objectsInChain returns a container of reco objects
    *        which match to objects of type trigType in the specified chain,
    *        where a match is defined by a distance, according to the supplied
    *        metric, of less than maxDistance.  If onlyPassedFeatures is true
    *        then only matches with trigger objects in passed chains will
    *        be considered.
    */
   template< typename trigType, typename baseType >
   std::vector< const baseType* >
   objectsInChain( const std::vector< const baseType* >& baseObjects,
                   const std::string& chainName,
                   bool onlyPassedFeatures, 
                   float maxDistance,
                   const DistanceFunctor< trigType, baseType >* metric ) const;

   // default metric version - matching will be done via deltaR,
   // and so it is not necessary to supply a metric
   template< typename trigType, typename baseType >
   std::vector< const baseType* >
   objectsInChain( const std::vector< const baseType* >& baseObjects,
                   const std::string& chainName,
                   bool onlyPassedFeatures = false, 
                   float maxDistance = 0.1 ) const;

   /**
    * @brief matchToAllObjects returns a map from matched objects
    *        to the chains they were found in.  Matching is done
    *        according to criteria in matchToObjects.
    */
   template< typename trigType, typename baseType >
   std::map< const trigType*, std::vector< std::string > >
   matchToAllTriggerObjects( const baseType* baseObject,
                             float maxDistance,
                             bool onlyPassedFeatures,
                             const DistanceFunctor< trigType, baseType >* metric ) const;

   // default metric version - matching will be done via deltaR,
   // and so it is not necessary to supply a metric
   template< typename trigType, typename baseType >
   std::map< const trigType*, std::vector< std::string > >
   matchToAllTriggerObjects( const baseType* baseObject,
                             float maxDistance = 0.1,
                             bool onlyPassedFeatures = false ) const;

   /**********************************************************/
   /*           Setting running parameters                   */
   /**********************************************************/
   class FeatureLabelHolder;
    
   /**
    * @brief setFeatureLabel is used to set the label to be used
    *        when extracting features from the navigation.  Note that
    *        it should only be used like:
    *
    *        <code>
    *        m_matchTool->setFeatureLabel("label")->matchToTriggerObject( recoObj,<br/>
    *                                                                     chainName );
    *        </code>
    *       
    *        and never on its own.  Note that it will only affect the
    *        very next match.
    */
   TrigMatchToolCore::FeatureLabelHolder
   setFeatureLabel( const std::string& label ) {
      SlotCache& slotCache = *m_slotCache;
      std::unique_lock<SlotCache::mutex_t> lock (slotCache.m_mutex);
      slotCache.m_featureLabel = label;
      slotCache.m_caches = &slotCache.m_cacheMap[label];
      return FeatureLabelHolder( this, slotCache, std::move(lock) );
   }

   // This class does two things for us
   // (1) It remembers when it was created, so that it can reset the feature label
   //     after it goes out of scope.  This prevents users from accidentally setting
   //     the label and then not resetting it properly.
   // (2) It allows us to overload the operators to make access to the trig match tool
   //     uniform
   class FeatureLabelHolder {

   public:
      FeatureLabelHolder( TrigMatchToolCore* matchTool,
                          SlotCache& slotCache,
                          std::unique_lock<std::recursive_mutex>&& lock)
         : m_matchTool( matchTool ),
           m_slotCache( slotCache ),
           m_lock( std::move (lock) )
      {}

      ~FeatureLabelHolder() {
         m_slotCache.resetFeatureLabel();
      }

      TrigMatchToolCore* operator->() const {
         return m_matchTool;
      }

      TrigMatchToolCore* matchTool() const {
         return m_matchTool;
      }

   private:
      TrigMatchToolCore *m_matchTool;
      SlotCache& m_slotCache;
      std::unique_lock<std::recursive_mutex> m_lock;
   }; // class FeatureLabelHolder


protected:
   void setTDT( Trig::TrigDecisionToolCore* tdt ) {
      m_trigDecisionToolCore = tdt;
   }

   // called on end event if we're able to get access to such information
   virtual void endEvent();


   // Clear saved chain name -> index mapping and rebuild from current
   // configuration.
   void clearChainIndex();


private:
   // Associate from chain name to integer index, and l1l2map.
   // Methods of this class are thread-safe.
   class ChainNameIndex
   {
   public:
     ChainNameIndex (TrigMatchToolCore* core);
     size_t chainNameToIndex (const std::string& chainName);
     std::vector<std::string> configuredChainNames();
     std::string chainName (size_t index);
     void clear();
     std::string propagateChainNames (const std::string& chainName);
    

   private:
     void assertConfiguredChainNames();

     typedef std::mutex mutex_t;
     typedef std::lock_guard<mutex_t> lock_t;

     mutex_t m_mutex;
    
     TrigMatchToolCore* m_core;

     // List of chain names.
     // The first m_nConfiguredChainNames are those that come from
     // the configuration.  The remainder are those added for user
     // queries (they could be regexps).
     std::vector< std::string > m_chainNames;
     size_t m_nConfiguredChainNames = 0;

     // Map from chain names to indices.
     typedef std::unordered_map<std::string, size_t> chainIndexMap_t ;
     chainIndexMap_t m_chainIndexMap;

     // cache the map from l1 items to the combined l2
     // string for access in the tdt
     std::map< std::string, std::string > m_l1l2Map;
   };
   mutable ChainNameIndex m_chainNameIndex ATLAS_THREAD_SAFE;


  // function for printing warnings - note that this depends on whether
   // you are in ARA or not
   virtual void warning( const std::string& w ) const = 0;

   // status functions for determining what information we have access to
   // This allows the correct caching to take place based upon whether or
   // not the tool is running in athena.  In athena, we can register with
   // store gate to get call backs when the trig decision changes, but
   // we cannot get this info in ARA.
   virtual bool changedDecisionAware() const { return false; };

   // return configured chain names.  Note that 
   // this is different for ARA and athena versions of the tool.
   virtual std::vector<std::string> getConfiguredChainNames() const = 0;

   // Functionality for loading the trigger objects from the navigation

   // Getting a container from the trig decision tool.  Note that this is
   // an abstract function, as the exact method is different for
   // ARA and Athena.
   virtual Trig::FeatureContainer
   getFeatureContainer( const std::string &chainName, const int condition ) const = 0;

   // determine how to propagate L1 chain names to L2 chain names
   template< typename trait >
   std::string propagateChainNames( const std::string& chainName,
                                    const trait* ) const
   {
      return chainName;
   }
   std::string propagateChainNames( const std::string& chainName,
                                    const TrigMatch::AncestorAttached* ) const
   {
      return this->propagateChainNames( chainName );
   }
   virtual std::string propagateChainNames( const std::string& chainName ) const;
   virtual std::string lowerChainName( const std::string& chainName ) const = 0;

   // fills objects with the trigger objects from the chain name
   // with only passed features as desired.  Queries cache first
   // if possible
   template< typename trigType >
   void getObjects( std::vector< const trigType* >& objects,
                    const std::string& chainName,
                    bool onlyPassedFeatures ) const;

   template< typename trigType >
   void getObjects( std::vector< const trigType* >& objects,
                    size_t chainIndex,
                    bool onlyPassedFeatures ) const;
    
   // function for loading objects that are attached directly
   // to the navigation
   template< typename trigType >
   void collectObjects( const std::string& featureLabel,
                        std::vector< const trigType* >& objects,
                        const Trig::FeatureContainer &featureContainer,
                        bool onlyPassedFeatures,
                        const TrigMatch::DirectAttached* ) const;

   // function for loading objects that are attached as
   // containers to the navigation
   template< typename trigType, typename contType >
   void collectObjects( const std::string& featureLabel,
                        std::vector< const trigType* >& objects,
                        const Trig::FeatureContainer& featureContainer,
                        bool onlyPassedFeatures,
                        const contType* ) const;

   // function for loading l1 objects from the navigation
   template<typename trigType>
   void collectObjects( const std::string& featureLabel,
                        std::vector< const trigType* >& objects,
                        const Trig::FeatureContainer& featureContainer,
                        bool onlyPassedFeatures,
                        const TrigMatch::AncestorAttached* ) const;

   size_t chainNameToIndex (const std::string& chainName) const;

   /**
    * @brief Alternate version of @c getTriggerObjects taking a chain index.
    */
   template< typename trigType >
   std::vector< const trigType* >
   getTriggerObjects( size_t chainIndex,
                      bool onlyPassedFeatures ) const;

   /**
    * @brief matchToTriggerObjects returns the objects of type trigType
    *        from chain chainName, which match to baseObject, sorted by
    *        distance from baseObject according to metric.
    */
   template< typename trigType, typename baseType >
   std::vector< const trigType* >
   matchToTriggerObjects( const baseType* baseObject,
                          size_t chainIndex,
                          float maxDistance,
                          bool onlyPassedFeatures,
                          const DistanceFunctor< trigType, baseType >* metric ) const;

   template< typename trigType, typename baseType >
   bool
   chainPassedByObject( const baseType* baseObject,
                        size_t chainIndex,
                        float maxDistance,
                        const DistanceFunctor< trigType, baseType > *metric ) const;

   // cache from chainName and onlyPassedFeatures to std::vector<Trig::Feature<trigType> >
   class TrigFeatureCacheBase {
   public:
      virtual ~TrigFeatureCacheBase() {}
      virtual void clear() = 0;
   }; // class TrigFeatureCacheBase

   template<typename trigType>
   class TrigFeatureCache : public TrigFeatureCacheBase {
   public:
      TrigFeatureCache()
      {
        m_size[0] = m_size[1] = 0;
      }

      virtual void clear()
      {
        for (int i = 0; i < 2; i++) {
          cacheDatavec_t().swap (m_data[i]);
        }
      }

     bool get (size_t chainIndex,
               bool onlyPassedFeatures,
               std::vector< const trigType* >& objects)
     {
       cacheDatavec_t& v = m_data[onlyPassedFeatures];
       if (chainIndex < v.size() && v[chainIndex].valid) {
         objects = v[chainIndex].data;
         return true;
       }
       return false;

     }

      void add( size_t chainIndex, 
                bool onlyPassedFeatures, 
                const std::vector< const trigType* >& data)
      {
        cacheDatavec_t& v = m_data[onlyPassedFeatures];
        if (chainIndex >= v.size()) {
          size_t& sz = m_size[onlyPassedFeatures];
          if (chainIndex >= sz)
            sz = chainIndex + 1;
          v.resize (sz);
        }
        v[chainIndex].valid = true;
        v[chainIndex].data = data;
      }

   private:
      struct cacheData_t {
        bool valid;
        std::vector<const trigType*> data;
        cacheData_t() : valid(false) {}
        void clear()
        {
          valid = false;
          std::vector<const trigType*>().swap (data);
        }
      };
      typedef std::vector<cacheData_t> cacheDatavec_t;
        cacheDatavec_t m_data[2];
      size_t m_size[2];
   }; // class TrigFeatureCache

   // TDT access
   Trig::TrigDecisionToolCore* m_trigDecisionToolCore;


   class TypeMap
   {
   public:
     int key (const std::type_info* tid);

   private:
     typedef std::mutex mutex_t;
     typedef std::lock_guard<mutex_t> lock_t;

     typedef std::unordered_map<const std::type_info*, int> typeMap_t;
     typeMap_t m_typeMap;
     mutex_t m_mutex;
   };
   mutable TypeMap m_typeMap ATLAS_THREAD_SAFE;


   struct SlotCache
   {
     SlotCache()
     {
       m_caches = &m_cacheMap[""];
     }

     ~SlotCache()
     {
       for (const cacheMap_t::value_type& p : m_cacheMap) {
         for (TrigFeatureCacheBase* cache : p.second) {
           delete cache;
         }
       }
     }

     void clear()
     {
       lock_t lock (m_mutex);
       for (const cacheMap_t::value_type& p : m_cacheMap) {
         for (TrigFeatureCacheBase* cache : p.second) {
           cache->clear();
         }
       }
       std::vector<Trig::FeatureContainer>().swap (m_featureContainers);
       std::vector<bool>().swap (m_featureContainersValid);
     }
     
     /**
      * @brief resetFeatureLabel is used to reset the label to be used
      *        when extracting features from the navigation.  It should not
      *        be necessary for users to call this function - it will be
      *        reset automatically.
      */
     void resetFeatureLabel()
     {
       m_featureLabel = "";
       m_caches = &m_cacheMap[""];
     }

     std::string m_featureLabel;

     typedef std::vector<TrigFeatureCacheBase*> cacheVec_t;
   
     // Current cache vector.
     cacheVec_t* m_caches;

     typedef std::unordered_map<std::string, cacheVec_t> cacheMap_t;
     cacheMap_t m_cacheMap;
 
     std::vector<Trig::FeatureContainer> m_featureContainers;
     std::vector<bool> m_featureContainersValid;
     size_t m_nFeatureContainers = 100;

    typedef std::recursive_mutex mutex_t;
     typedef std::lock_guard<mutex_t> lock_t;
     mutex_t m_mutex;
   };
   mutable SG::SlotSpecificObj<SlotCache> m_slotCache ATLAS_THREAD_SAFE;

   template <typename trigType>
   TrigFeatureCache<trigType>& getCache (int type_key,
                                         SlotCache& slotCache,
                                         const SlotCache::lock_t& lock) const;

   TrigFeatureCacheBase*& getCache1 (const std::type_info* tid, int type_key,
                                     SlotCache& slotCache,
                                     const SlotCache::lock_t& lock) const;

   const Trig::FeatureContainer&
   getCachedFeatureContainer (size_t chainIndex,
                              SlotCache& cache,
                              const SlotCache::lock_t& lock) const;

}; // end TrigMatchToolCore declaration

// Include the implementation:
#include "TrigObjectMatching/TrigMatchToolCore.icc"

#endif // TRIGOBJECTMATCHING_TRIGMATCHTOOLCORE_H
