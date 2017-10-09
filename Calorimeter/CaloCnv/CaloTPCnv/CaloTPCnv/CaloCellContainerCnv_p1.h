//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCELLCONTAINERCNV_P1_H
#define CALOTPCNV_CALOCELLCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaPoolCnvSvc/ITPConverter.h"
#include <memory>

class CaloCellContainer;
class CaloCompactCellContainer;

class CaloCellContainerCnv_p1
  : public ITPCnvBase
{
public:
  CaloCellContainerCnv_p1();

  virtual ~CaloCellContainerCnv_p1() {}
  virtual void persToTrans(const CaloCompactCellContainer*, CaloCellContainer*) ;
  virtual void transToPers(const CaloCellContainer*, CaloCompactCellContainer*) ;

  /** Convert persistent object representation to transient
      @param pers [IN] void* pointer to the persistent object
      @param trans [OUT] void* pointer to the empty transient object
      @param log [IN] output message stream
  */
  virtual void persToTransUntyped(const void* pers, void* trans,
                                  MsgStream& log);

  /** Convert transient object representation to persistent
      @param trans [IN] void* pointer to the transient object
      @param pers [OUT] void* pointer to the empty persistent object
      @param log [IN] output message stream
  */  
  virtual void transToPersUntyped(const void* trans, void* pers,
                                  MsgStream& log);

  /** return C++ type id of the transient class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& transientTInfo() const;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& persistentTInfo() const;

};


#endif
