/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENTTPCNV_TRIGINDETEVENTTPCNVDICT_H
#define TRIGINDETEVENTTPCNV_TRIGINDETEVENTTPCNVDICT_H

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_tlp1.h"

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p2.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p2.h"


#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp4.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_p1.h"

#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p4.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p5.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p3.h"

#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainer_tlp1.h"

#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainer_tlp1.h"

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1_old_cnv.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_tlp1.h"

#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_tlp1.h"

#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p2.h"

#include "TrigInDetEventTPCnv/TrigTrackCounts_p1.h"   
#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_p1.h"    
#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_tlp1.h" 

#include "TrigInDetEventTPCnv/TrigVertexCounts_p1.h"   
#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_p1.h"    
#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_tlp1.h" 

#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertex_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp2.h"
#include "TrigInDetEventTPCnv/TrigVertex_p2.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigInDetEventTPCnvDict
{

  std::vector<TrigTauTracksInfo_p1>           m_dummyTrigTauTracksInfo_p1;
  std::vector<TrigTauTracksInfo_p2>           m_dummyTrigTauTracksInfo_p2;
  std::vector<TrigInDetTrackCollection_p1>    m_dummyTrigInDetTrackCollection_p1;
  std::vector<TrigInDetTrack_p1>              m_dummyTrigInDetTrack_p1;
  std::vector<TrigInDetTrack_p2>              m_dummyTrigInDetTrack_p2;
  std::vector<TrigInDetTrack_p3>              m_dummyTrigInDetTrack_p3;
  std::vector<TrigInDetTrack_p4>              m_dummyTrigInDetTrack_p4;
  std::vector<TrigInDetTrack_p5>              m_dummyTrigInDetTrack_p5;
  std::vector<TrigInDetTrackFitPar_p1>        m_dummyTrigInDetTrackFitPar_p1;
  std::vector<TrigInDetTrackFitPar_p2>        m_dummyTrigInDetTrackFitPar_p2;
  std::vector<TrigInDetTrackFitPar_p3>        m_dummyTrigInDetTrackFitPar_p3;
  std::vector<TrigHisto1D_p1>                 m_dummyTrigHisto1D_p1;
  std::vector<TrigHisto2D_p1>                 m_dummyTrigHisto2D_p1;
  std::vector<TrigSpacePointCounts_p1>        m_dummySpacePointCounts_p1;
  std::vector<TrigSpacePointCounts_p2>        m_dummySpacePointCounts_p2;
  std::vector<TrigSpacePointCounts_p3>        m_dummySpacePointCounts_p3;
  std::vector<TrigSpacePointCounts_p4>        m_dummySpacePointCounts_p4;
  std::vector<TrigTrtHitCounts_p1>            m_dummyTrtHitCounts_p1;
  std::vector<TrigTrtHitCounts_p2>            m_dummyTrtHitCounts_p2;
  std::vector<TrigTrackCounts_p1>             m_dummyTrackCounts_p1;
  std::vector<TrigTrackCounts_p2>             m_dummyTrackCounts_p2;
  std::vector<TrigVertexCounts_p1>            m_dummyVertexCounts_p1;
  std::vector<TrigVertex_p1>                  m_dummyVertex_p1;
  std::vector<TrigVertex_p2>                  m_dummyVertex_p2;

};

#endif
