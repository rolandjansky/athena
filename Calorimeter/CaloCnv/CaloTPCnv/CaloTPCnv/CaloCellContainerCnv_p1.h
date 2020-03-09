//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCELLCONTAINERCNV_P1_H
#define CALOTPCNV_CALOCELLCONTAINERCNV_P1_H

#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include <memory>

class CaloCellContainer;
class CaloCompactCellContainer;

class CaloCellContainerCnv_p1
  : public T_AthenaPoolTPCnvWithKeyBase<CaloCellContainer, CaloCompactCellContainer>
{
public:
  using base_class::transToPersWithKey;
  using base_class::persToTransWithKey;

  CaloCellContainerCnv_p1();


  virtual
  void persToTransWithKey (const CaloCompactCellContainer*,
                           CaloCellContainer*,
                           const std::string& key,
                           MsgStream& log) const override;

  virtual
  void transToPersWithKey (const CaloCellContainer*,
                           CaloCompactCellContainer*,
                           const std::string& key,
                           MsgStream& log) const override;
};


#endif
