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

// Helper
namespace {
  template<class T>
  bool contains(const std::vector<T>& vec, const T& val) 
  {
    return std::find(vec.begin(), vec.end(), val) != vec.end();
  }
}


Lvl1ConsistencyChecker::Lvl1ConsistencyChecker(const std::string& name, const std::string& type,
                                               const IInterface* parent) 
  : AthAlgTool(name, type, parent)
{
  declareProperty("printWarnings", m_printWarnings=true, "print detailed error reports");
  declareProperty("returnFailure", m_returnFailure=false, "in case of inconsitency return FAILURE");
  declareProperty("maxTOBs", m_returnFailure=false, "in case of inconsitency return FAILURE");
  declareProperty("thresholdsToCheck", m_thresholdsToCheck, "trigger threshold types to check");

  declareInterface<Lvl1ConsistencyChecker>( this );
} 

StatusCode Lvl1ConsistencyChecker::start() {

  // create set of threshold names
  std::set<std::string> threshold_names;
  for ( const auto & item : m_configuredItems ) {
    for ( const auto & thr : item.second.thresholds ) {
      threshold_names.insert(thr.second.name);
    }
  }
  
  // now book the histogram
  m_hist = new TH1I("ThresholdItemMisses", "Thresholds missing for items", 
                    threshold_names.size(), 0, threshold_names.size());

  // label x axis
  uint bin = 1;
  for ( const std::string & thrName : threshold_names ) {
    m_hist->GetXaxis()->SetBinLabel(bin++, thrName.c_str());
  }

  m_histAll = (TH1I*)m_hist->Clone("ThresholdItemMissesAll");
  m_histAll->SetTitle("Thresholds missing for items (incl. overflows)");

  ITHistSvc* histSvc(0);
  CHECK(service("THistSvc", histSvc));

  std::string bookname = "/EXPERT/"+name()+"/";
  replace(bookname.begin(), bookname.end(), '.', '/');

  CHECK(histSvc->regHist( bookname+m_hist->GetName(), m_hist) );
  CHECK(histSvc->regHist( bookname+m_histAll->GetName(), m_histAll) );

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
        ATH_MSG_DEBUG("Threshold: " << thr.second.name << " type: " << thr.second.type
                      << " multiplicity: " << unsigned(thr.second.multiplicity) << " " << thr.first); 
      }
    }
  }
   
  return StatusCode::SUCCESS;
}


HLT::ErrorCode
Lvl1ConsistencyChecker::check(const std::vector<const LVL1CTP::Lvl1Item*>& items,
                              const HLT::Navigation* nav,
                              const std::vector<std::string>& ignoreThresholds)
{
  std::vector<ThresholdId> errors;
  HLT::SteeringInternalReason::Code failure = HLT::SteeringInternalReason::UNKNOWN;

  for ( const LVL1CTP::Lvl1Item* item : items ){
    if ( ! item->isPassedAfterVeto() ) // we do not need to check items which did not pass
      continue;

    uint32_t id = item->hashId();
    auto cIt = m_configuredItems.find(id);
    if ( cIt == m_configuredItems.end() ) {
      ATH_MSG_WARNING("Item which is passed to the checker was not known from configuration, id: " << item->hashId() << " " << item->name());
      continue;
    }

    // now check if all thresholds (TEs) are present
    for ( const auto & thr : cIt->second.thresholds ) {
         
      unsigned found = nav->countAllOfType(thr.first, false);

      if ( found < thr.second.multiplicity ) { // here we have a problem
        if (!contains(errors, thr.first)) {    // only fill histogram once per missing threshold
          errors.push_back(thr.first);
          
          // Fill all missing thresholds
          m_histAll->Fill(thr.second.name.c_str(), 1.0);
          // Fill all except ignored thresholds (e.g. overflows)
          if ( !contains(ignoreThresholds, thr.second.type) ) {
            if (thr.second.type=="CALO")       failure = HLT::SteeringInternalReason::MISSING_CALO_ROI;
            else if (thr.second.type=="MUON")  failure = HLT::SteeringInternalReason::MISSING_MUON_ROI;
            else                               failure = HLT::SteeringInternalReason::MISSING_OTHER_ROI;
            m_hist->Fill(thr.second.name.c_str(), 1.0);
          }
        }
        if (m_printWarnings) {          // report problem for all items
          ATH_MSG_WARNING("Item " << cIt->second.name << " required: " 
                          << unsigned(thr.second.multiplicity) << " of: " << thr.second.name << " while got: " << found);
        }        
      }
    }
  }
  
  if ( failure != HLT::SteeringInternalReason::UNKNOWN ) {
    return HLT::ErrorCode(m_returnFailure ? HLT::Action::ABORT_EVENT : HLT::Action::CONTINUE, 
                          HLT::Reason::MISSING_FEATURE, failure );
  }

  return HLT::OK;
}





//Fills in a map recursively counting how many RoIs a trigger item requires.
//Note that it only can deal with AND'd thresholds, not OR'd or NOT for RoI items such as in forward jet items
//items not used in the L2 will also not be checked correctly as the TE used for the lookup is 0 for all
void Lvl1ConsistencyChecker::makeItemRoIMap(const TrigConf::TriggerItemNode *node, Lvl1ConsistencyChecker::Item& item) {
  if (!node) return;
  if (node->isThreshold() && node->triggerThreshold()) {
    if ( contains(m_thresholdsToCheck, node->triggerThreshold()->type()) ) {
      Threshold t;
      t.name = node->triggerThreshold()->name();
      t.type = node->triggerThreshold()->type();
      t.multiplicity = node->multiplicity();
      item.addThreshold(t); 
    }
    //    ATH_MSG_DEBUG(" found threshold: " << node->triggerThreshold()->name() <<" with multiplicity:" << node->multiplicity(); 
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
  auto ti = thresholds.find(id);
  if ( ti != thresholds.end() )
    thresholds[id].multiplicity += t.multiplicity;
  else
    thresholds[id] = t;
}



