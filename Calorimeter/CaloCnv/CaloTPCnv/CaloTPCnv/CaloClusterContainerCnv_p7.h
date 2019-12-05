//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CaloClusterContainerCnv_p7_H
#define CALOTPCNV_CaloClusterContainerCnv_p7_H

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloSamplingData.h"
#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p2.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterContainer_p7.h"

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/AthenaBarCodeCnv_p1.h"
#include "AthLinks/ElementLink.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class CaloClusterContainer;
class CaloCluster;

class CaloClusterContainerCnv_p7
  : public T_AthenaPoolTPCnvConstBase<CaloClusterContainer, CaloClusterContainer_p7>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual
  void persToTrans (const CaloClusterContainer_p7* pers,
                    CaloClusterContainer* trans,
                    MsgStream &log) const override;


  virtual
  void transToPers (const CaloClusterContainer* trans,
                    CaloClusterContainer_p7* pers,
                    MsgStream &log) const override;


private:
  friend class CaloClusterContainerCnvTest_p7;
  //Conversion function for individual clusters (called in a loop over the container)
  void persToTrans(const CaloClusterContainer_p7::CaloCluster_p*, CaloCluster*, MsgStream &) const;
  void transToPers(const CaloCluster*, CaloClusterContainer_p7::CaloCluster_p*, MsgStream &) const;

  //Sub-Converters:
  CaloTowerSegCnv_p1                                     m_caloTowerSegCnv;
  P4EEtaPhiMCnv_p2                                       m_P4EEtaPhiMCnv;
  ElementLinkCnv_p3<ElementLink<CaloShowerContainer> >   m_showerElementLinkCnv;
  ElementLinkCnv_p3<ElementLink<CaloCellLinkContainer> > m_cellElementLinkCnv;
  AthenaBarCodeCnv_p1                                    m_barcodeCnv;
};


#endif
