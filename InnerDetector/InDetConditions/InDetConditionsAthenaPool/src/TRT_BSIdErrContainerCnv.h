/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file TRT_BSIdErrContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef TRT_BSIdErrContainerCnv_H
#define TRT_BSIdErrContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetConditionsAthenaPool/TRT_BSIdErrContainer_p1.h"
#include "TRT_BSIdErrContainerCnv_p1.h"

// the latest persistent representation type of DataCollection:
typedef  TRT_BSIdErrContainer_p1  TRT_BSIdErrContainer_PERS;
typedef  T_AthenaPoolCustomCnv<TRT_BSIdErrContainer, TRT_BSIdErrContainer_PERS >   TRT_BSIdErrContainerCnvBase;

class TRT_BSIdErrContainerCnv : public TRT_BSIdErrContainerCnvBase {
friend class CnvFactory<TRT_BSIdErrContainerCnv >;
protected:
  TRT_BSIdErrContainerCnv (ISvcLocator* svcloc) : TRT_BSIdErrContainerCnvBase(svcloc) {}
  virtual TRT_BSIdErrContainer_PERS*   createPersistent (TRT_BSIdErrContainer* transCont);
  virtual TRT_BSIdErrContainer*       createTransient ();

  TRT_BSIdErrContainerCnv_p1  m_TPConverter;
};
#endif


