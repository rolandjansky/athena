/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeature_p1
 *
 * @brief persistent partner for TrigMuonClusterFeature
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeature_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURE_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURE_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonClusterFeature_p1 {
  friend class TrigMuonClusterFeatureCnv_p1;

 public:

  TrigMuonClusterFeature_p1() {}
  virtual ~TrigMuonClusterFeature_p1(){}

  //private:

  float m_eta;
  float m_phi;
  int m_nroi;
  int m_njet;
  int m_ntrk;


};

#endif

