/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABARCODECNV_P1_H
#define ATHENABARCODECNV_P1_H
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthenaKernel/IAthenaBarCode.h"


#include <DataModelAthenaPool/AthenaBarCode_p1.h>
#include <Navigation/AthenaBarCodeImpl.h>

class MsgStream;

class AthenaBarCodeCnv_p1 :
	public T_AthenaPoolTPCnvBase<AthenaBarCodeImpl, AthenaBarCode_p1>
{
public:
	AthenaBarCodeCnv_p1()
          : m_UUIDHash(0)
	{
	}
	
	virtual void persToTrans(const AthenaBarCode_p1* persObj,
			AthenaBarCodeImpl* transObj, MsgStream &) ;
	virtual void transToPers(const AthenaBarCodeImpl* transObj,
			AthenaBarCode_p1* persObj, MsgStream &) ;

  AthenaBarCode_t m_UUIDHash;
  
protected:


};

inline void AthenaBarCodeCnv_p1::transToPers(const AthenaBarCodeImpl* trans,
				      AthenaBarCode_p1* pers, MsgStream &) {
  pers->m_athenabarcode = trans->getAthenaBarCode();
  //  ATH_MSG_DEBUG("AthenaBarCodeCnv_p1::transToPers::pers->m_barcode="<<std::hex
  //		<<pers->m_athenabarcode);
}

inline void AthenaBarCodeCnv_p1::persToTrans(const AthenaBarCode_p1* pers,
				      AthenaBarCodeImpl* trans, MsgStream &) {

  static unsigned short msgCount(0);
  const int ABCMAXMSGCOUNT=10;

  if(pers->m_athenabarcode==IAthenaBarCode::UNDEFINEDBARCODE) {
    trans->setAthenaBarCode(IAthenaBarCode::UNDEFINEDBARCODE);
    //Maybe reading from an old version where Athenabarcode didn't exist
    if (msgCount<ABCMAXMSGCOUNT) {
      //      log << MSG::DEBUG << "AthenaBarCodeCnv_p1::persToTrans(): AthenaBarCode undefined in the persistent state. Maybe reading from an older version where AthenaBarCode didn't exist. A new AthenaBarCode has been assigned to this object: " <<std::hex<<trans->getAthenaBarCode()<<endreq;
      ++msgCount;
    } else if (msgCount==ABCMAXMSGCOUNT) {
      //      log << MSG::DEBUG << "AthenaBarCodeCnv_p1::persToTrans(): AthenaBarCode undefined in the persistent state. Maybe reading from an older version where AthenaBarCode didn't exist. A new AthenaBarCode has been assigned to this object: " <<std::hex<<trans->getAthenaBarCode()<<"suppressing further messages on this issue, please set message level to VERBOSE to see other messages."<<endreq;
      ++msgCount;
    } else {
      //      log << MSG::VERBOSE << "AthenaBarCodeCnv_p1::persToTrans(): AthenaBarCode undefined in the persistent state. Maybe reading from an older version where AthenaBarCode didn't exist. A new AthenaBarCode has been assigned to this object: " <<std::hex<<trans->getAthenaBarCode()<<endreq;
    }
  } else {
    trans->setAthenaBarCode(pers->m_athenabarcode);
    if (!(trans->hasUUIDHash())) {
      //      log << MSG::WARNING
      //	  << "AthenaBarCodeCnv_p1::persToTrans(): BarCode for Object "
      //	  <<trans->getAthenaBarCode()
      //	  <<" has no jobID hash, something wrong!"<<endreq;
    }
  }
  
  //  log<<MSG::VERBOSE<<"AthenaBarCodeCnv_p1::persToTrans::trans->getAthenaBarCode()="
  //<<std::hex<<trans->getAthenaBarCode()<<endreq;
}

#endif

