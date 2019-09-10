// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamFractionContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 */

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_SCT_ByteStreamFractionContainerCnv_H
#define INDETBYTESTREAMERRORSATHENAPOOL_SCT_ByteStreamFractionContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetByteStreamErrorsAthenaPool/SCT_ByteStreamFractionContainer_p1.h"
#include "SCT_ByteStreamFractionContainerCnv_p1.h"

/** the latest persistent representation type of DataCollection: */
typedef  SCT_ByteStreamFractionContainer_p1 SCT_ByteStreamFractionContainer_PERS;
typedef  T_AthenaPoolCustomCnv<SCT_ByteStreamFractionContainer, SCT_ByteStreamFractionContainer_PERS> SCT_ByteStreamFractionContainerCnvBase;

class SCT_ByteStreamFractionContainerCnv : public SCT_ByteStreamFractionContainerCnvBase {
friend class CnvFactory<SCT_ByteStreamFractionContainerCnv>;
public:
  SCT_ByteStreamFractionContainerCnv (ISvcLocator* svcloc) : SCT_ByteStreamFractionContainerCnvBase(svcloc) {}
protected:
  virtual SCT_ByteStreamFractionContainer_PERS*createPersistent(SCT_ByteStreamFractionContainer* transCont);
  virtual SCT_ByteStreamFractionContainer* createTransient();

  SCT_ByteStreamFractionContainerCnv_p1  m_TPConverter;
};

#endif // INDETBYTESTREAMERRORSATHENAPOOL_SCT_ByteStreamFractionContainerCnv_H
