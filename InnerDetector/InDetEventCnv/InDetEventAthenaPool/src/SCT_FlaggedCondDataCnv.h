/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_FlaggedCondDataCnv.h
 * @brief Header file which defines a typedef for templated converter class
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 */

#ifndef SCT_FlaggedCondDataCnv_H
#define SCT_FlaggedCondDataCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "SCT_ConditionsData/SCT_FlaggedCondData.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetEventAthenaPool/SCT_FlaggedCondData_p1.h"
#include "SCT_FlaggedCondDataCnv_p1.h"

typedef  SCT_FlaggedCondData_p1 SCT_FlaggedCondData_PERS;
typedef  T_AthenaPoolCustomCnv<SCT_FlaggedCondData, SCT_FlaggedCondData_PERS> SCT_FlaggedCondDataCnvBase;

class SCT_FlaggedCondDataCnv : public SCT_FlaggedCondDataCnvBase {
friend class CnvFactory<SCT_FlaggedCondDataCnv>;
public:
  SCT_FlaggedCondDataCnv(ISvcLocator* svcloc) : SCT_FlaggedCondDataCnvBase(svcloc) {}
protected:
  virtual SCT_FlaggedCondData_PERS* createPersistent(SCT_FlaggedCondData* transCont);
  virtual SCT_FlaggedCondData* createTransient();

  SCT_FlaggedCondDataCnv_p1 m_TPConverter;
};

#endif // SCT_FlaggedCondDataCnv_H
