/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"
#include "PixelRDOElemLinkVecCnv_p1.h"


void PixelRDOElemLinkVecCnv_p1::persToTrans(const PixelRDOElemLinkVec_p1* pers, 
                                            PixelRDOElemLinkVec* trans, 
                                            MsgStream &log) {

    // reset element link converters, and provide container name lookup table
    m_elementLinkCnv.resetForCnv(pers->m_linkNames);

    trans->pixelRDOs().resize(pers->m_pixelRDOs.size());
    for (unsigned int i = 0; i < pers->m_pixelRDOs.size(); ++i) {
//         log << MSG::ERROR
//             << "persToTrans: index " <<   pers->m_pixelRDOs[i].m_contIndex << " " 
//             << pers->m_pixelRDOs[i].m_elementIndex 
//             << endreq;
      m_elementLinkCnv.persToTrans(&pers->m_pixelRDOs[i], &trans->pixelRDOs()[i], log);
    }
}


void PixelRDOElemLinkVecCnv_p1::transToPers(const PixelRDOElemLinkVec* trans, 
                                            PixelRDOElemLinkVec_p1* pers, 
                                            MsgStream &log)  {

    // reset element link converters, and provide container name lookup table
    m_elementLinkCnv.resetForCnv(pers->m_linkNames);

    pers->m_pixelRDOs.resize(trans->pixelRDOs().size());
    for (unsigned int i = 0; i < trans->pixelRDOs().size(); ++i) {
//         log << MSG::ERROR
//             << "transToPers: index " <<   MSG::hex 
//             << trans->m_pixelRDOs[i].index() << " " << MSG::dec
//             << endreq;
      m_elementLinkCnv.transToPers( &trans->pixelRDOs()[i], &pers->m_pixelRDOs[i], log);
    }
}




