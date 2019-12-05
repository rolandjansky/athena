//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCLUSTERCONTAINERCNV_P3_H
#define CALOTPCNV_CALOCLUSTERCONTAINERCNV_P3_H

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloSamplingData.h"
#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
//#include "CaloTPCnv/CaloClusterMomentContainerCnv_p2.h"
#include "CaloTPCnv/CaloSamplingDataContainerCnv_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p1.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterContainer_p3.h"

#include "DataModelAthenaPool/ElementLinkCnv_p2.h"
#include "AthLinks/ElementLink.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class CaloClusterContainer;
class CaloCluster;

class CaloClusterContainerCnv_p3
  : public T_AthenaPoolTPCnvConstBase<CaloClusterContainer, CaloClusterContainer_p3>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual
  void persToTrans (const CaloClusterContainer_p3* pers,
                    CaloClusterContainer* trans,
                    MsgStream &log) const override;


  virtual
  void transToPers (const CaloClusterContainer* trans,
                    CaloClusterContainer_p3* pers,
                    MsgStream &log) const override;


private:
  typedef ElementLinkCnv_p2<ElementLink<CaloShowerContainer> >::State
    ShowerLinkState;
  typedef ElementLinkCnv_p2<ElementLink<CaloCellLinkContainer> >::State
    CellLinkState;

  //Conversion function for individual clusters (called in a loop over the container)
  void persToTrans (const CaloClusterContainer_p3::CaloCluster_p* pers,
                    CaloCluster* trans,
                    ShowerLinkState& showerLinkState,
                    CellLinkState& cellLinkState,
                    MsgStream &) const;
  void transToPers (const CaloCluster* trans,
                    CaloClusterContainer_p3::CaloCluster_p* pers,
                    ShowerLinkState& showerLinkState,
                    CellLinkState& cellLinkState,
                    MsgStream &) const;

  //Sub-Converters:
  CaloTowerSegCnv_p1                                     m_caloTowerSegCnv;
  P4EEtaPhiMCnv_p1                                       m_P4EEtaPhiMCnv;
//  CaloClusterMomentContainerCnv_p2                       m_momentContainerCnv;
  CaloSamplingDataContainerCnv_p1                        m_samplingDataContainerCnv;
  ElementLinkCnv_p2<ElementLink<CaloShowerContainer> >   m_showerElementLinkCnv;
  ElementLinkCnv_p2<ElementLink<CaloCellLinkContainer> > m_cellElementLinkCnv;
};


#endif
