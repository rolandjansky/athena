/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SINGLETRACKINPUT_H
#define FTK_SINGLETRACKINPUT_H

#include "TrigFTKSim/FTK_RawInput.h"
#include "TrigFTKSim/PatternBank.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "TrigFTKSim/FTKCacheLookup.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"

/** A class that implements most of TrigFTKLib functionality:
    - reading in single muons
    - filtering away noisy events
    - producing sectors or patterns
    - fitting-on-the-fly of clean single muon tracks
*/
class FTK_SingleTrackInput : public FTK_RawInput
{
  // Additional variables for single muon mode
 protected:
  const FTKRegionMap *m_rmap;      // needed for filterHits function
  const FTKSSMap *m_ssmap;         // to get patterns
  PatternBank **m_sectors;         // sector banks (in subregions)
  PatternBank **m_patterns;        // pattern banks (in subregions)
  FTKConstantBank **m_constants;   // constant banks (in subregions)
  PatternDefinition m_sector;      // current sector
  PatternDefinition m_pattern;     // current pattern
  FTKTrack m_track;                // current track

public:
  FTK_SingleTrackInput(const FTK_SingleTrackInput& v);
  FTK_SingleTrackInput(const FTKPlaneMap *pmap, const FTKRegionMap *rmap, const FTKSSMap *ssmap = 0);
  virtual ~FTK_SingleTrackInput();

  /* single muon mode functions */
  int filterHits();   // similar to old filterHits in TrigFTKLib
  void singleMuonMode(); // set some defaults for m_sector, m_pattern, m_track
  void reservePatterns(int subreg, bool createEmpty=false);
  void reserveSectors(int subreg, bool createEmpty=false);
  void loadSectors(int subreg, const char *path);
  void loadConstants(int subreg, const char *path);
  int Region();   // gets a region out of filtered m_hits
  const PatternDefinition& Sector() { return m_sector; }
  const PatternDefinition& Pattern() { return m_pattern; }
  const FTKTrack& Track() { return m_track; }
  void DumpTrack(std::ostream&);
  void DumpTrack(FTKTrackLookup&);
  int getSector();   // puts sector ids[nplanes] into m_sector
  int findSector();  // puts sector_id in the bank into m_sector, and subregion into m_subregion
  int getPattern();  // puts pattern ids[nplanes] and sector_id into m_pattern
  int performFit();  // fit the current single muon event and save results in m_track
  void saveSector(int sub) {
    m_sectors[sub]->addPattern(m_sector);
  }
  void savePattern(int sub) {
    m_patterns[sub]->addPattern(m_pattern);
  }
  PatternBank *getSectorBank(int sub) { return m_sectors[sub]; }
  PatternBank *getPatternBank(int sub) { return m_patterns[sub]; }

};
#endif // FTK_SINGLETRACKINPUT_H
