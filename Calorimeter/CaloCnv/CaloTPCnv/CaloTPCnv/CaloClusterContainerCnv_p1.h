//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOCLUSTERCONTAINERCNV_P1_H
#define CALOATHENAPOOL_CALOCLUSTERCONTAINERCNV_P1_H

#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "CaloTPCnv/CaloSamplingDataCnv_p1.h"
#include "CaloTPCnv/CaloClusterMomentStoreCnv_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p1.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterContainer_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthLinks/ElementLink.h"

class CaloClusterContainer;
class CaloCluster;



class CaloClusterContainerCnv_p1
  : public T_AthenaPoolTPCnvConstBase<CaloClusterContainer, CaloClusterContainer_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual
  void persToTrans (const CaloClusterContainer_p1* pers,
                           CaloClusterContainer* trans,
                           MsgStream &log) const override;


  virtual
  void transToPers (const CaloClusterContainer* trans,
                    CaloClusterContainer_p1* pers,
                    MsgStream &log) const override;


private:
  //Conversion function for individual clusters (called in a loop over the container)
  void persToTrans(const CaloClusterContainer_p1::CaloCluster_p*, CaloCluster*, MsgStream &) const;
  void transToPers(const CaloCluster*, CaloClusterContainer_p1::CaloCluster_p*, MsgStream &) const;


  //Sub-Converters:
  CaloTowerSegCnv_p1 m_caloTowerSegCnv;
  P4EEtaPhiMCnv_p1 m_P4EEtaPhiMCnv;
  CaloSamplingDataCnv_p1 m_caloSamplingDataCnv;
  CaloClusterMomentStoreCnv_p1  m_caloMomentStoreCnv;
  ElementLinkCnv_p1<ElementLink<CaloShowerContainer> > m_showerElementLinkCnv;
  ElementLinkCnv_p1<ElementLink<CaloCellLinkContainer> > m_cellElementLinkCnv;
};


#endif
