/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKDATAINPUT_H
#define FTKDATAINPUT_H

#include "TrigFTKSim/FTKTruthTrack.h"
#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/FTKTruthTrack.h"
#include "TrigFTKSim/FTKHit.h"
#include "TrigFTKSim/FTKPMap.h"
// needed to read SCTtrk tracks (Constantinos case)
#include "TrigFTKSim/FTKTrackInput.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTKRoadInput.h"
#include "TrigFTKSim/FTKTrack.h"
#include "StoreGate/StoreGateSvc.h"

#include <vector>
#include <iostream>
#include <sstream>

class MultiTruth;


/** base class to manage input from data */
class FTKDataInput {
private:

protected:
  int m_nevent; // number of events processed

  const FTKPlaneMap *m_pmap;
  const FTKPlaneMap *m_pmap_unused; 

  std::vector<FTKHit> m_hits; // final list of hits, after pmap is applied
  std::vector<FTKHit> *m_reghits; // final list of hits, after pmap and rmap are applied
  std::vector<FTKHit> m_hits_unused; // list of the ignored hits

  int m_run_number; // event's run number
  int m_event_number; // event number
  float m_averageInteractionsPerCrossing;
  float m_actualInteractionsPerCrossing;
  int m_LB;
  int m_BCID;
  unsigned int m_extendedLevel1ID;
  unsigned int m_level1TriggerType;
  std::vector<unsigned int> m_level1TriggerInfo;
  StoreGateSvc*  m_storeGate;
  StoreGateSvc*  m_detStore;
  StoreGateSvc*  m_evtStore;
  
  std::vector<FTKRawHit> m_original_hits; // global list of raw hits
  std::vector<FTKRawHit> **m_original_reghits; // local list of hits, RMAP applied
  std::vector<FTKTruthTrack> m_truth_track; // list of raw hits
  std::vector<FTKHit*> **m_reghits_read; // local list of hits for reading in FTKHits directly

  // additional variables that may be useful
  int m_nplanes;
  int m_nregions;
  int m_nsubregions;
  int m_ncoords;
  int m_region;
  int m_subregion;

  bool m_Clustering; // flag to enable the clustering
  bool m_SaveClusterContent; // flag to allow the debug of the cluster content
  bool m_DiagClustering; // true if diagonal clustering is used
  bool m_SctClustering; // true if SCT clustering is performed
  unsigned int m_PixelClusteringMode; // 1 means ToT information is used
                                      //   && 400/600um pixels are accounted for
                                      // 0 is default used up to 2013
  bool m_Ibl3DRealistic; // If true, use HWlike clustering in IBL 3D mod
  bool m_DuplicateGanged;
  bool m_GangedPatternRecognition;

  bool m_SplitBlayerModules; // true if the BLayer modules are splitted

  bool m_init; // tell if the read is initialized
  bool m_save_unused; // save the hits in the unsed planes
  bool m_read_clusters; // read cluster info and don't bother performing clustering

  bool m_read_FTKhits_directly; // option to skip over FTKRawHits and read in FTKHits directly 

  // SCTtrk input (only used when using SCT-first architecture)
  // These tracks are stored in FTKTrackStream in root output of track_fitter
  FTKTrackInput **m_trackinput;
  // These roads are stored in FTKRoadStream in root output of road_merger
  FTKRoadInput **m_roadinput;

  // Additional information for Naoki's studies
  int m_nao_nhits_tot;             // total number of hits per event
  int m_nao_nclus_tot;             // total number of hits per event

  int m_firstEventFTK;
  
protected:
  bool *m_goodRegions;
  bool m_regional;
  void processRegion(int, bool);

public:
  FTKDataInput();
  FTKDataInput(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused=0x0);
  virtual ~FTKDataInput();

  bool hasRegionalInput() const { return m_regional; }

  void setPlaneMaps(const FTKPlaneMap *pmap,const FTKPlaneMap *pmap_unused);



  virtual int addFile(const char*) {
    std::cerr << "Method not implemented";
    return -1;
  }

  virtual int addFilesList(const char *) {
    std::cerr << "Method not implemented";
    return -1;
  }

  virtual int addTrackFile(int, const char*) {
    std::cerr << "Method not implemented";
    return -1;
  }
  virtual int addTrackFilesList(int, const char *) {
    std::cerr << "Method not implemented";
    return -1;
  }

  virtual int addRoadFile(int, const char*) {
    std::cerr << "Method not implemented";
    return -1;
  }
  virtual int addRoadFilesList(int, const char *) {
    std::cerr << "Method not implemented";
    return -1;
  }

  virtual const char *getCurrentFileName()=0;

  int getCurrentRegion() const { return m_region; }
  FTKRoadInput* getRoadInput() { return m_roadinput[m_region]; }
  const FTKTrackInput* getTrackInput() const { return m_trackinput[m_region]; }

  void setClustering(bool v) { m_Clustering = v; }
  bool getClustering() const { return m_Clustering; }

