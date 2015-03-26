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

  int   nmdt;
  float mdtt[500];
  float mdtr[500];
  float mdtdr[500];
  float mdtx[500];
  float mdty[500];
  float mdtz[500];
  float mdtgx[500];
  float mdtgy[500];
  float mdtgz[500];
  int   mdtid[500];
  int   mdtpatid[500];
  int   mdtsegid[500];

  int   ntrig;
  float trigerr[500];
  float trigx[500];
  float trigy[500];
  float trigz[500];
  float triggx[500];
  float triggy[500];
  float triggz[500];
  int   trigid[500];
  int   trigpatid[500];
  int   trigsegid[500];

  int   nseg;
  int   segid[50];
  int   seggeo[50];
  float seggeotd[50];
  float seggeots[50];
  float seggeold[50];
  float seggeoft0y[50];
  float seggeoft0z[50];
  float seggeoft1y[50];
  float seggeoft1z[50];
  int   segid1[50];
  int   seggeo1[50];
  float seggeotd1[50];
  float seggeots1[50];
  float seggeold1[50];
  float seggeoft0y1[50];
  float seggeoft0z1[50];
  float seggeoft1y1[50];
  float seggeoft1z1[50];
  float segx[50];
  float segy[50];
  float segz[50];
  float seggx[50];
  float seggy[50];
  float seggz[50];
  float segdx[50];
  float segdy[50];
  float segdz[50];
  float segdgx[50];
  float segdgy[50];
  float segdgz[50];
  int   segpatid[50];

  int   npat;
  float patgx[10];
  float patgy[10];
  float patgz[10];
  float patdgx[10];
  float patdgy[10];
  float patdgz[10];  

  const MdtIdHelper* m_mdtIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const MuonGM::MuonDetectorManager* m_detMgr;
};


#endif
