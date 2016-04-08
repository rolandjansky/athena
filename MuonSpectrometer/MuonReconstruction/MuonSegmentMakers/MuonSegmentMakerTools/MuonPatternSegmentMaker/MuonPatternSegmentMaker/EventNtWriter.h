/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTNTWRITER_H
#define EVENTNTWRITER_H

#include "TFile.h"
#include "TTree.h"
#include <string>
#include <vector>

#include "MuonPatternSegmentMaker/MuonPatternSegmentMaker.h"

class MdtIdHelper;
class TgcIdHelper;
class RpcIdHelper;


namespace MuonGM {
  class MuonDetectorManager;
}

class EventNtWriter {
 public:
  EventNtWriter(std::string name, const MdtIdHelper* mdtId = 0, const RpcIdHelper* rpcId = 0, 
		const TgcIdHelper* tgcId = 0, const MuonGM::MuonDetectorManager* detMgr = 0);
  void endEvent();
  void close();
  void fill( Muon::MuonPatternSegmentMaker::ROTsPerRegion& rots, const Muon::MuonPatternCombination* pattern, bool hasPhi );
    
 private:

  TFile* m_file;
  TTree* m_tree;

  int   m_nmdt;
  float m_mdtt[500];
  float m_mdtr[500];
  float m_mdtdr[500];
  float m_mdtx[500];
  float m_mdty[500];
  float m_mdtz[500];
  float m_mdtgx[500];
  float m_mdtgy[500];
  float m_mdtgz[500];
  int   m_mdtid[500];
  int   m_mdtpatid[500];
  int   m_mdtsegid[500];

  int   m_ntrig;
  float m_trigerr[500];
  float m_trigx[500];
  float m_trigy[500];
  float m_trigz[500];
  float m_triggx[500];
  float m_triggy[500];
  float m_triggz[500];
  int   m_trigid[500];
  int   m_trigpatid[500];
  int   m_trigsegid[500];

  int   m_nseg;
  int   m_segid[50];
  int   m_seggeo[50];
  float m_seggeotd[50];
  float m_seggeots[50];
  float m_seggeold[50];
  float m_seggeoft0y[50];
  float m_seggeoft0z[50];
  float m_seggeoft1y[50];
  float m_seggeoft1z[50];
  int   m_segid1[50];
  int   m_seggeo1[50];
  float m_seggeotd1[50];
  float m_seggeots1[50];
  float m_seggeold1[50];
  float m_seggeoft0y1[50];
  float m_seggeoft0z1[50];
  float m_seggeoft1y1[50];
  float m_seggeoft1z1[50];
  float m_segx[50];
  float m_segy[50];
  float m_segz[50];
  float m_seggx[50];
  float m_seggy[50];
  float m_seggz[50];
  float m_segdx[50];
  float m_segdy[50];
  float m_segdz[50];
  float m_segdgx[50];
  float m_segdgy[50];
  float m_segdgz[50];
  int   m_segpatid[50];

  int   m_npat;
  float m_patgx[10];
  float m_patgy[10];
  float m_patgz[10];
  float m_patdgx[10];
  float m_patdgy[10];
  float m_patdgz[10];  

  const MdtIdHelper* m_mdtIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const MuonGM::MuonDetectorManager* m_detMgr;
};


#endif
