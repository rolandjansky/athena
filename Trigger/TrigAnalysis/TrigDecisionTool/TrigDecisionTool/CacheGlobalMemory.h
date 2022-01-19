/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_CACHE_GLOBAL_MEMORY_H
#define TRIGGER_DECISION_TOOL_CACHE_GLOBAL_MEMORY_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @class  : CacheGlobalMemory
 *
 * @brief registry for ChainGroups
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/

#include<vector>
#include<set>
#include<map>
#include<unordered_map>
#include<string>
#include<mutex>
#include<memory>

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTStreamTag.h"

#include "TrigConfL1Data/CTPConfig.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/Lvl1Item.h"

#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/EventStoreType.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigNavigation.h"

#ifndef XAOD_ANALYSIS // Full Athena only
#include "EventInfo/EventInfo.h"
#include "TrigDecisionEvent/TrigDecision.h"
#endif

namespace HLT {
  class Chain;
  class TriggerElement;
}

namespace LVL1CTP {
  class Lvl1Result;
}


namespace Trig {

  class ChainGroup;

  class CacheGlobalMemory : public virtual Logger {

  public:
    // constructors, destructor
    CacheGlobalMemory() = default;
    ~CacheGlobalMemory() = default;

    /**
     * @brief creates new chain group
     * @param patterns list of patterns i.e. regexes, if chain groups already exists it is just given back
     *        The chain group is considered to exist if the patterns are the same.
     *        Patterns are stored and made unique i.e. "EF_mu4", "EF_mu6" is the same as "EF_mu6", "EF_mu4".
     *        It is not the same though as "EF_mu.*" even if in particular case that would mean the same 2 chains.
     *
     * @param alias is the short human readable name for the triggers which are in the group i.e. myMuons
     * @param parseAsRegex Sets if regular expression are allowed in patterns. Otherwise exact matches will be required.
     *        This is considerably faster, so is used for some internally created chains groups with a large number of patterns.
     **/
    const Trig::ChainGroup* createChainGroup(const std::vector< std::string >& patterns,
                                             const std::string& alias="",
                                             const bool parseAsRegex = true);
    /**
     * @brief Updates configuration of the chain groups
     * (i.e. regexes are reapplied to new set of chains)
     **/
    void update(const TrigConf::HLTChainList* confChains,
                const TrigConf::CTPConfig* ctp);

    const LVL1CTP::Lvl1Item* item(const std::string& name) const;            //!< CTP item from given name
    const LVL1CTP::Lvl1Item* item(const TrigConf::TriggerItem& i) const;     //!< CTP item from given config item
    const TrigConf::TriggerItem* config_item(const std::string& name) const; //!< CTP config item from given name
    float item_prescale(int ctpid) const;                                      //!< Prescale for CPT item

    const HLT::Chain* chain(const std::string& name) const;                  //!< HLT chain object from given name (0 pointer returned if no match)
    const HLT::Chain* chain(const TrigConf::HLTChain& chIt) const;           //!< HLT chain object from given config chain
    const TrigConf::HLTChain* config_chain(const std::string& name) const;   //!< HLT config chain from given name

    const HLT::TrigNavStructure* navigation() const {   //!< gives back pointer to navigation object (unpacking if necessary)
      if(!m_navigationUnpacked){
        if(const_cast<CacheGlobalMemory*>(this)->unpackNavigation().isFailure()){
          ATH_MSG_WARNING("unpack Navigation failed");
        }
      }
      return m_navigation;
    }
    void navigation(HLT::TrigNavStructure* nav) { m_navigation = nav; }       //!< sets navigation object pointer

    std::map< std::vector< std::string >, Trig::ChainGroup* >& getChainGroups() {return m_chainGroupsRef;};
    //    std::map<unsigned, const LVL1CTP::Lvl1Item*>  getItems() {return m_items;};
    //    std::map<unsigned, const LVL1CTP::Lvl1Item*>  getItems() const {return m_items;};
    //    std::map<unsigned, const HLT::Chain*>         getL2chains() {return m_l2chains;};
    //    std::map<unsigned, const HLT::Chain*>         getL2chains() const {return m_l2chains;};
    //    std::map<unsigned, const HLT::Chain*>         getEFchains() {return m_efchains;};
    //    std::map<unsigned, const HLT::Chain*>         getEFchains() const {return m_efchains;};
    std::map<std::string, std::vector<std::string> > getStreams() {return m_streams;};
    std::map<std::string, std::vector<std::string> > getStreams() const {return m_streams;};

    const xAOD::TrigCompositeContainer* expressStreamContainer() const;

    /**
     * @brief checks if new event arrived with the decision
     * Need to use before any call to CacheGlobalMemory.
     * @return true if all went fine about decision, false otherwise
     **/
    bool assert_decision();

    /**
     * @brief invalidate previously unpacked decision
     * Needs to be called at the start of a new event.
     */
    void reset_decision();

    /// Set the event store to be used by the object
    void setStore( asg::EventStoreType* store ) { m_store = store; }
    /// Get the event store that the object is using
    const asg::EventStoreType* store() const { return m_store; }

