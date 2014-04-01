/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutputContainerCnv_p1
 *
 * @brief transient persistent converter for TrigRNNOutputContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_CNV_P1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigRNNOutputContainer.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p1.h"
 
class TrigRNNOutputContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv<TrigRNNOutputContainer, TrigRNNOutputContainer_p1, ITPConverterFor<TrigRNNOutput> > {
 
 public:

  TrigRNNOutputContainerCnv_p1() { }

  // This part is implemented in T_AthenaPoolTPConverter.h. 
  // It is here temporarily to override and allow some extra debugging
  virtual void persToTrans(const TrigRNNOutputContainer_p1 *persVect,
                           TrigRNNOutputContainer *transVect,
			   MsgStream &log) {
    log << MSG::DEBUG << "TrigRNNOutputContainerCnv::persToTrans" << endreq;
    if (!persVect) {
      log << MSG::WARNING << "TrigRNNOutputContainerCnv::persToTrans cannot convert NULL persVect" << endreq;
      return;
    }
    if (!transVect) {
      log << MSG::WARNING << "TrigRNNOutputContainerCnv::persToTrans cannot convert to NULL transVect" << endreq;
      return;
    }
    
    transVect->clear();
    transVect->reserve(persVect->size());
    
    // convert vector entries one by one
    for (TrigRNNOutputContainer_p1::const_iterator it = persVect->begin();
         it != persVect->end();  ++it) {
      TrigRNNOutput *p = createTransFromPStore(&m_elementCnv, *it, log); 
      transVect->push_back(p);
      if (!p)
        log << MSG::WARNING << "TrigRNNOutputContainerCnv::persToTrans failed for an element " << endreq;
    }
  }
  

  virtual void transToPers(const TrigRNNOutputContainer *transVect,
                           TrigRNNOutputContainer_p1 *persVect,
			   MsgStream &log) {
    if (!persVect) {
      log << MSG::WARNING << "TrigRNNOutputContainerCnv::transToPers cannot convert NULL persVect" << endreq;
      return;
    }
    if (!transVect) {
      log << MSG::WARNING << "TrigRNNOutputContainerCnv::transToPers cannot convert to NULL transVect" << endreq;
      return;
    }
    
    persVect->clear();
    persVect->reserve(transVect->size());
    // convert vector entries one by one
    for (TrigRNNOutputContainer::const_iterator it = transVect->begin();
	 it != transVect->end(); ++it ) {
      TPObjRef a = toPersistent(&m_elementCnv, *it, log);
      persVect->push_back(a);
      if (a.isNull()) {
        log << MSG::WARNING << "TrigRNNOutputContainerCnv::transToPers failed for an element " << *it << " "
	    << m_elementCnv << endreq;
      }
    } 
  }
};

#endif

