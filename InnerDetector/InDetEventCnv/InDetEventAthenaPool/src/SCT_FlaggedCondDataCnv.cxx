/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_FlaggedCondDataCnv.cxx
 * @brief Generated implementation file which includes header files needed by SCT_FlaggedCondDataCnv
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 */

#include "SCT_FlaggedCondDataCnv.h"

#include <memory>

SCT_FlaggedCondData_PERS*
SCT_FlaggedCondDataCnv::createPersistent(SCT_FlaggedCondData* transCont) {
  MsgStream log(msgSvc(), "SCT_FlaggedCondDataCnv");
  SCT_FlaggedCondDataCnv_p1 cnv;
  std::unique_ptr<SCT_FlaggedCondData_PERS> persObj = std::make_unique<SCT_FlaggedCondData_PERS>();
  cnv.transToPers(transCont, persObj.get(), log);
  return persObj.release(); 
}

SCT_FlaggedCondData*
SCT_FlaggedCondDataCnv::createTransient() {
  MsgStream log(msgSvc(), "SCT_FlaggedCondDataCnv");
  const pool::Guid p1_guid("0C10E502-F02B-440B-9E8F-6A8C31915D7C");
  if (compareClassGuid(p1_guid)) {
    /** using auto_ptr ensures deletion of the persistent object */
    std::auto_ptr<SCT_FlaggedCondData_p1> col_vect( poolReadObject< SCT_FlaggedCondData_p1 >() );
    SCT_FlaggedCondDataCnv_p1 cnv;
    std::unique_ptr<SCT_FlaggedCondData> trasnObj = std::make_unique<SCT_FlaggedCondData>();
    cnv.persToTrans(col_vect.get(), trasnObj.get(), log);
    return trasnObj.release();
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
