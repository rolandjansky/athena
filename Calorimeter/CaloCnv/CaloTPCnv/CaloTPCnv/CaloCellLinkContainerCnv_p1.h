//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCELLLINKCONTAINERCNV_P1_H
#define CALOTPCNV_CALOCELLLINKCONTAINERCNV_P1_H

#include "AthenaKernel/ITPCnvBase.h"
//#include "CaloEvent/CaloCellLinkContainer.h"
//#include "CaloTPCnv/CaloCellLinkContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Navigation/Navigable.h"

class CaloCellLinkContainer_p1;
class CaloCellLinkContainer;

class CaloCellLinkContainerCnv_p1
  : public ITPCnvBase
{
public:
  void persToTrans(const CaloCellLinkContainer_p1*, CaloCellLinkContainer*, MsgStream &log) ;
  void transToPers(const CaloCellLinkContainer*, CaloCellLinkContainer_p1*, MsgStream &log) ;

  virtual void persToTransUntyped(const void* pers,
                                  void* trans,
                                  MsgStream& log);
  virtual void transToPersUntyped(const void* trans,
                                  void* pers,
                                  MsgStream& log);
  virtual const std::type_info& transientTInfo() const;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& persistentTInfo() const;

private:
  NavigableCnv_p1< Navigable<CaloCellContainer,double>,float > m_converter;

};


#endif
