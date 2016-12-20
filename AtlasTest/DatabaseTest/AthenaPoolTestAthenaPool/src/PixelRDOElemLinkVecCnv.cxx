/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDOElemLinkVecCnv.h"


//Goes to .h file


PixelRDOElemLinkVecCnv::PixelRDOElemLinkVecCnv(ISvcLocator* svcLoc) : 
        PixelRDOElemLinkVecCnvBase(svcLoc),
        m_guid1("FE11B001-FF5B-4059-988C-92F93607ACBB")
{}


PixelRDOElemLinkVecPERS* PixelRDOElemLinkVecCnv::createPersistent(PixelRDOElemLinkVec* trans) {
    MsgStream log(msgSvc(), "PixelRDOElemLinkVecCnv");
    log << MSG::DEBUG << "Writing PixelRDOElemLinkVec_p1" << endmsg;
    PixelRDOElemLinkVecPERS* pers=new PixelRDOElemLinkVecPERS();
    m_converter_p1.transToPers(trans,pers,log); 
    return pers;
}
    
PixelRDOElemLinkVec* PixelRDOElemLinkVecCnv::createTransient() {
    MsgStream log(msgSvc(), "PixelRDOElemLinkVecCnv" );
    // Search for persistent version to convert - last version first
    PixelRDOElemLinkVec* trans = 0;
    if (compareClassGuid(m_guid1)) {
        trans=new PixelRDOElemLinkVec;
        log << MSG::DEBUG << "Read PixelRDOElemLinkVec_p1. GUID=" 
            << m_classID.toString() << endmsg;
        PixelRDOElemLinkVec_p1* pers=poolReadObject<PixelRDOElemLinkVec_p1>();
        m_converter_p1.persToTrans(pers,trans,log);
        delete pers;
        return trans;
    }
    else {
        log << MSG::ERROR << "Unsupported persistent version of PixelRDOElemLinkVec GUID=" 
            << m_classID.toString() << endmsg;
        throw std::runtime_error("Unsupported persistent version of PixelRDOElemLinkVec");
    }
    return trans;
}
