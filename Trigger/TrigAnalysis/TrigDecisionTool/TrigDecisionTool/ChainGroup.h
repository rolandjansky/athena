/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_CHAIN_GROUP_H
#define TRIGGER_DECISION_TOOL_CHAIN_GROUP_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : ChainGroup
 *
 * @brief container to hold trigger chains
 *
 * @author Michael Begel  <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Joerg Stelzer  <Joerg.Stelzer@cern.ch>  - DESY
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>   - UC Irvine - AGH-UST Krakow
 *
 ***********************************************************************************/

#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <stack>
#include <boost/algorithm/string.hpp>

#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigSteeringEvent/Enums.h"


namespace HLT {
  class Chain;
}

namespace LVL1CTP {
  class Lvl1Item;
}

namespace TrigConf {
  class TriggerItem;
  class HLTChain;
  class HLTTriggerElement;
}


namespace Trig {

  //class CacheGlobalMemory;

   class ChainGroup : public virtual Logger {

     friend class CacheGlobalMemory;

   public:

      typedef std::vector<std::string>::const_iterator const_iterator;
      const Trig::ChainGroup& operator+(const Trig::ChainGroup& rhs);
      bool operator==(const Trig::ChainGroup& rhs);
      bool operator!=(const Trig::ChainGroup& rhs);

      /**
       * @brief adds alias (sort understandabel name) to the group
       **/
      void addAlias(const std::string& alias);


      /**
       * @brief tells if chain group passed
       * @param conditions is modifying the question
       **/
      bool isPassed(unsigned int condition=TrigDefs::Physics) const;
      

      /**
       * @brief returns prescale factor
       * for chain group with single chain in returns real prescale factor
       * for real chain group composed of many prescaled chains returns 1 if at least one chain is unprescaled
       **/
      float getPrescale(unsigned int condition=TrigDefs::Physics) const;


      std::vector< std::string > getListOfTriggers() const;
      std::vector< std::string > getListOfStreams() const;
      std::vector< std::string > getListOfGroups() const;
      std::vector< std::string > getListOfThresholds() const;
      std::vector< std::string > getListOfSignatures() const;
      std::vector< std::vector< std::string > > getListOfTriggerElements() const;
      std::vector< std::vector< TrigConf::HLTTriggerElement* > > getHLTTriggerElements() const;

      
      /**
       * @brief returns bits (OR ed) of the chain group
       * Meaning of the returned bits can be understood by using masks defined in TrigDefs
       **/
      unsigned int isPassedBits() const;


      /**
       * @brief returns most severe error in the chains composing that chain group
       * for L1 it is just OK
       * If there is suspicion that there are other problems in the CG one needs to loop over chains and 
       * check each of them.
       **/
      HLT::ErrorCode error() const;


      /**
       * @brief returns all features related to given chain group of HLT chains or L1 items
       * Note: This does not yet work for L1_FJ..., i.e. no features are returned for these items.
       **/
      const FeatureContainer features(unsigned int condition = TrigDefs::Physics) const;
      
      // 
      const std::vector< std::string >& patterns() const {return m_patterns;}
   private:

      // constructor
      ChainGroup(const std::vector< std::string >& triggerNames,
                 const Trig::CacheGlobalMemory& parent);
      // 
      ~ChainGroup();

      bool  isCorrelatedL1items(const std::string& item) const;
      float correlatedL1Prescale(const std::string& item) const;
      float calculatePrescale(unsigned int condition=TrigDefs::Physics);

      /*
      bool getTEs(const TrigConf::HLTChain* chain, std::vector< std::vector< HLT::TriggerElement*> >& tes, 
                  unsigned int condition = TrigDefs::Physics) const;
      
      bool collectConfigurationTEs(const TrigConf::HLTChain* conf, bool onlyFinals, 
                                   std::vector< std::vector<TrigConf::HLTTriggerElement*> >& tes) const;
      
      bool collectNavigationTEs(unsigned int TEid, bool activeOnly, 
                                const std::vector<unsigned int>& lastIds,
                                std::vector<HLT::TriggerElement*>& tes) const;
      */

      //const FeatureContainer features(const TrigConf::HLTChain* chain, unsigned int condition) const;
      void appendFeatures(std::vector< std::vector< HLT::TriggerElement*> >& tes, FeatureContainer& fc) const;

      /**
       * @brief names of triggers within chain group
       **/
      const std::vector< std::string >& names() const {return m_names;}

      bool HLTResult(const std::string& chain, unsigned int condition) const;
      bool L1Result(const std::string& item, unsigned int condition) const;

      unsigned int HLTBits(const std::string& chain, const std::string& level) const;
      unsigned int L1Bits(const std::string& item) const;

      float HLTPrescale(const std::string& chain, unsigned int condition) const;
      float L1Prescale(const std::string& item, unsigned int condition) const;

      std::string getLowerName(const std::string& EFname) const;


      // for friends to run over the configuration chains 
      typedef std::set<const TrigConf::TriggerItem*>::const_iterator const_conf_item_iterator;
      typedef std::set<const TrigConf::HLTChain*>::const_iterator const_conf_chain_iterator;
      const_conf_chain_iterator conf_chain_begin()   const { return m_confChains.begin(); }
      const_conf_chain_iterator conf_chain_end()     const { return m_confChains.end(); }

      const_conf_item_iterator conf_item_begin() const { return m_confItems.begin(); }
      const_conf_item_iterator conf_item_end()   const { return m_confItems.end(); }

      std::vector<std::string> m_patterns;  //!< patterns with which the CG was constructed
    
      std::set<const TrigConf::HLTChain*>           m_confChains;
      std::set<const TrigConf::TriggerItem*>        m_confItems;

#ifndef __REFLEX__
      // quick cache (external therefore reference) of the result per event
      const Trig::CacheGlobalMemory&                       m_cgm;
#endif
      std::vector< std::string > m_names; //!< names of trigger derived from patterns & current configuration                 

      const Trig::CacheGlobalMemory* cgm(bool onlyConfig=false) const;

      // update the configuration
      void update(const TrigConf::HLTChainList* confChains,
                  const TrigConf::ItemContainer* confItems);

      ChainGroup& operator= (const ChainGroup&);

      float m_prescale;

   };
} // End of namespace

#endif
