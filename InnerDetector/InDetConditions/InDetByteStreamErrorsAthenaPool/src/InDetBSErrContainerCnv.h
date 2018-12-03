/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

/** the latest persistent representation type of DataCollection: */
typedef  InDetBSErrContainer_p1  InDetBSErrContainer_PERS;
typedef  T_AthenaPoolCustomCnv<InDetBSErrContainer, InDetBSErrContainer_PERS >   InDetBSErrContainerCnvBase;

class InDetBSErrContainerCnv : public InDetBSErrContainerCnvBase {
friend class CnvFactory<InDetBSErrContainerCnv >;
protected:
  InDetBSErrContainerCnv (ISvcLocator* svcloc) : InDetBSErrContainerCnvBase(svcloc) {}
  virtual InDetBSErrContainer_PERS*   createPersistent (InDetBSErrContainer* transCont);
  virtual InDetBSErrContainer*       createTransient ();

  InDetBSErrContainerCnv_p1  m_TPConverter;
};
#endif
