//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOTOWERCONTAINERCNV_P1_H
#define CALOTPCNV_CALOTOWERCONTAINERCNV_P1_H

//std::vector<unsigned int> m_caloRegions;
//#include <algorithm>
#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "CaloTPCnv/CaloTowerContainer_p1.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class CaloTowerContainer;
class CaloTowerContainer_p1;

class CaloTowerContainerCnv_p1
  : public T_AthenaPoolTPCnvConstBase<CaloTowerContainer, CaloTowerContainer_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual
  void persToTrans (const CaloTowerContainer_p1* pers,
                    CaloTowerContainer* trans,
                    MsgStream& log) const override;


  virtual
  void transToPers (const CaloTowerContainer* trans,
                    CaloTowerContainer_p1* pers,
                    MsgStream& log) const override;


private:
  CaloTowerSegCnv_p1 m_caloTowerSegCnv;
};


#endif
