/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_INDET_DICT_H
#define TRIG_INDET_DICT_H

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigHisto.h"
#include "TrigInDetEvent/TrigHisto1D.h"
#include "TrigInDetEvent/TrigHisto1DContainer.h"
#include "TrigInDetEvent/TrigHisto2D.h"
#include "TrigInDetEvent/TrigHisto2DContainer.h"
#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"
#include "TrigInDetEvent/TrigTrackCounts.h"
#include "TrigInDetEvent/TrigTrackCountsCollection.h"
#include "TrigInDetEvent/TrigVertexCounts.h"
#include "TrigInDetEvent/TrigVertexCountsCollection.h"

//namespace HLT::TrigInDetEventDict {
void dummyTriggerForTVCollTypedef(TrigVertexCollection a, 
				  DataVector<TrigInDetTrackCollection> b,
				  DataVector<TrigInDetTrackFitPar> c,
				  DataVector<TrigVertexCollection> d,
				  DataVector<TrigSpacePointCounts> e,
				  DataVector<TrigSpacePointCountsCollection> f,
				  DataVector<TrigTrackCounts> g,
				  DataVector<TrigTrackCountsCollection> h,
				  DataVector<TrigTauTracksInfo> i,
				  DataVector<TrigTauTracksInfoCollection> j,
				  DataVector<TrigTrtHitCounts> k,
				  DataVector<TrigTrtHitCountsCollection> l,
                                  DataVector<TrigHisto1D> m,
                                  DataVector<TrigHisto1DContainer> n,
                                  DataVector<TrigHisto2D> o,
                                  DataVector<TrigHisto2DContainer> p,
                                  DataVector<TrigVertexCounts> q,
                                  DataVector<TrigVertexCountsCollection> r) {
  TrigVertexCollection aa = a;
  DataVector<TrigInDetTrackCollection> bb = b; 
  DataVector<TrigInDetTrackFitPar> cc = c;
  DataVector<TrigVertexCollection> dd = d;
  DataVector<TrigSpacePointCounts> ee = e;
  DataVector<TrigSpacePointCountsCollection> ff = f;
  DataVector<TrigTrackCounts> gg = g;
  DataVector<TrigTrackCountsCollection> hh = h;
  DataVector<TrigTauTracksInfo> ii = i;
  DataVector<TrigTauTracksInfoCollection> jj = j;
  DataVector<TrigTrtHitCounts> kk = k;
  DataVector<TrigTrtHitCountsCollection> ll = l;
  DataVector<TrigHisto1D> mm = m; 
  DataVector<TrigHisto1DContainer> nn = n;
  DataVector<TrigHisto2D> oo = o;                                          
  DataVector<TrigHisto2DContainer> pp = p;
  DataVector<TrigVertexCounts> qq = q;
  DataVector<TrigVertexCountsCollection> rr = r;
}

#endif
