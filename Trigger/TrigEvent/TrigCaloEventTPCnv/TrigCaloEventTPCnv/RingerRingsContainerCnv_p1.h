/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsContainerCnv_p1
 *
 * @brief transient persistent converter for RingerRingsContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id: RingerRingsContainerCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "TrigCaloEventTPCnv/RingerRingsContainer_p1.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p1.h"
 
class RingerRingsContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv<RingerRingsContainer, RingerRingsContainer_p1, RingerRingsCnv_p1> {
 
 public:

  RingerRingsContainerCnv_p1() { }

  // This part is implemented in T_AthenaPoolTPConverter.h. 
  // It is here temporarily to override and allow some extra debugging
  virtual void persToTrans(const RingerRingsContainer_p1 *persVect,
                           RingerRingsContainer *transVect,
			   MsgStream &log) {
    log << MSG::DEBUG << "RingerRingsContainerCnv::persToTrans" << endreq;
    if (!persVect) {
      log << MSG::WARNING << "RingerRingsContainerCnv::persToTrans cannot convert NULL persVect" << endreq;
      return;
    }
    if (!transVect) {
      log << MSG::WARNING << "RingerRingsContainerCnv::persToTrans cannot convert to NULL transVect" << endreq;
      return;
    }
    
    transVect->clear();
    transVect->reserve(persVect->size());
    
    // convert vector entries one by one
    for (RingerRingsContainer_p1::const_iterator it = persVect->begin();
         it != persVect->end();  ++it) {
      RingerRings *p = createTransFromPStore(&m_elementCnv, *it, log); 
      transVect->push_back(p);
      if (!p)
        log << MSG::WARNING << "RingerRingsContainerCnv::persToTrans failed for an element " << endreq;
    }
  }
  

  virtual void transToPers(const RingerRingsContainer *transVect,
                           RingerRingsContainer_p1 *persVect,
			   MsgStream &log) {
    if (!persVect) {
      log << MSG::WARNING << "RingerRingsContainerCnv::transToPers cannot convert NULL persVect" << endreq;
      return;
    }
    if (!transVect) {
      log << MSG::WARNING << "RingerRingsContainerCnv::transToPers cannot convert to NULL transVect" << endreq;
      return;
    }
    
    persVect->clear();
    persVect->reserve(transVect->size());
    // convert vector entries one by one
    for (RingerRingsContainer::const_iterator it = transVect->begin();
	 it != transVect->end(); ++it ) {
      TPObjRef a = toPersistent(&m_elementCnv, *it, log);
      persVect->push_back(a);
      if (a.isNull()) {
        log << MSG::WARNING << "RingerRingsContainerCnv::transToPers failed for an element " << *it << " "
	    << m_elementCnv << endreq;
      }
    } 
  }
};

#endif

