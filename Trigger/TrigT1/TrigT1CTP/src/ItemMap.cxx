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

#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

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


LVL1CTP::ItemMap::~ItemMap() {
   for( auto entry : m_map ) {
      delete entry.second;
   }
}


LVL1CTP::ItemMap::ItemMap( const TrigConf::ItemContainer& item_vector, 
                           const TrigConf::PrescaleSet& prescales )
{
   for( const TrigConf::TriggerItem * item : item_vector ) {

      std::string definition = getDefinition(item);

      const std::string & itemName( item->name() );

      // get postion from ctp id
      unsigned int ctpId = item->ctpId();

      CTPTriggerItem * ctpItem = new CTPTriggerItem();
      ctpItem->setPrescale( prescales.prescales_float()[ ctpId ] );
      ctpItem->setCtpId( ctpId );
      ctpItem->setName(itemName);
      ctpItem->setLogic(definition);
      ctpItem->setTriggerType(item->triggerType());

      m_map[ itemName ] = ctpItem;
      m_itemNames.insert( itemName );
   }
}


/*
  e.g for the item L1_EM15VHI_2TAU12IM_J25_3J12
  the definition that is stored in COOL is  (1&2&3&4&5&6)
  and the list of thresholds is 1,EM15VHI_x1,EM15VHI  2,HA12IM_x2,HA12IM  1,J25_x1,J25  3,J12_x3,J12  BGRP0  BGRP1
  
  In this function this information is used to create a definition string 
  "(EM15VHI[x1]&HA12IM[x2]&J25[x1]&J12[x3]&BGRP0&BGRP1)"
*/ 
std::string
LVL1CTP::ItemMap::getDefinition( const TrigConf::TriggerItem * item ) const {


   std::vector<std::string> thresholdlist;
   std::string logic, conditions;
   item->buildLogic(logic, thresholdlist); // get's the list of thresholds, e.g. 1,EM15VHI_x1,EM15VHI  2,HA12IM_x2,HA12IM  1,J25_x1,J25  3,J12_x3,J12  BGRP0  BGRP1

   std::map<unsigned int, std::string> thrNames;
   size_t idx = 1;
   for(const std::string & thrmult : thresholdlist) {
      std::vector<std::string> res;
      boost::split(res, thrmult, boost::is_any_of(",")); // split 1,EM15VHI_x1,EM15VHI into multiplicity,name_mult, and name

      std::string name_mult("");
      if(res.size() == 1) {
         name_mult=res[0]; // for thresholds without multiplicity requirement like RNDM0 or BGRP0,...
      } else {
         name_mult = res[2]+"[x"+res[0]+"]"; // build "EM15VHI[x1]"
      }
      thrNames[idx++] = name_mult;
   }

   // now take the logic (1&2&3&4&5&6) apart and replace the numbers with names "thr[xmult]"
   std::string def = item->definition();
   std::vector<std::string> tokens;
   // build tokens with separators ()&|! and <space>. Keeps all separators except <space> in the list of tokens
   for ( auto & tok : boost::tokenizer<boost::char_separator<char> > (item->definition(), boost::char_separator<char>(" ", "()&|!")) ) {
      try {
         int n = boost::lexical_cast<int,std::string>(tok);
         tokens.emplace_back(thrNames[n]);
      }
      catch(const boost::bad_lexical_cast &) {
         tokens.emplace_back(tok);
      }
   }

   // and reassemble the logic definition string
   std::string definition("");
   for( auto & tok : tokens ) {
      definition += tok;
   }
      
   return definition;
}