  void setSaveClusterContent(bool v) { m_SaveClusterContent = v; }
  bool getSaveClusterContent() const { return m_SaveClusterContent; }

  void setDiagClustering(bool v) { m_DiagClustering = v; }
  bool getDiagClustering() const { return m_DiagClustering; }

  void setSctClustering(bool v) { m_SctClustering = v; }
  bool getSctClustering() const { return m_SctClustering; }

  void setPixelClusteringMode(unsigned int v) { m_PixelClusteringMode = v; }
  bool getPixelClusteringMode() const { return m_PixelClusteringMode; }

  void setIbl3DRealistic(bool v) { m_Ibl3DRealistic = v; }
  bool getIbl3DRealistic() const { return m_Ibl3DRealistic; }

  void setDuplicateGanged(bool v) { m_DuplicateGanged = v; }
  bool getDuplicateGanged() const { return m_DuplicateGanged; }

  void setGangedPatternRecognition(bool v) { m_GangedPatternRecognition = v; }
  bool getGangedPatternRecognition() const { return m_GangedPatternRecognition; }

  void setNRegions(int nregions) { m_nregions = nregions; }
  void setNSubRegions(int nsubregions) { m_nsubregions = nsubregions; } // for single muon mode
  void setNCoords(int ncoords) { m_ncoords = ncoords; }                 // for single muon mode
  
  void setSplitBlayerModules(bool v) { m_SplitBlayerModules = v; }
  bool getSplitBlayerModules() { return m_SplitBlayerModules; }

  void setSaveUnused(bool flag) { m_save_unused = flag; }
  bool getSaveUnused() const { return m_save_unused; }

  void setReadClusters(bool flag=true) { m_read_clusters = flag; }
  bool getReadClusters() const { return m_read_clusters; }

  void setReadFTKHits(bool flag) { m_read_FTKhits_directly = flag; }
  bool getReadFTKHits() { return m_read_FTKhits_directly; }

  void setFirstEvent(int firstEventFTK = -1) { m_firstEventFTK = firstEventFTK;}
  int getFirstEvent() const {return m_firstEventFTK;}

  virtual int init(bool *good=0x0)=0;
  virtual void initRoadsTracks() {;}
  virtual void useRoadsTracks(int, int) {;}

  unsigned int getNRawHits() const { return m_original_hits.size(); }
  void clearRawHits() { m_original_hits.clear(); }
  const std::vector<FTKRawHit>& getOriginalHits() const { return m_original_hits; };
  void initClustering();
  
  void clearTruthTrack() { m_truth_track.clear(); }
  const std::vector<FTKTruthTrack>& getTruthTrack() const { return m_truth_track; };
  

  virtual int readData()=0; // read the next event
  virtual void processEvent(bool clearOrig=true); // process the event

  virtual std::vector<FTKHit>& getHitsList()
  { return m_hits; }
  virtual std::vector<FTKHit>& getHitsListUnused() { return m_hits_unused;} // list of the ignored hits  

  virtual std::vector<FTKHit>& getRegionalHitsList(int reg)
  { return m_reghits[reg]; }

  unsigned int getNHits() const { return m_hits.size(); }
  unsigned long runNumber() const { return m_run_number; }
  unsigned long eventNumber() const { return m_event_number; }
  int averageInteractionsPerCrossing() const {return m_averageInteractionsPerCrossing;}
  int actualInteractionsPerCrossing() const {return m_actualInteractionsPerCrossing;}
  int LB() const {return m_LB;}
  int BCID() const {return m_BCID;}
  unsigned int extendedLevel1ID() const {return m_extendedLevel1ID;}
  unsigned int level1TriggerType() const {return m_level1TriggerType;}
  std::vector<unsigned int> level1TriggerInfo() const {return m_level1TriggerInfo;}

  void setRunNumber(const unsigned long& val) { m_run_number = val; }
  void setEventNumber(const unsigned long& val) { m_event_number = val; }
  void setaverageInteractionsPerCrossing(const int& val) {m_averageInteractionsPerCrossing = val;}
  void setactualInteractionsPerCrossing(const int& val) {m_actualInteractionsPerCrossing = val;}
  void setLB(const int&val) {m_LB = val;}
  void setBCID(const int&val) {m_BCID = val;}
  void setextendedLevel1ID(const unsigned int &val) {m_extendedLevel1ID = val;}
  void setlevel1TriggerType(const unsigned int &val) {m_level1TriggerType = val;}
  void setlevel1TriggerInfo(const std::vector<unsigned int> &val) {m_level1TriggerInfo = val;}

  void naoSetNhitsTot(int val) { m_nao_nhits_tot=val; }
  void naoSetNclusTot(int val) { m_nao_nclus_tot=val; }
  int naoGetNhitsTot() const { return m_nao_nhits_tot; }
  int naoGetNclusTot() const { return m_nao_nclus_tot; }
  
};
#endif // FTKDATAINPUT_H
