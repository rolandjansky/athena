/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local includes
#include "./ItemMap.h"

// TrigT1 configuration includes
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/PrescaleSet.h"


// random number generator
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

LVL1CTP::ItemMap::ItemMap( const TrigConf::L1Menu * l1menu )
{
   for( const TrigConf::L1Item & item : *l1menu ) {
      const std::string & itemName( item.name() );

      // get postion from ctp id
      unsigned int ctpId = item.ctpId();

      CTPTriggerItem * ctpItem = new CTPTriggerItem();
      ctpItem->setPrescale( 1 );
      ctpItem->setCtpId( ctpId );
      ctpItem->setName(itemName);
      ctpItem->setLogic(item.definition());
      ctpItem->setTriggerType(item.triggerTypeAsUChar());
      ctpItem->setBunchGroups(item.bunchgroups());

      m_map[ itemName ] = ctpItem;
      m_itemNames.insert( itemName );
      
   }
}


LVL1CTP::ItemMap::ItemMap( const TrigConf::ItemContainer& item_vector, 
                           const TrigConf::PrescaleSet& prescales )
{
   for( const TrigConf::TriggerItem * item : item_vector ) {
      const std::string & itemName( item->name() );

      // get postion from ctp id
      unsigned int ctpId = item->ctpId();

      CTPTriggerItem * ctpItem = new CTPTriggerItem();
      ctpItem->setPrescale( prescales.prescales_float()[ ctpId ] );
      ctpItem->setCtpId( ctpId );
      ctpItem->setName(itemName);
      ctpItem->setLogic(item->definition());
      ctpItem->setTriggerType(item->triggerType());

      m_map[ itemName ] = ctpItem;
      m_itemNames.insert( itemName );
   }
}

LVL1CTP::ItemMap::~ItemMap() {
   for( auto entry : m_map ) {
      delete entry.second;
   }
}

