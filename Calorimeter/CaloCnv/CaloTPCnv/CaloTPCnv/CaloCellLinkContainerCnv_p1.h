//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCELLLINKCONTAINERCNV_P1_H
#define CALOTPCNV_CALOCELLLINKCONTAINERCNV_P1_H

#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloCellLinkContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Navigation/Navigable.h"


class CaloCellLinkContainerCnv_p1
  : public T_AthenaPoolTPCnvWithKeyBase<CaloCellLinkContainer, CaloCellLinkContainer_p1>
{
public:
  using base_class::transToPersWithKey;
  using base_class::persToTransWithKey;

  virtual
  void persToTransWithKey (const CaloCellLinkContainer_p1*,
                           CaloCellLinkContainer*,
                           const std::string& key,
                           MsgStream &log) const override;

  virtual
  void transToPersWithKey (const CaloCellLinkContainer*,
                           CaloCellLinkContainer_p1*,
                           const std::string& key,
                           MsgStream &log) const override;


private:
  NavigableCnv_p1< Navigable<CaloCellContainer,double>,float > m_converter;
};


#endif
