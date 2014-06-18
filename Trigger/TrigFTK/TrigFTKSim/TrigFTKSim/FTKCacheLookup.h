/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  This file implements cached versions of region, sector, and track lookups.
  It can be used both for creating and reading lookup files (in root format)
  It is designed to be included directly into C code.
*/

#ifndef FTK_CACHE_LOOKUP
#define FTK_CACHE_LOOKUP

// exported functions
#ifdef __cplusplus
extern "C" {
#endif
  // user is expected to initialize the lookups she wants to use
  void loadRegionLookup(char *);
  void loadSectorLookup(char *);
  void loadTrackLookup(char *);
  void makeRegionLookup(char *);
  void makeSectorLookup(char *);
  void makeTrackLookup(char *);
  void flushLookups();
  // get region from lookup for a given event; return -1 if failed
  // if FORCE_REGION was supplied, this will return the forced value!
  int lookupRegion(int);
  void lookupForceRegion(int);
  // get sector from lookup for a given event; return -1 if failed
  int lookupSector(int);
  // check if given event has an 8L track
  int hasTrack(int);
  // get phi, cott, curv into supplied variables. Function returns -1 if failed
  int lookupPhiCottCurv(int,double*,double*,double*);
  // setters:
  void lookupAddRegion(int,int);
  void lookupAddSector(int,int);
  void lookupAddTrack(int,float,float,float);

#ifdef __cplusplus
}
#endif

// underlying C++ implementation
#ifdef __cplusplus

#define FTK_LOOKUP_CACHE_SIZE 30000

#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class lookup_container_int {
public:
  int evt;
  int data;
  lookup_container_int() {};
  lookup_container_int(int _evt, int _data) {
    evt=_evt;
    data=_data;
  }
  lookup_container_int& operator=(const lookup_container_int& v) {
    evt=v.evt;
    data=v.data;
    return *this;
  }
  friend std::ostream& operator<<(std::ostream&,const lookup_container_int&);
  friend std::istream& operator>>(std::istream&,lookup_container_int&);
};

class lookup_container_float3 {
public:
  int evt;
  float phi;
  float cott;
  float curv;
  lookup_container_float3() {};
  lookup_container_float3(int _evt, float _phi, float _cott, float _curv) {
    evt=_evt;
    phi=_phi;
    cott=_cott;
    curv=_curv;
  }
  lookup_container_float3& operator=(const lookup_container_float3& v) {
    evt=v.evt;
    phi=v.phi;
    cott=v.cott;
    curv=v.curv;
    return *this;
  }
  friend std::ostream& operator<<(std::ostream&,const lookup_container_float3&);
  friend std::istream& operator>>(std::istream&,lookup_container_float3&);
};

