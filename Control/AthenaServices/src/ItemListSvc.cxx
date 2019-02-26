///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ItemListSvc.cxx 
// Implementation file for class ItemListSvc
// Author: J.Cranshaw<cranshaw@anl.gov>
/////////////////////////////////////////////////////////////////// 


#include "ItemListSvc.h"

// Gaudi include files
#include <algorithm>
#include <sstream>


ItemListSvc::ItemListSvc(const std::string& name, 
		       ISvcLocator* pSvcLocator ) : 
  base_class(name, pSvcLocator),
  m_verboseThresh(0.20)
{
  declareProperty("VerboseThreshold",m_verboseThresh,"overlaps above this fraction print their items, def=0.2");
  assert( pSvcLocator );
}

ItemListSvc::~ItemListSvc()
{
}
StatusCode ItemListSvc::initialize()
{
  ATH_MSG_DEBUG("ItemListSvc initialize");
  return StatusCode::SUCCESS;
}

// Just print out the streams for each item
StatusCode ItemListSvc::finalize()
{
  ATH_MSG_DEBUG("ItemListSvc finalize");
  std::map<std::string, std::set<std::string> >::const_iterator it = m_streamItems.begin();
  ATH_MSG_DEBUG("-- OUTPUT STREAM ITEM OVERLAP SUMMARY --");
  while (it != m_streamItems.end()) {
    ATH_MSG_DEBUG("STREAM " << it->first << " has (" << it->second.size() << ") items");
    std::set<std::string>::const_iterator iprint = it->second.begin();
    while (iprint != it->second.end()) {
      ATH_MSG_DEBUG("   - " << *iprint);
      ++iprint;
    }
    std::map<std::string, std::set<std::string> >::const_iterator it2 = m_streamItems.begin();
    float inv_size = 1. / static_cast<float> (it->second.size());
    while (it2 != m_streamItems.end()) {
      if (it2->first != it->first) {
        std::set<std::string> olist;
        std::set_intersection(it->second.begin(),it->second.end(),it2->second.begin(),it2->second.end(),std::inserter(olist,olist.begin()));
        if (olist.size()>0) { 
          ATH_MSG_DEBUG(" --> Overlap with " << it2->first << " (" << olist.size() << ") items"); 
          if (float(olist.size()) * inv_size > m_verboseThresh) {
            for (std::set<std::string>::iterator olit = olist.begin(); olit != olist.end(); olit++) {
              ATH_MSG_DEBUG(" ----> item: " << *olit);
            }
          } 
        }
      }
      ++it2;
    }
    ++it;
  }
  return StatusCode::SUCCESS;
}

// add item to list for stream
StatusCode ItemListSvc::addStreamItem(const std::string& stream, const std::string& itemname)
{
  // Add to stream list
  // Check if item is already present
  std::map<std::string, std::set<std::string> >::iterator it = m_streamItems.find(stream);
  // if so, then add stream name for that item
  if (it != m_streamItems.end()) {
     it->second.insert(itemname);
  }
  // otherwise add item and stream
  else {
     std::set<std::string> start;
     start.insert(itemname);
     std::pair<std::map<std::string, std::set<std::string> >::iterator,bool> retc = m_streamItems.insert(std::make_pair(stream,start));
     if (!retc.second) {
        ATH_MSG_ERROR("Problem inserting " << retc.first->first);
     }
  }
  return StatusCode::SUCCESS;
}

// remove item from list for stream
StatusCode ItemListSvc::removeStreamItem(const std::string& stream, const std::string& itemname)
{
  std::map<std::string, std::set<std::string> >::iterator it = m_streamItems.find(stream);
  if (it == m_streamItems.end()) {
    ATH_MSG_INFO("Tried to remove non-existing item " << itemname << " for " << stream);
  }
  else {
    // remove stream for item
    int n = it->second.erase(itemname);
    if (n<1) ATH_MSG_WARNING("Could not find stream " << stream << " for " << itemname);
  }
  return StatusCode::SUCCESS;
}

// simple test to see if item or (stream,item) is already being written
bool ItemListSvc::containsItem(const std::string& itemname, const std::string& stream) const
{
  bool contains=false;
  std::map<std::string, std::set<std::string> >::const_iterator it = m_streamItems.begin();
  while (it != m_streamItems.end()) {
    if (it->first == stream || stream == "ANY") { 
      if (it->second.find(itemname) != it->second.end()) contains = true;
    }
    ++it;
  }
  return contains;
}

// This returns all the streams that an item was written to
std::vector<std::string> ItemListSvc::getStreamsForItem(const std::string& itemname) const
{
  std::vector<std::string> t;
  std::map<std::string, std::set<std::string> >::const_iterator it = m_streamItems.begin();
  while (it != m_streamItems.end()) {
    if (it->second.find(itemname) != it->second.end()) t.push_back(it->first);
    ++it;
  }
  return t;
}

// This returns the items that were output to a certain stream
std::vector<std::string> ItemListSvc::getItemsForStream(const std::string& stream) const
{
  std::vector<std::string> t;
  std::map<std::string, std::set<std::string> >::const_iterator it = m_streamItems.find(stream);
  if (it != m_streamItems.end()) std::copy(it->second.begin(), it->second.end(),t.begin());
  return t;
}
