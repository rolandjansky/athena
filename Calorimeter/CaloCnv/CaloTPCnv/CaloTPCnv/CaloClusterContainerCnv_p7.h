//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CaloClusterContainerCnv_p7_H
#define CALOTPCNV_CaloClusterContainerCnv_p7_H

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloSamplingData.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p2.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterContainer_p7.h"

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/AthenaBarCodeCnv_p1.h"
#include "AthLinks/ElementLink.h"
#include "AthenaPoolCnvSvc/ITPConverter.h"

class CaloClusterContainer;
class CaloCluster;

class CaloClusterContainerCnv_p7 : public ITPCnvBase {
public:
  typedef CaloClusterContainer Trans_t;
  typedef CaloClusterContainer_p7 Pers_t;


  CaloClusterContainerCnv_p7() {};
  virtual ~CaloClusterContainerCnv_p7() {}; 

  // Methods for invoking conversions on objects given by generic pointers.
  virtual void persToTransUntyped(const void* pers,
                                  void* trans,
                                  MsgStream& log) override;
  virtual void transToPersUntyped(const void* trans,
                                  void* pers,
                                  MsgStream& log) override;
  virtual const std::type_info& transientTInfo() const override;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& persistentTInfo() const override;

  void persToTrans(const CaloClusterContainer_p7*, CaloClusterContainer*, MsgStream &log) const;
  void transToPers(const CaloClusterContainer*, CaloClusterContainer_p7*, MsgStream &log) const;

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
