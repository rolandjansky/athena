/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_FTK_SECTOR_MATCH_TOOL_H__
#define __TRIG_FTK_SECTOR_MATCH_TOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigFTKToolInterfaces/ITrigFTKSectorMatchTool.h"

#include <map>
#include <string>
#include <vector>


class FTKTruthTrajectory;

class TrigFTKSectorMatchTool : public extends<AthAlgTool, ITrigFTKSectorMatchTool>
{
public:
  TrigFTKSectorMatchTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigFTKSectorMatchTool() = default;
  StatusCode initialize() override;

  bool match(const FTKTruthTrajectory &traj,
    std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    std::vector<int>& regions,
    std::vector<int>& sectors) const override;

private:
  using LayerEC = std::pair<int, bool>;
  using PlaneMap = std::map<LayerEC, int>;

  PlaneMap m_pixelPlanes_8L;
  PlaneMap m_pixelPlanes_12L;
  PlaneMap m_sctPlanes_8L;
  PlaneMap m_sctPlanes_12L;

  std::string m_pmap8_path;
  std::string m_pmap12_path;

  std::unordered_map<uint64_t, std::vector<int>> m_moduleToRegionMap;
  std::string m_rmap_path;

  struct Sector
  {
    int id;
    std::array<int, 12> moduleHashes;
  };

  std::vector<std::vector<Sector>> m_sectors; // list of sectors for each region
  std::vector<std::string> m_sectorFiles; // path to sector files

  static const int m_nplanes_8L = 8;
  static const int m_nplanes_12L = 12 - m_nplanes_8L;
  int   m_max_missing_8L;
  int   m_max_missing_12L;
  bool  m_allow_extra_miss_12L;
  float m_transition_region_max_eta;
  float m_transition_region_min_eta;

  using HitHashAndIndex = std::pair<int, int>;

  StatusCode loadRegionMap(const std::string& path, std::unordered_map<uint64_t, std::vector<int>>& moduleToRegionMap) const;

  StatusCode loadPixelPlaneMap(const std::string& path, PlaneMap& map) const;
  StatusCode loadSCTPlaneMap(const std::string& path, PlaneMap& map) const;

  StatusCode loadSectors(const std::string& path, std::vector<Sector>& sectors) const;

  bool findMatchingCombinations(std::unordered_map<int, std::vector<HitHashAndIndex>> planeToHitMap8L,
    std::unordered_map<int, std::vector<HitHashAndIndex>> planeToHitMap12L,
    std::unordered_map<int,int> selectedRegion, // {Region, # of hits}
    std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    std::vector<int>& region, std::vector<int>& sector ) const;

  bool hitRequirement8L(const std::unordered_set<int>& planesWithHit) const;
  bool hitRequirement12L(const std::unordered_set<int>& planesWithHit, float trackEta) const;
  bool isTransitionRegion(float eta) const;

  uint64_t ftkHash(bool isPixel, bool isEC, int layer, int phi, int eta) const;
};

#endif
