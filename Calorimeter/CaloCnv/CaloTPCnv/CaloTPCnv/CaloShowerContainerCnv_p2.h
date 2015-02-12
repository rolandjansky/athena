//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOSHOWERCONTAINERCNV_P2_H
#define CALOTPCNV_CALOSHOWERCONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "CaloTPCnv/CaloClusterMomentContainerCnv_p1.h"
#include "CaloTPCnv/CaloSamplingDataContainerCnv_p1.h"
#include "AthenaPoolCnvSvc/ITPConverter.h"
#include "GaudiKernel/MsgStream.h"

class CaloShowerContainer;
class CaloShowerContainer_p2;

class CaloShowerContainerCnv_p2
  : public ITPCnvBase
{
public:
  CaloShowerContainerCnv_p2();

  void persToTrans(const CaloShowerContainer_p2*, CaloShowerContainer*, MsgStream &);
  void transToPers(const CaloShowerContainer*, CaloShowerContainer_p2*, MsgStream &);

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

 private:
  CaloClusterMomentContainerCnv_p1 m_momentContainerCnv;
  CaloSamplingDataContainerCnv_p1 m_samplingContainerCnv;

};


#endif
