/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROADSTREAM_H
#define FTKROADSTREAM_H

#include "ftkdefs.h"
#include "FTKRoad.h"
#include "FTKSS.h"
#include "FTKRoadKDTree.h"

#include <TClonesArray.h>

#include <vector>
#include <algorithm>
#include <list>
#include <map>
#include <utility>


typedef std::map<int,FTKSS> FTKSS_map_t;
typedef std::vector<FTKSS_map_t> FTKSS_container_t;
typedef std::map<int, FTKSS_map_t> FTKUnusedSS_map_t;


class FTKRoadStream  {
private:
  unsigned long m_run_number; // run number
  unsigned long m_event_number; // event number

  int m_nroads; // number of roads in m_roads TClonesArray
  int m_nroads_4L; // 4-Layer SCTTrk roads
  TClonesArray *m_roads; //-> clones array of roads

  int m_nroads_tot; // totoal number of found roads
  int m_nroads_good; // total number of accepted roads
  int m_nroads_maj; // total number of found majority-roads
  int m_nroads_maj_good; // total number of accepted majority-roads
  
  std::vector< std::map<int,FTKSS> > m_ssmaps; // hits, divided by plane
  std::map< int, std::map<int,FTKSS> > m_ssmaps_unused; // unused hits, divided by plane

  std::map< std::pair<int,int>, int > m_roadIdMap; //! Not saved on disk. 
  // Used to optimize the findRoad function.
  
  // Additional information for Naoki's studies
 private:
  int m_nao_nhits_tot;             // total number of hits per event
  int m_nao_nclus_tot;             // total number of hits per event
  std::vector<int> m_nao_nclus;    // number of clusters routed into current region [nplanes]
  std::vector<int> m_nao_nclus_road;    // number of clusters routed into current region [nplanes] in roads, allowing for duplicates
  std::vector<int> m_nao_nss;      // number of superstrips going into AM [nplanes]
  int m_nao_nroads_am;             // number of roads out of AM
  int m_nao_nroads_am_complete;             // number of roads out of AM, complete no missing hits
  int m_nao_nroads_am_misspix;             // number of roads out of AM, missing at least one pixel hit
  int m_nao_nroads_am_misssct;             // number of roads out of AM, missing at least one sct hit
  int m_nao_nroads_mod;            // number of roads (extra counter for split arch)
  int m_nao_nroads_rw;             // number of roads after roadwarrior
 public:
  void naoSetNhitsTot(int val) { m_nao_nhits_tot=val; }
  void naoSetNclusTot(int val) { m_nao_nclus_tot=val; }
  void naoSetNclus(const std::vector<int>& val) { m_nao_nclus=val; }
  void naoSetNclus_road(const std::vector<int>& val) { m_nao_nclus_road=val; }
  void naoSetNss(const std::vector<int>& val) { m_nao_nss=val; }
  void naoAddNclus(const std::vector<int>& val) {
    assert(val.size()==m_nao_nclus.size());
    std::transform(val.begin(),val.end(),m_nao_nclus.begin(),m_nao_nclus.begin(),std::plus<int>()); 
  }
  void naoAddNclus_road(const std::vector<int>& val) {
    assert(val.size()==m_nao_nclus_road.size());
    std::transform(val.begin(),val.end(),m_nao_nclus_road.begin(),m_nao_nclus_road.begin(),std::plus<int>()); 
  }
  void naoAddNss(const std::vector<int>& val) {
    assert(val.size()==m_nao_nss.size());
    std::transform(val.begin(),val.end(),m_nao_nss.begin(),m_nao_nss.begin(),std::plus<int>()); 
  }
  void naoSetNroadsAM(int val) { m_nao_nroads_am=val; }
  void naoSetNroadsAMComplete(int val) { m_nao_nroads_am_complete=val; }
  void naoSetNroadsAMMissPix(int val) { m_nao_nroads_am_misspix=val; }
  void naoSetNroadsAMMissSCT(int val) { m_nao_nroads_am_misssct=val; }
  void naoSetNroadsMOD(int val) { m_nao_nroads_mod=val; }
  void naoSetNroadsRW(int val) { m_nao_nroads_rw=val; }
  void naoAddNroadsAM(int val) { m_nao_nroads_am+=val; }
  void naoAddNroadsAMComplete(int val) { m_nao_nroads_am_complete+=val; }
  void naoAddNroadsAMMissPix(int val) { m_nao_nroads_am_misspix+=val; }
  void naoAddNroadsAMMissSCT(int val) { m_nao_nroads_am_misssct+=val; }
  void naoAddNroadsMOD(int val) { m_nao_nroads_mod+=val; }
  void naoAddNroadsRW(int val) { m_nao_nroads_rw+=val; }

