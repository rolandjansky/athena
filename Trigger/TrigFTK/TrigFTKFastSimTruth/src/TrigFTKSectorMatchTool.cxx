/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigFTKSectorMatch tool
// -------------------------------
// ATLAS Collaboration
//
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <cstdio>

#include "HepMC/GenParticle.h"

#include "TrigFTKFastSimTruth/FTKTruthTrajectory.h"
#include "TrigFTKFastSimTruth/TrigFTKSectorMatchTool.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKRawHit.h"

TrigFTKSectorMatchTool::TrigFTKSectorMatchTool(const std::string& type, const std::string& name, const IInterface* parent)
: base_class(type, name, parent),
  m_max_missing_8L(1),
  m_max_missing_12L(1),
  m_allow_extra_miss_12L(true),
  m_transition_region_max_eta(1.4),
  m_transition_region_min_eta(1.0)
{
  declareProperty("SectorFiles", m_sectorFiles);

  declareProperty("TransitionRegionMaxEta", m_transition_region_max_eta);
  declareProperty("TransitionRegionMinEta", m_transition_region_min_eta);

  declareProperty("PMapPath8L",  m_pmap8_path);
  declareProperty("PMapPath12L", m_pmap12_path);
  declareProperty("RMapPath",    m_rmap_path);
}

StatusCode TrigFTKSectorMatchTool::initialize()
{

  ATH_CHECK(loadRegionMap(m_rmap_path, m_moduleToRegionMap));

  ATH_CHECK(loadPixelPlaneMap(m_pmap8_path, m_pixelPlanes_8L));
  ATH_CHECK(loadPixelPlaneMap(m_pmap12_path, m_pixelPlanes_12L));

  ATH_CHECK(loadSCTPlaneMap(m_pmap8_path, m_sctPlanes_8L));
  ATH_CHECK(loadSCTPlaneMap(m_pmap12_path, m_sctPlanes_12L));

  ATH_MSG_DEBUG("Loading " << m_sectorFiles.size() << " sector files");
  m_sectors.reserve(m_sectorFiles.size());
  for (const auto& path: m_sectorFiles) {
    std::vector<Sector> sectorsInRegion;

    ATH_CHECK(loadSectors(path, sectorsInRegion));
    m_sectors.push_back(std::move(sectorsInRegion));
  }

  return StatusCode::SUCCESS;
}

uint64_t TrigFTKSectorMatchTool::ftkHash(bool isPixel, bool isEC, int layer, int phi, int eta) const
{
  return (static_cast<int>(isPixel) << 19) + (static_cast<int>(isEC) << 18) + (layer << 12) + (phi << 6) + eta;
}

