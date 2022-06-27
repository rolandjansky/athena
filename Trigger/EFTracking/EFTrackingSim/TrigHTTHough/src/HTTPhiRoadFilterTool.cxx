// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTPhiRoadFilterTool.cxx
 * @author Elliot Lipeles - lipeles@cern.ch
 * @date March 25th, 2021
 * @brief Implements road filtering using eta module patterns
 */

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTConstants.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "HTTPhiRoadFilterTool.h"

#include "TH2.h"

#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <iostream>

static inline std::string instance_name(std::string const & s);


///////////////////////////////////////////////////////////////////////////////
// AthAlgTool

HTTPhiRoadFilterTool::HTTPhiRoadFilterTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
    base_class(algname, name, ifc),
    m_name(instance_name(name))
{
    declareInterface<IHTTRoadFilterTool>(this);
}


StatusCode HTTPhiRoadFilterTool::initialize()
{
  // Retrieve info
  ATH_CHECK(m_HTTMapping.retrieve());
  m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();
  
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Main Algorithm

StatusCode HTTPhiRoadFilterTool::filterRoads(const std::vector<HTTRoad*> & prefilter_roads, std::vector<HTTRoad*> & postfilter_roads) 
{
    ATH_MSG_INFO("Start Phi Road Filter"); 
    
    m_postfilter_roads.clear();
    postfilter_roads.clear();

    // Filter roads
    for (auto & road : prefilter_roads) {
      HTTRoad newroad = buildRoad(road);
      unsigned hit_layers = newroad.getHitLayers();
      
      unsigned layer_cnt=0;
      for (unsigned lyr = 0; lyr < m_nLayers; lyr++) {
	if (hit_layers & (1<<lyr)) layer_cnt++;
      }
      
      if (layer_cnt >= m_threshold) {
	m_postfilter_roads.push_back(newroad);
      }
	
    }

    // copy roads to outputs
    postfilter_roads.reserve(m_postfilter_roads.size());
    for (HTTRoad & r : m_postfilter_roads) postfilter_roads.push_back(&r);
    
    ATH_MSG_INFO("Event Done");
    
    m_event++;
    return StatusCode::SUCCESS;
}


HTTRoad_Hough  HTTPhiRoadFilterTool::buildRoad(HTTRoad* origr) const
{
  ATH_MSG_DEBUG("PhiRoad Build Road");
  HTTRoad_Hough& origr_hough = static_cast<HTTRoad_Hough&>(*origr);
  float phi = origr_hough.getX();
  float qPt  = origr_hough.getY();

  // make new road -- main alg doesn't keep it if not needed
  HTTRoad_Hough r(origr_hough); // only works with Hough roads!
  r.setNLayers(m_nLayers);
  layer_bitmask_t hitLayers = 0;
  
  // add hits
  for (unsigned lyr = 0; lyr < m_nLayers; lyr++) {
    std::vector<const HTTHit*> road_hits;
    for (auto hit : origr->getHits(lyr)) {
      float phi_expected = -1.0*asin(htt::A * hit->getR() * qPt) + phi;
      if (abs(hit->getGPhi()-phi_expected)< (m_window.value()[lyr]+qPt*m_ptscaling)) {
	road_hits.push_back(hit);
	hitLayers |= 1 << hit->getLayer();
      }
    }
    ATH_MSG_DEBUG("PhiRoad Hits " << lyr << " " << road_hits.size() << " " << origr->getHits(lyr).size());
    r.setHits(lyr,road_hits);
  }

  r.setHitLayers(hitLayers);
  return r;
}

static inline std::string instance_name(std::string const & s)
{
    size_t pos = s.find_last_of(".");
    if (pos != std::string::npos)
        return s.substr(pos + 1);
    return s;
}

