/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_CACHE_GLOBAL_MEMORY_H
#define TRIGGER_DECISION_TOOL_CACHE_GLOBAL_MEMORY_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : CacheGlobalMemory
 *
 * @brief registry for ChainGroups
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/

#include<iterator>
#include<vector>
#include<list>
#include<set>
#include<map>
#include<string>
#include "boost/foreach.hpp"
#include <ext/hash_map>

#include "TrigDecisionEvent/TrigDecision.h"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTStreamTag.h"

#include "TrigConfL1Data/CTPConfig.h"

#include "TrigSteeringEvent/Chain.h"


#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "TrigDecisionTool/Logger.h"


namespace HLT {
  class Chain;
  class TriggerElement;
  class NavigationCore;
}

namespace LVL1CTP {
  class Lvl1Item;
  class Lvl1Result;
}

namespace Trig {

  class ChainGroup;

  class CacheGlobalMemory : public virtual Logger {

  public:
    // constructors, destructor
    CacheGlobalMemory();
    ~CacheGlobalMemory();

    /**
     * @brief
     **/

    /**
     * @brief creates new chain group
     * @param patterns list of patterns i.e. regexes, if chain groups already exists it is just given back
     *        The chain group is considered to exist if the patterns are the same. 
     *        Patterns are stored and made unique i.e. "EF_mu4", "EF_mu6" is the same as "EF_mu6", "EF_mu4".
     *        It is not the same though as "EF_mu.*" even if in particular case that would mean the same 2 chains.
     *        
     * @param alias is the short human readable name for the triggers which are in the group i.e. myMuons
     **/
    const Trig::ChainGroup* createChainGroup(const std::vector< std::string >& patterns,
                                             const std::string& alias="");
    /**
     * @brief Updates configuration of the chain groups 
     * (i.e. regexes are reapplied to new set of chains)
     **/
    void update(const TrigConf::HLTChainList* confChains,
                const TrigConf::CTPConfig* ctp);

    const LVL1CTP::Lvl1Item* item(const std::string& name) const;            //!< CTP item from given name
    const LVL1CTP::Lvl1Item* item(const TrigConf::TriggerItem& i) const;     //!< CTP item from given config item
    const TrigConf::TriggerItem* config_item(const std::string& name) const; //!< CTP config item from given name
    int item_prescale(int ctpid) const;                                      //!< Prescale for CPT item
    
    const HLT::Chain* chain(const std::string& name) const;                  //!< HLT chain object from given name (0 pointer returned if no match)
    const HLT::Chain* chain(const TrigConf::HLTChain& chIt) const;           //!< HLT chain object from given config chain
    const TrigConf::HLTChain* config_chain(const std::string& name) const;   //!< HLT config chain from given name

    const HLT::NavigationCore* navigation() const { return m_navigation; }  //!< gives back pointer to navigation object
    void navigation(HLT::NavigationCore* nav) { m_navigation = nav; }       //!< sets navigation object pointer

    void decisionHandle(DecisionObjectHandle* h) { m_decisionObjectHandle = h; }

    
    std::map< std::vector< std::string >, Trig::ChainGroup* >& getChainGroups() {return m_chainGroupsRef;};
    //    std::map<unsigned, const LVL1CTP::Lvl1Item*>  getItems() {return m_items;};
    //    std::map<unsigned, const LVL1CTP::Lvl1Item*>  getItems() const {return m_items;};
    //    std::map<unsigned, const HLT::Chain*>         getL2chains() {return m_l2chains;};
    //    std::map<unsigned, const HLT::Chain*>         getL2chains() const {return m_l2chains;};
    //    std::map<unsigned, const HLT::Chain*>         getEFchains() {return m_efchains;};
    //    std::map<unsigned, const HLT::Chain*>         getEFchains() const {return m_efchains;};
    std::map<std::string, std::vector<std::string> > getStreams() {return m_streams;};
    std::map<std::string, std::vector<std::string> > getStreams() const {return m_streams;};

    /**
     * @brief cheks if new event arrived with the decision
     * Need tu use before any call to CacheGlobalMemory.
     * @return true if all went fine about decision, false otherwise     
     **/
    bool assert_decision();


    // 
    template<class T>
    void deleteAtTheEndOfEvent(T t) const { m_deleteAtEndOfEvent.insert(t); }


    

  private:
    friend class DecisionAccess;
    Trig::DecisionObjectHandle* m_decisionObjectHandle;

    /**
     * @brief unpacks whole trigger decision for the event
     **/
    StatusCode unpackDecision(const TrigDec::TrigDecision*);

