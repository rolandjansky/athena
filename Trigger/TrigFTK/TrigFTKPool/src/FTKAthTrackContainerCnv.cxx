/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FTKAthTrackContainerCnv.h"

FTKAthTrackContainer_p1* FTKAthTrackContainerCnv::createPersistent(FTKAthTrackContainer *transObj) {
  MsgStream log(msgSvc(),"FTKAthTrackContainerCnv");
  FTKAthTrackContainer_p1 *persObj = m_TPconverter.createPersistent(transObj,log);
  return persObj;
}



FTKAthTrackContainer* FTKAthTrackContainerCnv::createTransient() {
  static pool::Guid p1_guid("94372D90-D05F-44B0-8E12-2E795342EAFC");

  if (compareClassGuid(p1_guid)) {
    MsgStream log(msgSvc(),"FTKAthTrackContainerCnv");
    std::auto_ptr<FTKAthTrackContainer_p1> cont_p1(this->poolReadObject<FTKAthTrackContainer_p1>());
    return m_TPconverter.createTransient(cont_p1.get(),log);
  }

  return 0x0;
}
