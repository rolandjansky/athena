/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <map>
#include "GaudiKernel/ITHistSvc.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigNavigation/Navigation.h"

#include "TrigMonitorBase/TrigLockedHist.h"
#include "TrigSteering/Lvl1ConsistencyChecker.h"

#include "TrigT1Result/RoIBResult.h"

#include "boost/foreach.hpp"

#include <TH1I.h>

Lvl1ConsistencyChecker::Lvl1ConsistencyChecker(const std::string& name, const std::string& type,
		       const IInterface* parent) 
  : AthAlgTool(name, type, parent),
    m_histogram(0)
{
  declareProperty("printErrorMessages", m_printErrorMessages=true, "print detailed error reports");
  declareProperty("returnFailure", m_returnFailure=false, "in case of inconsitency return FAILURE");
  declareProperty("maxTOBs", m_returnFailure=false, "in case of inconsitency return FAILURE");

  declareInterface<Lvl1ConsistencyChecker>( this );
} 


StatusCode Lvl1ConsistencyChecker::initialize() {
  return StatusCode::SUCCESS;
}


StatusCode Lvl1ConsistencyChecker::start() {
   // make monitoring histogram 
   // count number of thresholds used 

   // create set of threshold names
   std::set<std::string> threshold_names;
   for ( const auto & item : m_configuredItems ) {
      for ( const auto & thr : item.second.thresholds ) {
         threshold_names.insert(thr.second.name);
      }
   }
  
   // now book the histogram
   m_histogram = new TH1I("ThresholdItemMisses", 
                          "Thresholds missing for items",
                          threshold_names.size(), 0, threshold_names.size());

   // name x axis
   uint bin = 1;
   for ( const std::string & thrName : threshold_names ) {
      m_histogram->GetXaxis()->SetBinLabel(bin++, thrName.c_str());
   }


   ITHistSvc *histSvc;
   if (service("THistSvc", histSvc).isFailure()) {
      ATH_MSG_WARNING("Could not get the THistSvc");
      return StatusCode::RECOVERABLE;
   }

   std::string bookname = "/EXPERT/"+name()+"/"+m_histogram->GetName();
   replace(bookname.begin(), bookname.end(), '.', '/');

   if ( histSvc->regHist( bookname, m_histogram).isFailure() ) {
      ATH_MSG_WARNING("Could not register histogram in the THistSvc");
      return StatusCode::RECOVERABLE;
   }
   ATH_MSG_DEBUG("Booked monitoring histogram");

   return StatusCode::SUCCESS;
}

StatusCode Lvl1ConsistencyChecker::updateConfig(const TrigConf::ILVL1ConfigSvc* config) {
   m_configuredItems.clear();

   const TrigConf::ItemContainer& lvl1Items = config->ctpConfig()->menu().items();
  
   for( const TrigConf::TriggerItem * confItem : lvl1Items ) {
      Item item;
      item.name = confItem->name();
      makeItemRoIMap( confItem->topNode(), item);
      m_configuredItems[TrigConf::HLTUtils::string2hash(item.name)] = item;
   }

   if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("Configured for checking items & thresholds");
      for ( const auto & item : m_configuredItems ) {
         ATH_MSG_DEBUG("Item: " << item.second.name << " " << item.first); 
         for ( const auto & thr : item.second.thresholds ) {
            ATH_MSG_DEBUG("Threshold: " << thr.second.name << " multiplicity: " << unsigned(thr.second.multiplicity) << " " << thr.first); 
         }
      }
   }
   
   return StatusCode::SUCCESS;
}


HLT::ErrorCode
Lvl1ConsistencyChecker::check(const std::vector<const LVL1CTP::Lvl1Item*>& items,
                              const HLT::Navigation* nav) {
   unsigned error_count(0);
   ATH_MSG_DEBUG("Executing check");

   std::vector<const LVL1CTP::Lvl1Item*>::const_iterator i;

   for ( const LVL1CTP::Lvl1Item* item : items ){
      if ( ! item->isPassedAfterVeto() ) // we do not need to check items which did not pass
         continue;

      uint32_t id = item->hashId();
      items_iterator cIt = m_configuredItems.find(id);
      if ( cIt == m_configuredItems.end() ) {
         ATH_MSG_WARNING("Item which is passed to the checker was not known from configuration, id: " << item->hashId() << " " << item->name());
         continue;
      }

      // now check if all thresholds (TEs) are present
      for ( const auto & thr : cIt->second.thresholds ) {
         
         unsigned found = nav->countAllOfType(thr.first, false);

         if ( found < thr.second.multiplicity ) { // here we have probem
            error_count++;
            lock_histogram_operation<TH1I>(m_histogram)->Fill(thr.second.name.c_str(), 1.);
            if (m_printErrorMessages ) {
              ATH_MSG_WARNING("Item " << cIt->second.name << " required: " 
                              << unsigned(thr.second.multiplicity) << " of: " << thr.second.name << " while got: " << found);
            }
         }
      }
   }

   if ( error_count && m_returnFailure) {
      return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::MISSING_FEATURE, 
                            HLT::SteeringInternalReason::NO_LVL1_ITEMS );
   }
   return HLT::OK;
}





//Fills in a map recursively counting how many RoIs a trigger item requires.
//Note that it only can deal with AND'd thresholds, not OR'd or NOT for RoI items such as in forward jet items
//items not used in the L2 will also not be checked correctly as the TE used for the lookup is 0 for all
void Lvl1ConsistencyChecker::makeItemRoIMap(const TrigConf::TriggerItemNode *node, Lvl1ConsistencyChecker::Item& item) {
   if (!node) return;
   if (node->isThreshold() && node->triggerThreshold()) {
      const std::string& tName = node->triggerThreshold()->name();
      if ( thresholdToCheck(tName) ) {
         unsigned int multiplicity=node->multiplicity();
         Threshold t;
         t.name = tName;
         t.multiplicity = multiplicity;    
         item.addThreshold(t); 
      }
      //    ATH_MSG_DEBUG(" found threshold: " << tName <<" with multiplicity:" << multiplicity 
      //	     << " for item:" << item.name);

   }
   if (node->type()==TrigConf::TriggerItemNode::AND) {
      for(TrigConf::TriggerItemNode* child : node->children()) {
         makeItemRoIMap(child,item); 
      }
   }
}

void Lvl1ConsistencyChecker::Item::addThreshold(const Threshold& t) {


  uint32_t id = TrigConf::HLTUtils::string2hash(t.name);
  thresholds_iterator ti = thresholds.find(id);
  if ( ti != thresholds.end() )
    thresholds[id].multiplicity += t.multiplicity;
  else
    thresholds[id] = t;
}


bool Lvl1ConsistencyChecker::thresholdToCheck(const std::string& name) {
  if (  name.find("MU")    == 0
	|| name.find("EM") == 0
	|| name.find("HA") == 0
	|| name.find("J")  == 0 
	|| name.find("TE") == 0 
	|| name.find("XE") == 0 
	|| name.find("XS") == 0  ) {
    return true;
  }
  return false;
}

