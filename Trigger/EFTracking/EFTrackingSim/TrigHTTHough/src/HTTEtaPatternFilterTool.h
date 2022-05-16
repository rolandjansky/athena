// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTETAPATTERNFILTERTOOL_H
#define HTTETAPATTERNFILTERTOOL_H

/**
 * @file HTTEtaPatternFilterTool.h
 * @author Elliot Lipeles - lipeles@cern.ch
 * @date March 25th, 2021
 * @brief Implements road filtering using eta module patterns
 *
 * Declarations in this file:
 *      class HTTEtaPatternFilterTool : public AthAlgTool, virtual public IHTTRoadFilterTool
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTHough/IHTTRoadFilterTool.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

#include "TFile.h"

#include <string>
#include <vector>
#include <map>
#include <boost/dynamic_bitset_fwd.hpp>

class HTTEtaPatternFilterTool : public extends<AthAlgTool, IHTTRoadFilterTool>
{
 public:

  ///////////////////////////////////////////////////////////////////////
  // AthAlgTool

  HTTEtaPatternFilterTool(const std::string&, const std::string&, const IInterface*);

  virtual StatusCode initialize() override;

  ///////////////////////////////////////////////////////////////////////
  // IHTTRoadFilterTool

  virtual StatusCode filterRoads(const std::vector<HTTRoad*> & prefilter_roads, std::vector<HTTRoad*> & postfilter_roads) override;

 private:

  ///////////////////////////////////////////////////////////////////////
  // Handles
  ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping {this, "TrigHTTMappingSvc", "TrigHTTMappingSvc"};

  ///////////////////////////////////////////////////////////////////////
  // Properties

  Gaudi::Property <std::string> m_pattern_file_path{this, "EtaPatterns", "", "path to pattern file"};
  Gaudi::Property <unsigned> m_threshold {this, "threshold", 0, "Minimum number of hit layers to fire a road"};


  ///////////////////////////////////////////////////////////////////////////////
  // Utility Structs
  
  struct ModuleId
  {
    SiliconTech siTech = SiliconTech::undefined;
    DetectorZone zone = DetectorZone::undefined;
    int etaModule = 0;
  };
  friend bool operator <(const ModuleId& lhs, const ModuleId& rhs);
  typedef std::vector<ModuleId> EtaPattern; // list of module ids in each layer

  
// For a specific moduleId, stores pointers to the bitmasks of each pattern
// that contains this module, set during initialize().
// This class is also reused every input road, storing the hits in the module.

  struct ModulesToPattern
  {
    std::vector<layer_bitmask_t*> m_pattern_bitmasks; // these point to the values in m_pattermap, created in initialize()
    std::vector<const HTTHit*> m_hits; // reset every input road
    
    void reset() { m_hits.clear(); }
    void addPattern(layer_bitmask_t* counter) { m_pattern_bitmasks.push_back(counter); }
    void addHit(const HTTHit* hit)
    {
      if (hit->getHitType() != HitType::wildcard) {
	m_hits.push_back(hit);
	for (layer_bitmask_t* counter : m_pattern_bitmasks)
	  (*counter) |= (1 << hit->getLayer());
      }
    }
    const std::vector<const HTTHit*> & getHits() const { return m_hits; }
  };
  

  ///////////////////////////////////////////////////////////////////////
  // Event Storage
  std::vector<HTTRoad_Hough> m_postfilter_roads;
  
  ///////////////////////////////////////////////////////////////////////
  // Convenience
  
  unsigned m_nLayers = 0U; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();
  
  // The below maps are created in initialize, with fixed keys. But the counters (values)
  // are reset every input road.
  std::map<EtaPattern, layer_bitmask_t> m_patternmap;
  // keys initialized from file
  // for each input road, the bitmask is reset
  std::vector<std::map<ModuleId, ModulesToPattern>> m_moduleHits;
  // inverses the above map, mapping (layer, moduleId) to patterns
  // note this stores pointers to m_patternmap, and will modify it
  // also stores a list of hits for each input road
  
  ///////////////////////////////////////////////////////////////////////
  // Helpers
  void readPatterns(std::string const & filepath);
  void buildMap();
  void resetCounters();
  void addHitsToMap(HTTRoad* r);
  void addRedundantPatterns(std::set<EtaPattern> & usedPatterns, EtaPattern const & currPatt, unsigned nExtra);
  HTTRoad_Hough buildRoad(std::pair<EtaPattern, layer_bitmask_t> const & patt, HTTRoad* origr) const;
  std::string to_string(const EtaPattern &patt) const;
  std::string to_string(const HTTRoad &road) const;
  std::string to_string(const std::vector<unsigned> &v) const;
};

#endif // HTTETAPATTERNFILTERTOOL_H