template <typename TDATA>
class FTKCacheLookup {
 protected:
  enum MODE { READ, WRITE };
  // caching layer
  std::map<int,TDATA> m_lookup;
  int m_maxevt;
  const int m_cachesize;
  // IO
  TDATA m_data;
  MODE m_mode;
  TTree* m_t;
  int m_entry;
  int m_nentries;
  TFile* m_fout;
  // private functions
  virtual void setupTree()=0;
 public:
  // dummy constructor
  FTKCacheLookup() : m_cachesize(FTK_LOOKUP_CACHE_SIZE) {
    m_t=0;
    m_entry=0;
    m_nentries=0;
    m_fout=0;
    m_maxevt=-1;
  }
  // virtual destructor
  virtual ~FTKCacheLookup() {
    delete m_fout;
  };
  // open file functions
  int open_for_read(const char *fname) {
    m_mode=READ;
    m_fout = 0;
    TChain *t = new TChain("tree","FTK lookup tree");
    const int st=t->Add(fname);
    m_t=t;
    setupTree();
    m_nentries=m_t->GetEntries();
    std::cout << "Opened file " << fname << " with " << m_nentries << " events (status=" << st << ")" << std::endl;
    return m_nentries;
  }
  int open_for_write(const char *fname) {
    m_mode=WRITE;
    m_fout = new TFile(fname,"RECREATE"); m_fout->cd();
    std::cout << "Created file " << fname << std::endl;
    m_fout->SetCompressionLevel(5);
    m_t = new TTree("tree","FTK lookup tree");
    setupTree();
    return m_fout->IsOpen();
  }
  // call in the end to flush output tree
  void done() {
    if(m_mode==WRITE) {
      TFile *fout = m_t->GetCurrentFile();
      std::cout << "Saving TTree into this file: " << fout->GetName() << std::endl;
      fout->Write();
      fout->Close();
    }
  }
  // writes current m_data into the output tree
  void fill() {
    m_t->Fill();
    m_entry++;
  }
  // refill the cache from a TTree
  void refill() {
    m_lookup.clear();
    if(m_entry>=m_nentries) return;
    int m_entry_new = m_entry+m_cachesize > m_nentries ? m_nentries : m_entry+m_cachesize;
    for(int i=m_entry;i<m_entry_new; i++) {
      m_t->GetEntry(i);
      m_lookup[m_data.evt] = m_data;
    }
    m_maxevt = m_data.evt;
    std::cout << "Loading entries [" << m_entry << ".." << m_entry_new << "]; max event = "
	      << m_maxevt << std::endl;
    m_entry=m_entry_new;
  }
  // check if there is an entry for given event
  int check(int evt) {
    if(evt>m_maxevt) refill();
    if(m_lookup.find(evt)==m_lookup.end()) return -1;
    return 0;
  }
  // accessor for current event
  int get_current_evt() {
    return m_data.evt;
  }
  // cache-less access
  int next_cacheless() {
    if(m_entry>=m_nentries) return -1;
    m_t->GetEntry(m_entry++);
    return 0;
  }
  // text file output [aka convert root format to text format]
  void dump_to_textfile(const char *fname) {
    std::ofstream a(fname);
    for(int i=0;i<m_t->GetEntries();i++) {
      m_t->GetEntry(i);
      a << m_data << std::endl;
    }
    a.close();
  }
  // text file input [aka convert text format to root format]
  void load_from_textfile(const char *fname) {
    std::ifstream a(fname);
    while (a >> m_data) {
      fill();
    }
    a.close();
  }
};

class FTKIntLookup : public FTKCacheLookup<lookup_container_int> {
protected:
  void setupTree() {
    if(m_mode==WRITE) 
      m_t->Branch("data",&m_data.evt,"evt/I:data/I");
    if(m_mode==READ)
      m_t->SetBranchAddress("data",&m_data);
  }
public:
  // -1 signifies failure
  int get(int evt) {
    if(evt>m_maxevt) refill();
    if(m_lookup.find(evt)==m_lookup.end()) return -1;
    return m_lookup[evt].data;
  }
  void set(int evt, int data) {
    m_data.evt=evt;
    m_data.data=data;
    fill();
  }
  int get_current_int() {
    return m_data.data;
  }

};

class FTKTrackLookup : public FTKCacheLookup<lookup_container_float3> {
protected:
  void setupTree() {
    if(m_mode==WRITE) 
      m_t->Branch("data",&m_data.evt,"evt/I:phi/F:cott/F:curv/F");
    if(m_mode==READ)
      m_t->SetBranchAddress("data",&m_data);
  }
public:
  int get(int evt, float &phi, float &cott, float &curv) {
    if(evt>m_maxevt) refill();
    if(m_lookup.find(evt)==m_lookup.end()) return -1;
    phi=m_lookup[evt].phi;
    cott=m_lookup[evt].cott;
    curv=m_lookup[evt].curv;
    return 0;
  }
  void set(int evt, float phi, float cott, float curv) {
    m_data.evt=evt;
    m_data.phi=phi;
    m_data.cott=cott;
    m_data.curv=curv;
    fill();
  }
};

#endif // __cplusplus

#endif // FTK_CACHE_LOOKUP
