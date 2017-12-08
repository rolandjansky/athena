/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file TRT_BSErrContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef TRT_BSErrContainerCnv_H
#define TRT_BSErrContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "InDetByteStreamErrors/TRT_BSErrContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetConditionsAthenaPool/TRT_BSErrContainer_p1.h"
#include "TRT_BSErrContainerCnv_p1.h"

// the latest persistent representation type of DataCollection:
typedef  TRT_BSErrContainer_p1  TRT_BSErrContainer_PERS;
typedef  T_AthenaPoolCustomCnv<TRT_BSErrContainer, TRT_BSErrContainer_PERS >   TRT_BSErrContainerCnvBase;

class TRT_BSErrContainerCnv : public TRT_BSErrContainerCnvBase {
friend class CnvFactory<TRT_BSErrContainerCnv >;
protected:
  TRT_BSErrContainerCnv (ISvcLocator* svcloc) : TRT_BSErrContainerCnvBase(svcloc) {}
  virtual TRT_BSErrContainer_PERS*   createPersistent (TRT_BSErrContainer* transCont);
  virtual TRT_BSErrContainer*       createTransient ();

  TRT_BSErrContainerCnv_p1  m_TPConverter;
};
#endif


