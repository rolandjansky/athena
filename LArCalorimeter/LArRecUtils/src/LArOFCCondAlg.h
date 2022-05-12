/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

  /**
   * AlgoTool to compute OFC on fly
   *

 - OFC are computed consistently with autocorrelation 
   (by default (NMinBias=-1), these OFC are retrieved)

 - possibility to retrieve OFC for a wanted luminosity (set by NMinBias, 
   the number of minimum bias events per bunch-crossing)
   but they won't be consistent with autocorrelation anymore.
   slower since re-do calculation

 - flag FromDB switches between 
     "take OFC stored in DataBase" 
    and 
     "take OFC stored in datamembers (computed consistently from 
      LArAutoCorrTotal) or recomputed for a wanted luminosity"

   *
   */
#ifndef LAROFCCONDALG_H
#define LAROFCCONDALG_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArPedestal.h"

#include "LArRawConditions/LArAutoCorrTotal.h"
#include "LArRawConditions/LArOFC.h"

#include "LArCabling/LArOnOffIdMapping.h"

class LArOFCCondAlg : public AthAlgorithm {
public:
  LArOFCCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~LArOFCCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_LArOnOffIdMappingObjKey;
  SG::ReadCondHandleKey<ILArShape> m_LArShapeObjKey;
  SG::ReadCondHandleKey<ILArNoise> m_LArNoiseObjKey;
  SG::ReadCondHandleKey<ILArPedestal> m_LArPedestalObjKey;
  SG::ReadCondHandleKey<LArAutoCorrTotal> m_LArAutoCorrTotalObjKey;

  SG::WriteCondHandleKey<LArOFC> m_LArOFCObjKey;

  float m_Nminbias;
  bool m_isMC;
  bool m_isSuperCell;
  unsigned int m_firstSample;
  bool m_useHighestGainAutoCorr;

  unsigned int findTheNSamples(ILArShape::ShapeRef_t Shape,
                           unsigned int nsamples_AC_OFC,
                                           unsigned int nsamples_shape) const;

  bool m_Dump;

  size_t m_nGains = 0UL;
};

#endif
