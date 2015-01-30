/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROADFINDER_H
#define ROADFINDER_H

#include "FTKSetup.h"
#include "FTKDataInput.h"
#include "FTKRoadFileOutput.h"
#include "FTKSectorMap.h"

// We need this to access RawInput-specific information in SCTtrk mode
// (m_datainput has to be dynamic_cast into FTK_RawInput)
// in the future, relevant FTK_RawInput stuff can be moved to FTKDataInput
#include "FTK_RawInput.h"

class FTK_AMsimulation_base;

class RoadFinder {
private:
  int m_verbosity;

  FTKDataInput *m_datainput;  // generic pointer to read input data
  
  FTKRoadOutput *m_roadoutput; // generic pointer to write road results

  int m_nbanks; // number of banks in this configuration
  int m_nsubs; // # of subregions (gets from ${M})
  int m_sub; // current subregion

  FTK_AMsimulation_base **m_banks; // array with the  AM banks

  bool *m_goodRegion; // array with a flag to check which region are really simulated

  int m_roadwarrior; // road warrior level, 0 off

  // resources needed for PIX+SCTtrk mode
  int m_SCTtrk_nlayers;
  int m_SCTtrk_nsubs;
  int m_SCTtrk_lastlayer;
  int m_SCTtrk_maxMissingPlanes;
  FTKSectorMap *m_sector_map;

  bool m_SSsearch_unused; // group hits in unused layers in SSs

public:
  RoadFinder();
  ~RoadFinder();

  void init();
  void end();

  void setVerbosity(int v) { m_verbosity = v; }
  int getVerbosity() { return m_verbosity; }

  // set the data input module using an abstract class
  void setDataInputModule(FTKDataInput *module)
    { m_datainput = module; }
   FTKDataInput* getDataInputModule() 
   { return m_datainput; }

   void setRoadOutputModule(FTKRoadOutput *module)
   { m_roadoutput = module; }

   FTKRoadOutput* getRoadOutputModule()
   { return m_roadoutput; }

   void setNBanks(int);
   int getNBanks() const { return  m_nbanks; }

  void setAMBank(int, FTK_AMsimulation_base*);
  FTK_AMsimulation_base* getAMBank(int id) {
    return m_banks[id];
  }

  void setSCTtrkNlayers(int v) { m_SCTtrk_nlayers = v; }
  int getSCTtrkNlayers() const { return m_SCTtrk_nlayers; }

  void setSCTtrkNsubs(int v) { m_SCTtrk_nsubs = v; }
  int getSCTtrkNsubs() const { return m_SCTtrk_nsubs; }

  void setNsubregions(int v) { m_nsubs = v; }
  int getNsubregions() const { return m_nsubs; }

  void setSubregion(int v) { m_sub = v; }
  int getSubregion() const { return m_sub; }

  void setSCTtrkLastLayer(int v) { m_SCTtrk_lastlayer = v; }
  int getSCTtrkLastLayer() const { return m_SCTtrk_lastlayer; }

  void setSCTtrkMaxMissingPlanes(int v) { m_SCTtrk_maxMissingPlanes = v; }
  int getSCTtrkMaxMissingPlanes() const { return m_SCTtrk_maxMissingPlanes; }

  void setSectorMap(FTKSectorMap *s) { m_sector_map = s; }
  int applySectorMap(int, int);

  void setSSSearchUnused(bool flag) { m_SSsearch_unused = flag; }
  bool getSSSearchUnused() const { return m_SSsearch_unused; }

  int nextEvent(); // elaborate the following event
};
#endif // ROADFINDER_H
