/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeature_p3
 *
 * @brief persistent partner for IsoMuonFeature
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @modified by Fabrizio Salvatore <p.salvatore@cern.ch> - U of Sussex
 +
 * File and Version Information:
 * $Id: IsoMuonFeature_p3.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURE_P3_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURE_P3_H

//#include "AthenaPoolUtilities/TPObjRef.h"

class IsoMuonFeature_p3 {
  friend class IsoMuonFeatureCnv_p3;

 public:

  IsoMuonFeature_p3() : m_flag(0), m_RoiIdMu(0) {}
  ~IsoMuonFeature_p3(){}

  //private:

  float m_allTheFloats[14];
  int   m_flag;
  int   m_RoiIdMu;

};

#endif