StatusCode TrigFTKSectorMatchTool::loadRegionMap(const std::string& path,
  std::unordered_map<uint64_t, std::vector<int>>& moduleToRegionMap) const
{
  std::ifstream rmapFile(path);

  int region;

  std::string line;
  while (std::getline(rmapFile, line)) {
    if (line.substr(0,6) == "towers") {
      continue;
    }
    else if (line.size() < 3) {
      std::stringstream ss(line);
      ss >> region;
    }
    else if (line.size() > 0) {
      if (region < 0 || region >= 64) {
        ATH_MSG_FATAL("Invalid rmap!");
        return StatusCode::FAILURE;
      }

      int isPixel, isEC, layer, min_phi, max_phi, nModulesPhi, min_eta, max_eta, nModulesEta;

      std::stringstream ss(line);
      ss >> isPixel >> isEC >> layer >> min_phi >> max_phi >> nModulesPhi >> min_eta >> max_eta >> nModulesEta;

      std::vector<int> phiRange;
      std::vector<int> etaRange;

      if (min_phi == -1 || max_phi == -1 || min_eta == -1 || max_eta == -1) {
        continue;
      }

      if(min_phi <= max_phi) {
        for (int iPhi = min_phi; iPhi <= max_phi; iPhi++) {
          phiRange.push_back(iPhi);
        }
      }
      else{
        for (int iPhi = min_phi; iPhi < nModulesPhi; iPhi++) {
          phiRange.push_back(iPhi);
        }

        for (int iPhi = 0; iPhi <= max_phi; iPhi++) {
          phiRange.push_back(iPhi);
        }
      }


      if (min_eta <= max_eta){
        for (int iEta = min_eta; iEta <= max_eta; iEta++) {
          etaRange.push_back(iEta);
        }
      }
      else{
        for (int iEta = min_eta; iEta < nModulesEta; iEta++) {
          etaRange.push_back(iEta);
        }

        for (int iEta = 0; iEta <= max_eta; iEta++) {
          etaRange.push_back(iEta);
        }
      }

      for (size_t iPhi = 0; iPhi < phiRange.size(); iPhi++) {
        for (size_t iEta = 0; iEta < etaRange.size(); iEta++) {
          const auto moduleHash = ftkHash(isPixel, isEC, layer, phiRange[iPhi], etaRange[iEta]);
          moduleToRegionMap[moduleHash].push_back(region);
        }
      }

    }
  }

  for (auto& keyValuePair: moduleToRegionMap) {
    std::sort(keyValuePair.second.begin(), keyValuePair.second.end());
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigFTKSectorMatchTool::loadPixelPlaneMap(const std::string& path, PlaneMap& map) const
{
  std::ifstream input(path);
  std::string line;

  while (std::getline(input, line)) {
    if (line.empty() || line[0] != 'p')
      continue;

    int br_ec, unused1, layer, unused2, plane;
    if (sscanf(line.c_str(), "pixel %d %d %d dim %d plane %d", &br_ec, &unused1, &layer, &unused2, &plane) != 5) {
      ATH_MSG_FATAL("Invalid line in Pixel plane map: " << line);
      return StatusCode::FAILURE;
    }

    auto key = std::make_pair(layer, br_ec != 0);
    map[key] = plane;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigFTKSectorMatchTool::loadSCTPlaneMap(const std::string& path, PlaneMap& map) const
{
  std::ifstream input(path);
  std::string line;

  while (std::getline(input, line)) {
    if (line.empty() || line[0] != 'S')
      continue;

    int br_ec, unused1, layer, unused2, unused3, plane;
    if (sscanf(line.c_str(), "SCT %d %d %d dim %d stereo %d plane %d",&br_ec, &unused1, &layer, &unused2, &unused3, &plane) != 6) {
      ATH_MSG_FATAL("Invalid line in SCT plane map: " << line);
      return StatusCode::FAILURE;
    }

    auto key = std::make_pair(layer, br_ec != 0);
    map[key] = plane;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigFTKSectorMatchTool::loadSectors(const std::string& path, std::vector<Sector>& sectors) const
{
  std::unique_ptr<std::ifstream> sectorFile(ftk_dcap::open_for_read(path));
  std::string line;

  // read number of sectors and planes
  std::getline(*sectorFile, line);

  int numSectors, numPlanes;
  if (sscanf(line.c_str(), "%d %d", &numSectors, &numPlanes) != 2) {
      ATH_MSG_FATAL("Invalid sector file " << path << ": " << line);
      return StatusCode::FAILURE;
  }

  sectors.reserve(numSectors);

  if (numPlanes != 12) {
    ATH_MSG_FATAL("Sectors for " << numPlanes << " (!= 12) layers are not supported!");
    return StatusCode::FAILURE;
  }

  // read sector definitions
  while (std::getline(*sectorFile, line)) {
    Sector sec;
    int unused1, unused2;

    if (sscanf(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                &sec.id,
                &sec.moduleHashes[0],  &sec.moduleHashes[1],  &sec.moduleHashes[2],
                &sec.moduleHashes[3],  &sec.moduleHashes[4],  &sec.moduleHashes[5],
                &sec.moduleHashes[6],  &sec.moduleHashes[7],  &sec.moduleHashes[8],
                &sec.moduleHashes[9], &sec.moduleHashes[10], &sec.moduleHashes[11],
                &unused1, &unused2) != 15) {
      ATH_MSG_FATAL("Invalid sector file " << path << ": " << line);
      return StatusCode::FAILURE;
    }

    sectors.push_back(sec);
  }

  return StatusCode::SUCCESS;
}

bool TrigFTKSectorMatchTool::findMatchingCombinations(std::unordered_map<int, std::vector<HitHashAndIndex>> planeToHitMap8L,
  std::unordered_map<int, std::vector<HitHashAndIndex>> planeToHitMap12L,
  std::unordered_map<int,int> hitsPerRegion,
  std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
  std::vector<int>& regions, std::vector<int>& sectors) const
{
  const int nRequired = 9;

  const int max8LayerMisMatch = (planeToHitMap8L.size() == 8) ? 1 : 0;
  const int max12LayerMisMatch = (planeToHitMap12L.size() == 4) ? 1 : 0;

  for (auto region_numHits : hitsPerRegion) {

    //check # of hits in the region
    if(region_numHits.second < nRequired)
      continue;

    const int iBank = region_numHits.first;

    // Iterate through all sectors in this region, and see if the hits match the sector
    for (size_t iSector = 0; iSector < m_sectors[iBank].size(); ++iSector) {
      const auto& sector = m_sectors[iBank][iSector];

      // match by layer
      bool sectorMatched = true;
      int n_8LayerMisMatch = 0;
      int n_12LayerMisMatch = 0;

      std::vector<int> hitsOnPlane[12];

      for (int iPlane = 0; iPlane < 12; iPlane++) {
        bool layerMatch = false;

        const bool is8LPlane = planeToHitMap8L.count(iPlane) > 0;
        const bool is12LPlane = planeToHitMap12L.count(iPlane) > 0;

        if (is8LPlane && is12LPlane) {
          ATH_MSG_FATAL("A plane should not be found as 8L and 12L at the same time.");
        }

        if (is8LPlane) {
          const auto allHitsOnPlane = planeToHitMap8L.at(iPlane);
          const auto moduleId = sector.moduleHashes[iPlane];

          for (const HitHashAndIndex& hit: allHitsOnPlane) {
            if (hit.first == moduleId) {
              layerMatch = true;
              hitsOnPlane[iPlane].push_back(hit.second);
            }
          }
          if (!layerMatch) {
            n_8LayerMisMatch++;
            if (n_8LayerMisMatch > max8LayerMisMatch) {
              sectorMatched = false;
              break;
            }
          }
        }
        else if (is12LPlane) {
          const auto allHitsOnPlane = planeToHitMap12L.at(iPlane);

          const auto moduleId = sector.moduleHashes[iPlane];

          for (const HitHashAndIndex& hit: allHitsOnPlane) {
            if (hit.first == moduleId) {
              layerMatch = true;
              hitsOnPlane[iPlane].push_back(hit.second);
            }
          }
          if (!layerMatch) {
            n_12LayerMisMatch++;
            if (n_12LayerMisMatch > max12LayerMisMatch) {
              sectorMatched = false;
              break;
            }
          }
        }
      }

      // If the sector is matched, form all possible combinations of hits.
      // There could be multiple hits per plane.
      if (sectorMatched) {
        std::vector<int>::const_iterator startlist[12];
        std::vector<int>::const_iterator position[12];
        std::vector<int>::const_iterator endlist[12];
        int ncomb = 1;

        for (int curpl=0; curpl < 12; curpl++) {
          int nhits = hitsOnPlane[curpl].size();

          if(nhits == 0){
            startlist[curpl] = endlist[curpl] = position[curpl] = std::vector<int>::const_iterator();
          }
          else{// set the list iterators
            position[curpl] = startlist[curpl] = hitsOnPlane[curpl].begin();
            endlist[curpl] = hitsOnPlane[curpl].end();
            ncomb *= nhits;
          }
        }

        for (int comb=0;comb< ncomb;++comb) {

          sectors.push_back(sector.id);
          regions.push_back(iBank);

          std::unordered_map<int, int> this_sectorHitToPlaneMap;
          for (int curpl=0;curpl<12;curpl++) {
            if (startlist[curpl]==endlist[curpl]) // empty hit
              continue;

            this_sectorHitToPlaneMap[*position[curpl]] = curpl;
          }

          sectorHitToPlaneMap.push_back( this_sectorHitToPlaneMap );

          //change combination
          for (int curpl=0;curpl<12;curpl++) {
            if (startlist[curpl]==endlist[curpl]) // empty hit
              continue;

            // move to the next
            ++position[curpl];
            if (position[curpl] != endlist[curpl]){// possible combination
              break;
            }
            else {// restart on this plane, move the next plane
              position[curpl] = startlist[curpl];
            }
          }
        }
      }
    }
  }

  return sectorHitToPlaneMap.size() > 0;
}

bool TrigFTKSectorMatchTool::match(const FTKTruthTrajectory &traj,
    std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    std::vector<int>& regions,
    std::vector<int>& sectors) const
{

  ATH_MSG_DEBUG("Checking FTKTruthTrajectory " << traj.genParticle->barcode() << " with " << traj.hits.size() << " hits");

  /// start to emulate FTK layer requirements
  std::unordered_map<int, std::vector<HitHashAndIndex>> planeToHitMap8L; // this is a map for hit and first 8 layers in 12 layer numbering
  std::unordered_map<int, std::vector<HitHashAndIndex>> planeToHitMap12L; // this is a map for hit and the rest 4 layers in 12 layer numbering
  std::vector<int> matchedRegions;

  std::unordered_set<int> planesWithHit8L, planesWithHit12L;


  int ihit = 0;
  for (const auto& hit: traj.hits) {
    if (hit->getIsPixel()) {
      const int layer_disk = hit->getLayer();
      const int br_ec = hit->getBarrelEC();
      const int phimod = hit->getPhiModule();
      const int etamod = hit->getEtaModule();

      int eta_conv = etamod;

      if (br_ec == 0)
        eta_conv += (layer_disk == 0) ? 10 : 6;

      const bool isEC = br_ec != 0;

      const auto key = std::make_pair(layer_disk, isEC);
      const int pixelPlane8L = m_pixelPlanes_8L.count(key) > 0 ? m_pixelPlanes_8L.at(key) : -1;
      const int pixelPlane12L = m_pixelPlanes_12L.count(key) > 0 ? m_pixelPlanes_12L.at(key) : -1;

      const auto ftkModuleHash = ftkHash(true, isEC, layer_disk, phimod, eta_conv);
      if (m_moduleToRegionMap.count(ftkModuleHash) > 0) {
        std::vector<int> moduleRegions = m_moduleToRegionMap.at(ftkModuleHash);
        matchedRegions.insert(matchedRegions.begin(),moduleRegions.begin(),moduleRegions.end());
      }

      // std::vector<int> thisHit;
      // thisHit.push_back(hit->getIdentifierHash());
      // thisHit.push_back(ihit); // first is the offline ID, second is the hit number

      HitHashAndIndex thisHit(hit->getIdentifierHash(), ihit);

      ATH_MSG_DEBUG("FTK Pix " << hit->getIdentifierHash() << " " << pixelPlane8L << " " << pixelPlane12L);

      if (pixelPlane8L >= 0) {
        planesWithHit8L.insert(pixelPlane8L);

        /// record hits
        planeToHitMap8L[pixelPlane12L].push_back(thisHit);
      }
      else if (pixelPlane12L >= 0) {
        planesWithHit12L.insert(pixelPlane12L);

        //record hits
        planeToHitMap12L[pixelPlane12L].push_back(thisHit);
      }
    }
    else if (hit->getIsSCT()) {
      const int layer_disk = hit->getLayer();
      const int br_ec = hit->getBarrelEC();
      const int phimod = hit->getPhiModule();
      const int etamod = hit->getEtaModule();

      int eta_conv = etamod;

      if (br_ec == 0) {
        eta_conv += 6;
      }
      else {
        eta_conv = 0;
      }

      const bool isEC = br_ec != 0;

      auto key = std::make_pair(layer_disk, isEC);
      const int sctPlane8L = m_sctPlanes_8L.count(key) > 0 ? m_sctPlanes_8L.at(key) : -1;
      const int sctPlane12L = m_sctPlanes_12L.count(key) > 0 ? m_sctPlanes_12L.at(key) : -1;

      const auto ftkModuleHash = ftkHash(false, isEC, layer_disk, phimod, eta_conv);

      if (m_moduleToRegionMap.count(ftkModuleHash) > 0) {
        std::vector<int> moduleRegions = m_moduleToRegionMap.at(ftkModuleHash);
        matchedRegions.insert(matchedRegions.begin(),moduleRegions.begin(),moduleRegions.end());
      }

      // std::vector<int> thisHit;
      // thisHit.push_back(hit->getIdentifierHash());
      // thisHit.push_back(ihit); // first is the offline ID, second is the hit number
      HitHashAndIndex thisHit(hit->getIdentifierHash(), ihit);

      if (sctPlane8L >= 0) {
        planesWithHit8L.insert(sctPlane8L);

        /// record hits
        planeToHitMap8L[sctPlane12L].push_back(thisHit);
      }
      else if (sctPlane12L >= 0) {
        planesWithHit12L.insert(sctPlane12L);

        /// record hits
        planeToHitMap12L[sctPlane12L].push_back( thisHit );
      }
    }

    ihit++;
  }

  if (!hitRequirement8L(planesWithHit8L)) {
    ATH_MSG_DEBUG("FTKTruthTrajectory failed 8L requirement");
    return false;
  }

  if (!hitRequirement12L(planesWithHit12L, traj.genParticle->momentum().pseudoRapidity())) {
    ATH_MSG_DEBUG("FTKTruthTrajectory failed 12L requirement");
    return false;
  }

  std::unordered_map<int,int> hitsPerRegion;
  for (const auto& region: matchedRegions)
    hitsPerRegion[region]++;

  // findMatchingCombinations returns true, if at least one
  // combination is found
  return findMatchingCombinations(planeToHitMap8L,
    planeToHitMap12L,
    hitsPerRegion,
    sectorHitToPlaneMap,
    regions,
    sectors);
}

bool TrigFTKSectorMatchTool::hitRequirement8L(const std::unordered_set<int>& planesWithHit) const
{
  int expectedHits = m_nplanes_8L - m_max_missing_8L;

  // we ignore a missing hit in the last layer
  if (planesWithHit.count(m_nplanes_8L-1) == 0) {
    expectedHits -= 1;
  }

  return static_cast<int>(planesWithHit.size()) >= expectedHits;
}

bool TrigFTKSectorMatchTool::hitRequirement12L(const std::unordered_set<int>& planesWithHit, float trackEta) const
{
  int expectedHits = m_nplanes_12L - m_max_missing_12L;

  // we ignore a missing hit for tracks in the transition region
  if (m_allow_extra_miss_12L && isTransitionRegion(trackEta)) {
    expectedHits -= 1;
  }

  return static_cast<int>(planesWithHit.size()) >= expectedHits;
}

bool TrigFTKSectorMatchTool::isTransitionRegion(float eta) const
{
  return abs(eta) > m_transition_region_min_eta && abs(eta) < m_transition_region_max_eta;
}
