/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <sstream>
#include <iomanip>

#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1CTP/CTPTriggerItemNode.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/CTPTriggerThreshold.h"
#include "TrigT1CTP/MsgLogger.h"

#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"

using namespace std;

bool LVL1CTP::CTPTriggerItemNode::evaluate( const TrigConf::TriggerItemNode* node, const ThresholdMap* map, const InternalTriggerMap* internalTrigger ) {

   MsgLogger logger( "CTPTriggerItemNode" );

   if( node->type() == TrigConf::TriggerItemNode::OBJ ) {

      if ( node->isThreshold() ) {
         const bool decision = map->decision( node->triggerThreshold() )->value() >= node->multiplicity();
         logger.send( MSG::VERBOSE, string(" Decision for OBJ TriggerItemNode with threshold: ")
                      + node->triggerThreshold()->name() + " is " + ( decision ? "ACTIVE" : "INACTIVE" ) );
         return decision;
      }

      if (node->internalTriggerType() == TrigConf::L1DataDef::UNDEF) {
         logger.send( MSG::WARNING, "Undefined internal trigger! Returning false." );
         return false;
      }

      bool decision = false;
      InternalTriggerMap::const_iterator iter = internalTrigger->find( make_pair(node->internalTriggerType(),node->internalTriggerNumber()) );
      if (iter != internalTrigger->end())
         decision = iter->second->evaluate();
      ostringstream message;
      message << " Decision for OBJ TriggerItemNode with internal trigger: " 
              << setw( 8 ) << TrigConf::L1DataDef::typeAsString(node->internalTriggerType()) << node->internalTriggerNumber()
              << " is " << ( decision ? "ACTIVE" : "INACTIVE" );
      logger.send( MSG::VERBOSE, message.str() );
      return decision;
   }


   const vector<TrigConf::TriggerItemNode*>& children = node->children();

   if( node->type() == TrigConf::TriggerItemNode::NOT ) {
      if(children.size() != 1) {
         logger.send( MSG::WARNING, "TriggerItemNode NOT has " + boost::lexical_cast<string,size_t>(children.size())
                      + "! Returning false." );
         return false;
      }
      bool decision = ! evaluate( children[0], map, internalTrigger );
      logger.send( MSG::VERBOSE, string("  Decision for NOT TriggerItemNode is ") + ( decision ? "ACTIVE" : "INACTIVE" ) );
      return decision;
   }


   if(children.size() == 0) {
      logger.send( MSG::WARNING, "AND or OR node without children! Returning false." );
      return false;
   }

   if( node->type() == TrigConf::TriggerItemNode::AND ) {
      bool decision = true;
      BOOST_FOREACH(TrigConf::TriggerItemNode* child, children) {
         decision &= evaluate( child, map, internalTrigger );
         if(!decision) break;
      }
      logger.send( MSG::VERBOSE, string("  Decision for AND TriggerItemNode is ") + ( decision ? "ACTIVE" : "INACTIVE" ) );
      return decision;
   }


   if ( node->type() == TrigConf::TriggerItemNode::OR ) {
      bool decision = false;
      BOOST_FOREACH(TrigConf::TriggerItemNode* child, children) {
         decision |= evaluate( child, map, internalTrigger );
         if(decision) break;
      }
      logger.send( MSG::VERBOSE, string("  Decision for OR TriggerItemNode is ") + ( decision ? "ACTIVE" : "INACTIVE" ) );
      return decision;
   }

   logger.send( MSG::FATAL, "TrigConf::TriggerItemNode::type() not recognized!!!" );
   assert( 0 );
   return false;

}
