// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellLinkContainerCnv_p2.h,v 1.4 2008-02-24 21:14:31 ssnyder Exp $
/* @file  CaloCellLinkContainerCnv_p2.h
 * @author Ilija Vukotic <ivukotic@cern.ch>, scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief T/P conversions for CaloCellLinkContainerCnv_p2.
 */

#ifndef CALOTPCNV_CALOCELLLINKCONTAINERCNV_P2_H
#define CALOTPCNV_CALOCELLLINKCONTAINERCNV_P2_H

#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloCellLinkContainer_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


/**
 * @brief T/P conversions for CaloCellLinkContainerCnv_p2.
 */
class CaloCellLinkContainerCnv_p2
  : public T_AthenaPoolTPCnvWithKeyBase<CaloCellLinkContainer, CaloCellLinkContainer_p2>
{
public:
  using base_class::transToPersWithKey;
  using base_class::persToTransWithKey;


  /**
   * @brief Convert from persistent to transient object.
   * @param pers The persistent object to convert.
   * @param trans The transient object to which to convert.
   * @param log Error logging stream.
   */
  virtual
  void persToTransWithKey (const CaloCellLinkContainer_p2* pers,
                           CaloCellLinkContainer* trans,
                           const std::string& key,
                           MsgStream& log) const override;


  /**
   * @brief Convert from transient to persistent object.
   * @param trans The transient object to convert.
   * @param pers The persistent object to which to convert.
   * @param log Error logging stream.
   */
  virtual
  void transToPersWithKey (const CaloCellLinkContainer* trans,
                           CaloCellLinkContainer_p2* pers,
                           const std::string& key,
                           MsgStream &log) const override;
};


#endif
