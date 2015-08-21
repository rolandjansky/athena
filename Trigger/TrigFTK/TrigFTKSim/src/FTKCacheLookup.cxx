/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKCacheLookup.h"
#include <iostream>
#include <cstdlib>

static FTKIntLookup *m_region_lookup(0);
static FTKIntLookup *m_sector_lookup(0);
static FTKTrackLookup *m_track_lookup(0);
static int m_force_region(-1);

std::ostream& operator<<(std::ostream &out, const lookup_container_int &data) {
  out << data.evt << "\t" << data.data;
  return out;
}
std::istream& operator>>(std::istream &out, lookup_container_int &data) {
  out >> data.evt >> data.data;
  return out;
}

std::ostream& operator<<(std::ostream &out, const lookup_container_float3 &data) {
  std::streamsize oldprec = out.precision();
  out << data.evt << "\t";
  out.precision(10);
  out << data.phi << "\t" << data.cott << "\t" << data.curv;
  out.precision(oldprec);
  out << std::flush;
  return out;
}
std::istream& operator>>(std::istream &out, lookup_container_float3 &data) {
  out >> data.evt >> data.phi >> data.cott >> data.curv;
  return out;
}

void makeRegionLookup(char *path) {
  delete m_region_lookup;
  m_region_lookup = new FTKIntLookup();
  if(!m_region_lookup->open_for_write(path)) {
    std::cerr << "ERROR: failed to create file " << path << std::endl;
    exit(0);
  }
}

void makeSectorLookup(char *path) {
  delete m_sector_lookup;
  m_sector_lookup = new FTKIntLookup();
  if(!m_sector_lookup->open_for_write(path)) {
    std::cerr << "ERROR: failed to create file " << path << std::endl;
    exit(0);
  }
}

void makeTrackLookup(char *path) {
  delete m_track_lookup;
  m_track_lookup = new FTKTrackLookup();
  if(!m_track_lookup->open_for_write(path)) {
    std::cerr << "ERROR: failed to create file " << path << std::endl;
    exit(0);
  }
}

void loadRegionLookup(char *path) {
  delete m_region_lookup;
  m_region_lookup = new FTKIntLookup();
  if(!m_region_lookup->open_for_read(path)) {
    std::cerr << "ERROR: no files matched pattern " << path << std::endl;
    exit(0);
  }
}

void loadSectorLookup(char *path) {
  delete m_sector_lookup;
  m_sector_lookup = new FTKIntLookup();
  if(!m_sector_lookup->open_for_read(path)) {
    std::cerr << "ERROR: no files matched pattern " << path << std::endl;
    exit(0);
  }
}

void loadTrackLookup(char *path) {
  delete m_track_lookup;
  m_track_lookup = new FTKTrackLookup();
  if(!m_track_lookup->open_for_read(path)) {
    std::cerr << "ERROR: no files matched pattern " << path << std::endl;
    exit(0);
  }
}

void lookupForceRegion(int region) {
  m_force_region=region;
}
int lookupRegion(int evt) {
  return m_force_region==-1 ? m_region_lookup->get(evt) : m_force_region;
}

int lookupSector(int evt) {
  return m_sector_lookup->get(evt);
}

int hasTrack(int evt) {
  return m_track_lookup->check(evt);
}

int lookupPhiCottCurv(int evt,double *phi,double *cott,double *curv) {
  float tmpphi(0),tmpcott(0),tmpcurv(0);
  int result=m_track_lookup->get(evt,tmpphi,tmpcott,tmpcurv);
  *phi=tmpphi;
  *cott=tmpcott;
  *curv=tmpcurv;
  return result;
}

void lookupAddRegion(int evt, int data) {
  m_region_lookup->set(evt,data);
}
void lookupAddSector(int evt, int data) {
  m_sector_lookup->set(evt,data);
}
void lookupAddTrack(int evt,float phi,float cott,float curv) {
  m_track_lookup->set(evt,phi,cott,curv);
}

void flushLookups() {
  if(m_region_lookup) m_region_lookup->done();
  if(m_sector_lookup) m_sector_lookup->done();
  if(m_track_lookup) m_track_lookup->done();
}
