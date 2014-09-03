// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellLinkContainerCnv_p2.h,v 1.4 2008-02-24 21:14:31 ssnyder Exp $
/* @file  CaloCellLinkContainerCnv_p2.h
 * @author Ilija Vukotic <ivukotic@cern.ch>, scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief T/P conversions for CaloCellLinkContainerCnv_p2.
 */

#ifndef CALOTPCNV_CALOCELLLINKCONTAINERCNV_P2_H
#define CALOTPCNV_CALOCELLLINKCONTAINERCNV_P2_H

#include "AthenaKernel/ITPCnvBase.h"
#define private public
#include "CaloEvent/CaloCellLinkContainer.h"
#undef private
#include "CaloTPCnv/CaloCellLinkContainer_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


/**
 * @brief T/P conversions for CaloCellLinkContainerCnv_p2.
 */
class CaloCellLinkContainerCnv_p2
  : public ITPCnvBase
{
public:
  /**
   * @brief Convert from persistent to transient object.
   * @param pers The persistent object to convert.
   * @param trans The transient object to which to convert.
   * @param log Error logging stream.
   */
  void persToTrans(const CaloCellLinkContainer_p2* pers,
                   CaloCellLinkContainer* trans,
                   MsgStream& log);


  /**
   * @brief Convert from transient to persistent object.
   * @param trans The transient object to convert.
   * @param pers The persistent object to which to convert.
   * @param log Error logging stream.
   */
  void transToPers(const CaloCellLinkContainer* trans,
                   CaloCellLinkContainer_p2* pers,
                   MsgStream &log);


  /**
   * @brief Convert from persistent to transient object, with untyped pointers.
   * @param pers The persistent object to convert.
   * @param trans The transient object to which to convert.
   * @param log Error logging stream.
   */
  virtual void persToTransUntyped(const void* pers,
                                  void* trans,
                                  MsgStream& log);


  /**
   * @brief Convert from transient to persistent object, with untyped pointers.
   * @param trans The transient object to convert.
   * @param pers The persistent object to which to convert.
   * @param log Error logging stream.
   */
  virtual void transToPersUntyped(const void* trans,
                                  void* pers,
                                  MsgStream& log);


  /**
   * @brief Return the @c std::type_info for the transient type.
   */
  virtual const std::type_info& transientTInfo() const;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& persistentTInfo() const;

};


#endif
