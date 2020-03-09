// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataWriteCnv/CLinksAODCnv_p1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief TP converter for CLinksAOD_p1.
 */


#ifndef DATAMODELTESTDATAWRITECNV_CLINKSAODCNV_P1_H
#define DATAMODELTESTDATAWRITECNV_CLINKSAODCNV_P1_H


#include "DataModelTestDataCommon/CLinksAOD.h"
#include "DataModelTestDataWriteCnv/CLinksAOD_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"


namespace DMTest {


class CLinksAODCnv_p1
  : public T_AthenaPoolTPCnvConstBase<CLinksAOD, CLinksAOD_p1>
{
public:
  using base_class::persToTrans;
  using base_class::transToPers;


  virtual void persToTrans (const CLinksAOD_p1* persObj,
                            CLinksAOD* transObj,
                            MsgStream &log) const override;

  virtual void transToPers (const CLinksAOD* transObj,
                            CLinksAOD_p1* persObj,
                            MsgStream &log) const override;


private:
  ElementLinkCnv_p3<ElementLink<CVec> > m_velCnv;
  ElementLinkVectorCnv_p1<ElementLinkVector<CVec> > m_elvCnv;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITECNV_CLINKSAODCNV_P1_H
