/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_ITEMMAP_H
#define TRIGT1CTP_ITEMMAP_H

// local include(s):
#include "./SpecialTrigger.h"

// STL include(s):
#include <set>
#include <map>

#include "TrigConfData/L1Menu.h"

#include "TrigConfL1Data/Menu.h"
#include "./CTPTriggerItem.h"



// Forward include(s):
namespace TrigConf {
  class PrescaleSet;
}

namespace LVL1CTP {

   // forward includes in namespace LVL1CTP
   class ThresholdMap;

   /**
    *   @short Map associating item name to CTPTriggerItem objects
    */

   class ItemMap {

   public:

      ItemMap( const TrigConf::L1Menu * l1menu );

      //! constructor setting list of trigger items and associated prescales (random prescale offset if randEngine is given)
      ItemMap( const TrigConf::ItemContainer& item_vector,
               const TrigConf::PrescaleSet& prescales );

      //! default destructor
      ~ItemMap();

      const CTPTriggerItem* getItem( const std::string & itemName ) const
      { return m_map.find( itemName )->second; }

      const std::set<std::string> & itemNames() const
      { return m_itemNames; }


   private:

      // turns item definition "1&2&3" into "EM[x2]&BGRP0&BGRP1", which is needed when item configuration is read from COOL
      std::string getDefinition( const TrigConf::TriggerItem * item ) const;

      /// map from item name to CTPTriggerItem
      std::map< std::string, const CTPTriggerItem* > m_map;

      std::set<std::string> m_itemNames{};

   };

}
#endif