  int naoGetNhitsTot() const { return m_nao_nhits_tot; }
  int naoGetNclusTot() const { return m_nao_nclus_tot; }
  int naoGetNclus(unsigned int pl) const { return m_nao_nclus.size()>pl ? m_nao_nclus[pl] : -1; }
  int naoGetNclus_road(unsigned int pl) const { return m_nao_nclus_road.size()>pl ? m_nao_nclus_road[pl] : -1; }
  int naoGetNss(unsigned int pl) const { return m_nao_nss.size()>pl ? m_nao_nss[pl] : -1; }
  const std::vector<int>& naoGetNclus() { return m_nao_nclus; }
  const std::vector<int>& naoGetNclus_road() { return m_nao_nclus_road; }
  const std::vector<int>& naoGetNss() { return m_nao_nss; }
  int naoGetNroadsAM() const { return m_nao_nroads_am; }
  int naoGetNroadsAMComplete() const { return m_nao_nroads_am_complete; }
  int naoGetNroadsAMMissPix() const { return m_nao_nroads_am_misspix; }
  int naoGetNroadsAMMissSCT() const { return m_nao_nroads_am_misssct; }
  int naoGetNroadsMOD() const { return m_nao_nroads_mod; }
  int naoGetNroadsRW() const { return m_nao_nroads_rw; }


  long int *m_evtidx; //!
  long int *m_barcode; //!
  float *m_frac; //!
  
public:
  FTKRoadStream();
  virtual ~FTKRoadStream();

  void init(int);

  unsigned long runNumber() const { return m_run_number; }
  unsigned long eventNumber() const { return m_event_number; }
  void setRunNumber(const unsigned long& val) { m_run_number = val; }
  void setEventNumber(const unsigned long& val) { m_event_number = val; }

  int getNPlanes() const { return m_ssmaps.size(); }

  void clear();

  FTKRoad* addRoad(const FTKRoad&);
  void inc4LRoad(const int& val=1) { m_nroads_4L+=val; } // Increment 4L road counter
  int getN4LRoads() const { return m_nroads_4L; } // Get 4L roads

  int getNRoads() const { return m_nroads; }

  void sortRoads();

  void addSS(int,const int,const FTKSS&);
  FTKSS& getSS(int pl, int id) { return m_ssmaps[pl][id]; }
  void addSSPlane(int pl,const std::map<int,FTKSS>& res) { m_ssmaps[pl] = res; }
  const std::map<int,FTKSS>& getSSPlane(int pl) const { return m_ssmaps[pl]; }
  const FTKSS_container_t& getSSContainer() const { return m_ssmaps; }

  void addUnusedSS(int,const int,const FTKSS&);
  void addUnusedSSMap(const std::map< int, std::map<int,FTKSS> >& ssmap) { m_ssmaps_unused = ssmap; }
  const FTKUnusedSS_map_t& getUnusedSSMap() const { return m_ssmaps_unused; }
  const FTKSS& getUnusedSS(int pl, int id) const;
  FTKRoad *getRoad(int) const;
  int findRoad(int,int);
  void removeRoad(int);
  void optimize();
  void buildRoadMap();

  int getNSS(int pl) const { return m_ssmaps[pl].size(); }

  void attachHits(int);
  void detachHits(int);
  
  void Print(bool printCoords=false);

  FTKRoadKDTree *buildKDTree();

  int computeUnusedSegmentsTruthList();
  int getNUnusedPlanes() const { return m_ssmaps_unused.size(); }
  long int getEventIndex(int idx) const { return m_evtidx[idx];}
  long int getBarcode(int idx ) const { return m_barcode[idx]; }
  float getBarcodeFrac(int idx) const { return m_frac[idx]; }


  /** this class helps to loop over the roads, its goal is 
      to simplify the method used to loop on complex output
      made using the variable resolution strips. In this case
      this iterator is alway on an usable road, properly 
      prepared to be used in track_fitter algorithms */
  class FTKRoadCursor : public TObject {
  public:
    FTKRoadCursor();
    FTKRoadCursor(FTKRoadStream*, FTKRoad*,int,bool);
    FTKRoadCursor(const FTKRoadCursor&);
    ~FTKRoadCursor();
    
    FTKRoad* operator()();
    FTKRoad* getRoad() { return (*this)(); }
    
  private:
    FTKRoadStream *m_stream; // streamer object
    FTKRoad *m_road; // managed road
    int m_position; // position in the streamer class
    bool m_first; //c true if the road is
    
  };

private:
  std::list<FTKRoadCursor> m_fit_list; //! list of the road to fit
  std::list<FTKRoadCursor>::iterator m_fit_iter; //! current road

  void expandSubRoads(FTKRoad*,FTKRoad*,int,bool);
public:  


  unsigned prepareFitList(); // make the list end return the begin  
  FTKRoad* fetchRoad(); // get the current road and point to the next
  FTKRoad* fetchFirstRoad(); // get the first road and point to the it
  void rewindFitList() { m_fit_iter = m_fit_list.begin(); }

  ClassDef(FTKRoadStream,12)
};

#endif // FTKROADSTREAM_H
