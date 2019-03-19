/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_Menu
#define TrigConf_Menu

#include "TrigConfL1Data/L1DataBaseclass.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TIP.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/random_access_index.hpp>

#include <iosfwd>
#include <vector>
#include <string>


namespace TrigConf {

   // trigger items multi index container
   struct tag_ctpid{};
   struct tag_name_hash{};

   typedef boost::multi_index::multi_index_container<TriggerItem*, boost::multi_index::indexed_by<
      boost::multi_index::random_access<>,
      boost::multi_index::ordered_unique< boost::multi_index::identity<TriggerItem> >,
      boost::multi_index::ordered_unique< boost::multi_index::tag<tag_ctpid>, boost::multi_index::const_mem_fun<TriggerItem, int, &TriggerItem::ctpId> >,
      boost::multi_index::hashed_unique < boost::multi_index::tag<tag_name_hash>, boost::multi_index::const_mem_fun<TrigConfData, const std::string&, &TrigConfData::name> >
      >
   > ItemContainer;
   
   typedef ItemContainer::index<tag_ctpid>::type      item_by_ctpid_t;
   typedef item_by_ctpid_t::iterator                  item_by_ctpid_iterator;
   
   typedef ItemContainer::index<tag_name_hash>::type               item_by_name_t;
   typedef ItemContainer::index<tag_name_hash>::type::iterator     item_by_name_iterator;
   
   class DiffStruct;

   class Menu  : public L1DataBaseclass {
   public:
      Menu();
      ~Menu() = default;

      item_by_ctpid_t&           item_by_ctpid();
      const item_by_ctpid_t&     item_by_ctpid() const;

      item_by_name_t&            item_by_name();
      const item_by_name_t&      item_by_name() const;

      // setter and getters
      void addTriggerItem(TriggerItem* ti);
      const ItemContainer& itemVector() const;
      const ItemContainer& items() const;
      TriggerItem* findTriggerItem(int ctpid) const { return item(ctpid); }
      TriggerItem* item(int ctpid) const;
      TriggerItem* item(const std::string& name) const;


      const std::vector<ThresholdMonitor*>& moncountVector() const { return m_ThresholdMonitorVector; }
      const std::vector<PIT*>&              pitVector() const { return m_PITs; }
      const std::vector<TIP*>&              tipVector() const { return m_TIPs; }
      const std::map<unsigned int, std::string>& lutOutputNames() const { return m_LUT; }
      const ThresholdConfig& thresholdConfig() const { return m_ThresholdConfig; }
      const CaloInfo& caloInfo() const { return m_ThresholdConfig.caloInfo(); }

      ThresholdConfig& thresholdConfig() { return m_ThresholdConfig; }

      std::vector<uint16_t> bunchgroupMask() const;

      // setters
      void setThresholdConfig( const TrigConf::ThresholdConfig& tc ) { m_ThresholdConfig = tc; }
      void setCaloInfo( const CaloInfo& ci ) { m_ThresholdConfig.setCaloInfo(ci); }

      // another interface to access the TriggerThreshold vector
      const std::vector<TriggerThreshold*>& thresholdVector() const;
      void addTriggerThreshold(TriggerThreshold* tt);

      void addThresholdMonitor(ThresholdMonitor* thrm);
      ThresholdMonitor* findThresholdMonitor(unsigned int id);

      void addLutOutputName(unsigned int, const std::string &);
      void setLutOutputNames(const std::map<unsigned int, std::string>& lut) { m_LUT = lut; }
      
      void addPit(PIT* pit);
      PIT* findPIT(unsigned int id);
      
      void addTip(TIP* tip);
      TIP* findTIP(unsigned int id);

      int size() const;

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      void writeXMLItems(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
      void writeXMLThresholds(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
      void writeXMLMonCounters(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

      DiffStruct* compareTo(const Menu* o) const;

      void clear();

      std::vector<TrigConf::TriggerItem*> itemsV() const;

   private:
      ThresholdConfig                m_ThresholdConfig;
#ifndef __COVERITY__
      ItemContainer                  m_TriggerItemVector;
#endif
      std::vector<ThresholdMonitor*> m_ThresholdMonitorVector;
      std::vector<PIT*>              m_PITs;
      std::vector<TIP*>              m_TIPs;
      std::map<unsigned int, std::string> m_LUT;  // contains the conditions out of each LUT output

   };


#ifndef __COVERITY__
   inline
   item_by_ctpid_t&           Menu::item_by_ctpid() { return m_TriggerItemVector.get<tag_ctpid>(); }
   inline
   const item_by_ctpid_t&     Menu::item_by_ctpid() const { return m_TriggerItemVector.get<tag_ctpid>(); }

   inline
   item_by_name_t&            Menu::item_by_name() { return m_TriggerItemVector.get<tag_name_hash>(); }
   inline
   const item_by_name_t&      Menu::item_by_name() const { return m_TriggerItemVector.get<tag_name_hash>(); }

   inline
   const ItemContainer& Menu::itemVector() const { return m_TriggerItemVector; }
   inline
   const ItemContainer& Menu::items() const { return m_TriggerItemVector; }

   inline
   int Menu::size() const { return m_TriggerItemVector.size(); }
#endif

}

#endif
