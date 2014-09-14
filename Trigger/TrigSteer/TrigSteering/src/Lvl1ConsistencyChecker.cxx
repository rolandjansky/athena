/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"

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

#include "boost/foreach.hpp"


#include <TH1I.h>

Lvl1ConsistencyChecker::Lvl1ConsistencyChecker(const std::string& name, const std::string& type,
		       const IInterface* parent) 
  : AlgTool(name, type, parent),
    m_histogram(0),
    m_log(0) {
  declareProperty("printErrorMessages", m_printErrorMessages=true, "print detailed error reports");
  declareProperty("returnFailure", m_returnFailure=false, "in case of inconsitency return FAILURE");

  declareInterface<Lvl1ConsistencyChecker>( this );
} 

StatusCode Lvl1ConsistencyChecker::initialize() {
  m_log = new MsgStream(msgSvc(), name());
  return StatusCode::SUCCESS;
}

StatusCode Lvl1ConsistencyChecker::start() {  
  // make monitoring hsitogram 
  // count number of thresholds used 
  std::set<std::string> threshold_names;
  for ( items_iterator i = m_configuredItems.begin(); 
	i != m_configuredItems.end(); ++i ) {
    for ( thresholds_iterator t = i->second.thresholds.begin();
	  t != i->second.thresholds.end(); ++t ) {
      threshold_names.insert(t->second.name);
    }
  }
  
  // now book the histogram
  m_histogram = new TH1I("ThresholdItemMisses", 
			 "Thresholds missing for items",
			 threshold_names.size(), 0, threshold_names.size());

  // name x axis
  int bin = 1;
  for ( std::set<std::string>::const_iterator n = threshold_names.begin();
	n != threshold_names.end(); ++n ) {
    m_histogram->GetXaxis()->SetBinLabel(bin, (*n).c_str());
    bin++;
  }


  ITHistSvc *histSvc;
  if (service("THistSvc", histSvc).isFailure()) {
    (*m_log) << MSG::WARNING << "Could not get the THistSvc" << endreq;
    return StatusCode::RECOVERABLE;
  }
  std::string bookname = "/EXPERT/"+name()+"/"+m_histogram->GetName();
  replace(bookname.begin(), bookname.end(), '.', '/');

  if ( histSvc->regHist( bookname,
			 m_histogram).isFailure() ) {
    (*m_log) << MSG::WARNING << "Could not register histogram in the THistSvc" << endreq;
    return StatusCode::RECOVERABLE;
  }
  (*m_log) << MSG::DEBUG << "Booked monitoring histogram" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode Lvl1ConsistencyChecker::updateConfig(const TrigConf::ILVL1ConfigSvc* config) {
  m_configuredItems.clear();
  const TrigConf::Menu& lvl1Menu = config->ctpConfig()->menu();
  const TrigConf::ItemContainer& lvl1Items = lvl1Menu.items();
  
  for (TrigConf::ItemContainer::const_iterator itemIt = lvl1Items.begin();
       itemIt != lvl1Items.end(); ++itemIt) {
    Item item;
    item.name = (*itemIt)->name();
    makeItemRoIMap((*itemIt)->topNode(), item);
    m_configuredItems[TrigConf::HLTUtils::string2hash(item.name)] = item;
  }
  (*m_log) << MSG::DEBUG << "Configured for checking items & thresholds" << endreq;
  for ( items_iterator i = m_configuredItems.begin(); i != m_configuredItems.end(); ++i ) {
    (*m_log) << MSG::DEBUG << "Item: " << i->second.name << " " << i->first <<  endreq; 
    for ( thresholds_iterator t = i->second.thresholds.begin(); t != i->second.thresholds.end(); ++t ) {
      (*m_log) << MSG::DEBUG << "Threshold: " << t->second.name << " multiplicity: " << unsigned(t->second.multiplicity) << " " << i->first  <<  endreq; 
    }
  }


  return StatusCode::SUCCESS;
}

HLT::ErrorCode
Lvl1ConsistencyChecker::check(const std::vector<const LVL1CTP::Lvl1Item*>& items,
                              const HLT::Navigation* nav) {
   unsigned error_count(0);
   (*m_log) << MSG::DEBUG << "Executing check" << endreq;
   std::vector<const LVL1CTP::Lvl1Item*>::const_iterator i;
   for ( const LVL1CTP::Lvl1Item* item : items ){
      if ( ! item->isPassedAfterVeto() ) // we do not need to check items which did not pass
         continue;

      uint32_t id = item->hashId();
      items_iterator cIt = m_configuredItems.find(id);
      if ( cIt == m_configuredItems.end() ) {
         (*m_log) << MSG::WARNING << "Item which is passed to the checker was not known from configuration, id: " << item->hashId() << " " << item->name() << endreq;
         continue;
      }

      // now check if all thresholds (TEs) are present
      for ( thresholds_iterator t = cIt->second.thresholds.begin(); t != cIt->second.thresholds.end(); ++ t ) {
         unsigned found = nav->countAllOfType(t->first, false);
         if ( found < t->second.multiplicity ) { // here we have probem
            error_count++;
            lock_histogram_operation<TH1I>(m_histogram)->Fill(t->second.name.c_str(), 1.);
            if (m_printErrorMessages ) {
               (*m_log) << MSG::WARNING << "Item " << cIt->second.name << " required: " 
                        << unsigned(t->second.multiplicity) << " of: " << t->second.name << " while got: " << found << endreq;
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
    //    (*m_log) << MSG::DEBUG << " found threshold: " << tName <<" with multiplicity:" << multiplicity 
    //	     << " for item:" << item.name  <<endreq;

  }
  if (node->type()==TrigConf::TriggerItemNode::AND) {
     BOOST_FOREACH(TrigConf::TriggerItemNode* child, node->children()) {
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
