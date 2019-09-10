// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBSErrContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_InDetBSErrContainerCnv_H
#define INDETBYTESTREAMERRORSATHENAPOOL_InDetBSErrContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetByteStreamErrorsAthenaPool/InDetBSErrContainer_p1.h"
#include "InDetBSErrContainerCnv_p1.h"
#include "InDetBSErrContainerCnv_p2.h"

/** the latest persistent representation type of DataCollection: */
typedef  InDetBSErrContainer_p2  InDetBSErrContainer_PERS;
typedef  T_AthenaPoolCustomCnv<InDetBSErrContainer, InDetBSErrContainer_PERS >   InDetBSErrContainerCnvBase;

class InDetBSErrContainerCnv : public InDetBSErrContainerCnvBase {
public:
  InDetBSErrContainerCnv (ISvcLocator* svcloc) : InDetBSErrContainerCnvBase(svcloc) {}
  
protected:
  virtual InDetBSErrContainer_PERS*   createPersistent (InDetBSErrContainer* transCont);
  virtual InDetBSErrContainer*       createTransient ();

  InDetBSErrContainerCnv_p1  m_TPConverter_p1;
  InDetBSErrContainerCnv_p2  m_TPConverter_PERS; // The latest one should be PERS. Currently p2 is PERS.
};
#endif
