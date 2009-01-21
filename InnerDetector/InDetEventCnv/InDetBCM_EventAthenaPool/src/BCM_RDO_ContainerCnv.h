/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Pool converter for BCM_RDO_Container
// Author: Dominique Tardif <Dominique.Tardif@cern.ch>

#ifndef INDETBCM_EVENTATHENAPOOL_BCM_RDO_CONTAINERCNV_H
#define INDETBCM_EVENTATHENAPOOL_BCM_RDO_CONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/CnvFactory.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_EventAthenaPool/BCM_RDO_Container_p0.h"
#include "BCM_RDO_ContainerCnv_p0.h"

class BCM_RDO_ContainerCnv : public T_AthenaPoolCustomCnv<BCM_RDO_Container,BCM_RDO_Container_p0> {

  friend class CnvFactory<BCM_RDO_ContainerCnv>;

 protected:
  BCM_RDO_ContainerCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv<BCM_RDO_Container, BCM_RDO_Container_p0>(svcloc) {}
  virtual BCM_RDO_Container_p0* createPersistent(BCM_RDO_Container* transCont);
  virtual BCM_RDO_Container* createTransient();

  BCM_RDO_ContainerCnv_p0 m_Converter;
};

#endif
