/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeature_p1
 *
 * @brief persistent partner for IsoMuonFeature
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeature_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURE_P1_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURE_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

class IsoMuonFeature_p1 {
  friend class IsoMuonFeatureCnv_p1;

 public:

  IsoMuonFeature_p1() :  m_EtInnerConeEC(0.0), m_EtOuterConeEC(0.0), m_EtInnerConeHC(0.0), m_EtOuterConeHC(0.0), 
			 m_NTracksCone(0), m_SumPtTracksCone(0.0), m_PtMuTracksCone(0.0), m_LAr_w(0.0),
			 m_Tile_w(0.0), m_RoiIdMu(0), m_PtMu(0.0), m_EtaMu(0.0), m_PhiMu(0.0) {}
  virtual ~IsoMuonFeature_p1(){}

  //private:

  float m_EtInnerConeEC;
  float m_EtOuterConeEC;
  float m_EtInnerConeHC;
  float m_EtOuterConeHC;
  int   m_NTracksCone;
  float m_SumPtTracksCone;
  float m_PtMuTracksCone;
  float m_LAr_w;
  float m_Tile_w;
  int   m_RoiIdMu;
  float m_PtMu;
  float m_EtaMu;
  float m_PhiMu;

};

#endif

