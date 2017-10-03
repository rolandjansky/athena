/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ZdcEventTPCnv/ZdcRawChannelCollectionCnv_p1.h"
#include "ZdcRawChannelCollectionCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

ZdcRawChannelCollection_PERS* ZdcRawChannelCollectionCnv::createPersistent(ZdcRawChannelCollection* transCont) {
    MsgStream mlog(msgSvc(), "ZdcRawChannelCollectionConverter" );
    ZdcRawChannelCollectionCnv_p1   converter;
    ZdcRawChannelCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}

ZdcRawChannelCollection* ZdcRawChannelCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "ZdcRawChannelCollectionConverter" );
    ZdcRawChannelCollectionCnv_p1   converter_p1;

    ZdcRawChannelCollection       *trans_cont(0);

    static const pool::Guid   p1_guid("A2BDA733-7A0A-459D-9237-33DFF3DC9982");
    if( this->compareClassGuid(p1_guid)) {
      std::auto_ptr< ZdcRawChannelCollection_p1 >   cont( this->poolReadObject< ZdcRawChannelCollection_p1 >() );
      trans_cont = converter_p1.createTransient( cont.get(), mlog );
    }
    else {
      throw std::runtime_error("Unsupported persistent version of Data collection");
    }
    return trans_cont;
}
