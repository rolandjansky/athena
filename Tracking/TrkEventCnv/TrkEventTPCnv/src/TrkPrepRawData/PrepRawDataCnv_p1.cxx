/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include <algorithm>
#include "DataModel/tools/IdentContIndex.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void PrepRawDataCnv_p1::persToTrans( const Trk::PrepRawData_p1 *persObj, Trk::PrepRawData *transObj, MsgStream &log )
{
  bool id32 = false;
  if ((persObj->m_clusId & 0XFFFFFFFF00000000LL) == 0 &&
  (persObj->m_clusId & 0X00000000FFFFFFFFLL) != 0) {
    transObj->m_clusId = Identifier32(persObj->m_clusId);
    id32 = true;
  }
  else
    transObj->m_clusId = persObj->m_clusId; 
  fillTransFromPStore( &m_localPosCnv, persObj->m_localPos, &transObj->m_localPos,log );
    // Trk::ErrorMatrix* pMat = createTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, log );
    // size_t size = pMat->values.size();
    // const Amg::MatrixX* consttMat = transObj->m_localCovariance; 
    // Amg::MatrixX*& tMat = const_cast<Amg::MatrixX*&>(consttMat); // m_localCovariance is const.
    // if (size==1){
    //   tMat = new  Amg::MatrixX(1,1);
    //   (*tMat)(0,0) = static_cast<double>(pMat->values[0]);
    // }else if (size==3){
    //   (*tMat)(0,0) = static_cast<double>(pMat->values[0]);
    //   (*tMat)(0,1) = static_cast<double>(pMat->values[1]);      
    //   (*tMat)(1,0) = static_cast<double>(pMat->values[1]);      
    //   (*tMat)(1,1) = static_cast<double>(pMat->values[2]);
    // } else {
    //   log<<MSG::WARNING<<"PrepRawDataCnv_p1::persToTrans - unexpected size of cov matrix values: "<<size<<endreq;
    // }

  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
  Amg::MatrixX* tempMat = new Amg::MatrixX;
  EigenHelpers::vectorToEigenMatrix(dummy.values, *tempMat, "PrepRawDataCnv_p1");
  transObj->m_localCovariance = tempMat;
    // delete pMat;

  transObj->m_rdoList.resize( persObj->m_rdoList.size() );

  if (id32) {
    for (size_t i = 0; i < persObj->m_rdoList.size(); i++)
      transObj->m_rdoList[i] = Identifier32 (persObj->m_rdoList[i]);
  }
  else
    std::copy( persObj->m_rdoList.begin(), persObj->m_rdoList.end(), transObj->m_rdoList.begin() );

  transObj->m_indexAndHash.setHashAndIndex(persObj->m_indexAndHash);
}

void PrepRawDataCnv_p1::transToPers( const Trk::PrepRawData *transObj, Trk::PrepRawData_p1 *persObj, MsgStream & log )
{
  persObj->m_clusId = transObj->m_clusId.get_identifier32().get_compact(); 
  persObj->m_localPos = toPersistent( &m_localPosCnv, &transObj->m_localPos, log );

  if (transObj->m_localCovariance){
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, *transObj->m_localCovariance, "PrepRawDataCnv_p1");
    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
  }

  persObj->m_rdoList.resize( transObj->m_rdoList.size() );
  unsigned int i=0;
  for( std::vector<Identifier>::const_iterator it = transObj->m_rdoList.begin();
  it != transObj->m_rdoList.end();++it,++i) {
    persObj->m_rdoList[i] =it->get_identifier32().get_compact();
  }

  persObj->m_indexAndHash=transObj->m_indexAndHash.hashAndIndex();
}
