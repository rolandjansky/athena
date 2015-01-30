/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : CosmicMuon_p1
 *
 * @brief persistent partner for CosmicMuon
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuon_p1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_COSMICMUON_P1_H
#define TRIGCOSMICEVENTTPCNV_COSMICMUON_P1_H

#include "TrigCosmicEvent/CosmicMuon.h"

class CosmicMuon_p1 
{
  friend class CosmicMuonCnv_p1;

 public:
  
  CosmicMuon_p1() {}
  virtual ~CosmicMuon_p1(){}
  
 protected:

  float mP;
  float mRadius;
  float mTheta;
  float mPhi;
  float mPoint[3];
  float mT;
  int mIsIngoing;
  int mNRpcPairs;
  int mNTgcPairs;
  int mNMdtHits;
  int mNMdtSegs;

};

#endif