    void setDecisionKeyPtr(SG::ReadHandleKey<xAOD::TrigDecision>* k) { m_decisionKeyPtr = k; }
    void setRun2NavigationKeyPtr(SG::ReadHandleKey<xAOD::TrigNavigation>* k) { m_run2NavigationKeyPtr = k; }
    void setRun3NavigationKeyPtr(SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>* k) { m_run3NavigationKeyPtr = k; }
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>& getRun3NavigationKeyPtr() { return *m_run3NavigationKeyPtr; }

#ifndef XAOD_ANALYSIS // Full Athena only
    void setOldDecisionKeyPtr(SG::ReadHandleKey<TrigDec::TrigDecision>* k) { m_oldDecisionKeyPtr = k; }
    void setOldEventInfoKeyPtr(SG::ReadHandleKey<EventInfo>* k) { m_oldEventInfoKeyPtr = k; }
#endif

    SG::ReadHandleKey<xAOD::TrigDecision>* xAODTrigDecisionKey() { return m_decisionKeyPtr; }

    //
    template<class T>
    void deleteAtTheEndOfEvent(T t) const { m_deleteAtEndOfEvent.insert(t); }




  private:
    friend class DecisionAccess;

    /**
     * @brief unpacks whole trigger decision for the event
     */
    StatusCode unpackDecision(const EventContext& ctx);
    /**
     * @brief unpacks HLT navigation structure (object access)
     */
    StatusCode unpackNavigation();

    /**
     * @brief unpacks everything that belongs to a ChainGroup
     **/
    void updateChainGroup(Trig::ChainGroup& chainGroup, const bool parseAsRegex = true);

    //
    // Data members
    //

    /// Pointer to the event store in use
    const asg::EventStoreType* m_store{nullptr};

    /// Trigger decision unpacker helper
    std::unique_ptr<IDecisionUnpacker> m_unpacker;

    bool m_decisionUnpacked{false};   //!< Was decision unpacked for this event?
    bool m_navigationUnpacked{false}; //!< Was navigation unpacked for this event?

    /// Navigation owned by CGM
    HLT::TrigNavStructure* m_navigation{nullptr};

    // chain groups
    std::map< std::vector< std::string >, Trig::ChainGroup > m_chainGroups;     //!< primary storage for chain groups
    std::map< std::vector< std::string >, Trig::ChainGroup* > m_chainGroupsRef; //!< this map keeps the chain group more than once i.e. when alias is given

    //    std::map<CTPID, const LVL1CTP::Lvl1Item*>          m_items;    //!< items keyed by id (changing every event)
    //    std::map<CHAIN_COUNTER, const HLT::Chain*>         m_l2chains; //!< chains keyed by chain counter (chainging every event)
    //    std::map<CHAIN_COUNTER, const HLT::Chain*>         m_efchains;

    std::unordered_map<std::string, const LVL1CTP::Lvl1Item*> m_itemsByName;     //!< items keyed by configuration name (chainging every event)
    std::unordered_map<std::string, const HLT::Chain*> m_l2chainsByName;  //!< L2 chains keyed by chain name (chainging every event)
    std::unordered_map<std::string, const HLT::Chain*> m_efchainsByName;  //!< L2 chains keyed by chain name (chainging every event)

    typedef unsigned CTPID;
    typedef unsigned CHAIN_COUNTER;
    std::map<CTPID, LVL1CTP::Lvl1Item>  m_itemsCache;               //!< cache of all CTP items possible (given configuration)
    std::map<CHAIN_COUNTER, HLT::Chain> m_l2chainsCache;            //!< cache of all L2 chains possible (given configuration)
    std::map<CHAIN_COUNTER, HLT::Chain> m_efchainsCache;            //!< cache of all EF chains possible (given configuration)

    std::map<std::string, std::vector<std::string> > m_groups;          //!< mapping from group to list of chains
    std::map<std::string, std::vector<std::string> > m_streams;         //!< mapping from stream to list of chains

    const TrigConf::ItemContainer* m_confItems{nullptr};             //!< items configuration
    const TrigConf::HLTChainList*  m_confChains{nullptr};            //!< all chains configuration
    mutable const xAOD::TrigCompositeContainer* m_expressStreamContainer{nullptr};

    SG::ReadHandleKey<xAOD::TrigDecision>* m_decisionKeyPtr{nullptr}; //!< Parent TDT's read handle key

#ifndef XAOD_ANALYSIS // Full Athena
    SG::ReadHandleKey<TrigDec::TrigDecision>* m_oldDecisionKeyPtr{nullptr}; //!< Parent TDT's read handle key
    SG::ReadHandleKey<EventInfo>* m_oldEventInfoKeyPtr{nullptr}; //!< Parent TDT's read handle key
#endif

    SG::ReadHandleKey<xAOD::TrigNavigation>* m_run2NavigationKeyPtr{nullptr}; //!< Parent TDT's read handle key
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>* m_run3NavigationKeyPtr{nullptr}; //!< Parent TDT's read handle key

    typedef std::unordered_map<std::string, const TrigConf::HLTChain*> ChainHashMap_t;
    ChainHashMap_t     m_mConfChains;            //!< map of conf chains

    char     m_bgCode{0}; //!< the encoded bunchgroup information


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
        for(iholder* i : m_todel) {
          delete i;
        }
        m_todel.clear();
      }
      ~AnyTypeDeleter();
    private:
      std::set< iholder*, holder_comp > m_todel;
    };  // end of deleter

    mutable AnyTypeDeleter m_deleteAtEndOfEvent;

    mutable std::recursive_mutex m_cgmMutex; //!< R3 MT protection only against --threads > 1. Needs refacotring...



  };
} // End of namespace

#endif
