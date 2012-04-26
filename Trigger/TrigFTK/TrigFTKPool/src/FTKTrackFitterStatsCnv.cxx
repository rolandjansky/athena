/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FTKTrackFitterStatsCnv.h"

FTKTrackFitterStats_p1* FTKTrackFitterStatsCnv::createPersistent(FTKTrackFitterStats *transObj) {
  MsgStream log(messageService(),"FTKTrackFitterStatsCnv");
  FTKTrackFitterStats_p1 *persObj = m_TPconverter.createPersistent(transObj,log);
  return persObj;
}



FTKTrackFitterStats* FTKTrackFitterStatsCnv::createTransient() {
  static pool::Guid p1_guid("09056832-AA09-9426-A2A8-7FC5ED7BFAA7");

  if (compareClassGuid(p1_guid)) {
    MsgStream log(messageService(),"FTKTrackFitterStatsCnv");
    std::auto_ptr<FTKTrackFitterStats_p1> cont_p1(this->poolReadObject<FTKTrackFitterStats_p1>());
    return m_TPconverter.createTransient(cont_p1.get(),log);
  }

  return 0x0;
}