    /**
     * @brief unpacks HLT navigation structure (object access)
     **/
    StatusCode unpackNavigation(const TrigDec::TrigDecision*);              

    /**
     * @brief unpacks chains for one level (L2 or EF) 
     **/
    StatusCode unpackChains(const std::vector<uint32_t>& serialized_chains,
                            std::map<unsigned, HLT::Chain*>& cache,  
                            std::map<std::string, const HLT::Chain*>& output);

    /**
     * @brief unpacks CTP decision bits
     **/
    StatusCode unpackItems(const LVL1CTP::Lvl1Result& result);

    /**
     * @brief unpacks everything that belongs to a ChainGroup
     **/
    void updateChainGroup(Trig::ChainGroup* chainGroup);

    /**
     * @brief access to navigation
     **/
    HLT::NavigationCore* navigation_feature() const { return m_navigation; }

    //
    // Data members
    //

    // Navigation owned by CGM
    HLT::NavigationCore* m_navigation;
    
    // chain groups 
    typedef std::map< std::vector< std::string >, Trig::ChainGroup* >::iterator ChGrIt;
    std::map< std::vector< std::string >, Trig::ChainGroup* > m_chainGroups;     //!< primary storage for chain groups
    std::map< std::vector< std::string >, Trig::ChainGroup* > m_chainGroupsRef;  //!< this map keeps the chain group more than once i.e. when alias is given

    //    std::map<CTPID, const LVL1CTP::Lvl1Item*>          m_items;    //!< items keyed by id (changing every event)
    //    std::map<CHAIN_COUNTER, const HLT::Chain*>         m_l2chains; //!< chains keyed by chain counter (chainging every event)
    //    std::map<CHAIN_COUNTER, const HLT::Chain*>         m_efchains;
    
    std::map<std::string, const LVL1CTP::Lvl1Item*>  m_itemsByName;     //!< items keyed by configuration name (chainging every event)
    std::map<std::string, const HLT::Chain*>         m_l2chainsByName;  //!< L2 chains keyed by chain name (chainging every event)
    std::map<std::string, const HLT::Chain*>         m_efchainsByName;  //!< L2 chains keyed by chain name (chainging every event)
    
    typedef unsigned CTPID;
    typedef unsigned CHAIN_COUNTER;
    std::map<CTPID, LVL1CTP::Lvl1Item*>  m_itemsCache;               //!< cache of all CTP items possible (given configuration)
    std::map<CHAIN_COUNTER, HLT::Chain*> m_l2chainsCache;            //!< cache of all L2 chains possible (given configuration)
    std::map<CHAIN_COUNTER, HLT::Chain*> m_efchainsCache;            //!< cache of all EF chains possible (given configuration)

    std::map<std::string, std::vector<std::string> > m_groups;          //!< mapping from group to list of chains
    std::map<std::string, std::vector<std::string> > m_streams;         //!< mapping from stream to list of chains
    
    const TrigConf::ItemContainer* m_confItems;             //!< items configuration
    const TrigConf::HLTChainList*  m_confChains;            //!< all chains configuration
  

    struct eqstr
    {
      bool operator()(const char* s1, const char* s2) const
      {
        return strcmp(s1, s2) == 0;
      }
    };

    typedef __gnu_cxx::hash_map<const char*, const TrigConf::HLTChain*, __gnu_cxx::hash<const char*>, eqstr> ChainHashMap_t;
    ChainHashMap_t     m_mConfChains;            //!< map of conf chains
  
    char     m_bgCode; //!< the encoded bunchgroup information
    
    
    class AnyTypeDeleter {

      
      struct iholder {
        virtual ~iholder() {}
        virtual void* ptr() const { return 0;}
      };

      struct holder_comp {
        bool operator()(iholder* lhs, iholder* rhs) const { 
          return lhs->ptr() < rhs->ptr(); }      
      };
      template<class T>
      class holder : public iholder {    
      public:
        holder(T& t): m_held(t) {}
        virtual ~holder() { 
          delete m_held;
          m_held = 0; }    
        virtual void* ptr() const { return (void*)m_held;}
      private:
        T m_held;
      };

    public:
      template<class T>
      void insert(T t) {
        m_todel.insert(new holder<T>(t));
      }
      void clear() {
        BOOST_FOREACH(iholder* i, m_todel) {
          delete i;
        }    
        m_todel.clear();
      }
      ~AnyTypeDeleter();
    private:  
      std::set< iholder*, holder_comp > m_todel;
    };  // end of deleter
    
    mutable AnyTypeDeleter m_deleteAtEndOfEvent;



  };
} // End of namespace

#endif
