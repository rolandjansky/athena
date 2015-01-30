/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETContainerCnv_p1
 *
 * @brief transient persistent converter for TrigMissingETContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingETContainerCnv_p1.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCONTAINER_CNV_P1_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"
 
class TrigMissingETContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigMissingETContainer, TrigMissingETContainer_p1, ITPConverterFor<TrigMissingET> >
{
 
 public:
  TrigMissingETContainerCnv_p1() {}

  //this part is implemented in T_AthenaPoolTPConverter.h. 
  //It is here temporarily to override and allow some extra debugging
  virtual void persToTrans(const  TrigMissingETContainer_p1* persVect, TrigMissingETContainer* transVect, MsgStream &log) {
    log << MSG::DEBUG << "TrigMissingETContainerCnv::persToTrans" << endreq;
    transVect->clear();
    if (persVect){
      transVect->reserve( persVect->size() );
      // convert vector entries one by one
      for( TrigMissingETContainer_p1::const_iterator 
	     it   = persVect->begin(),
	     iEnd = persVect->end();
	   it != iEnd;  ++it ) {
	TrigMissingET *p = createTransFromPStore( &m_elementCnv, *it, log ); 
	transVect->push_back( p);
	if (!p)
	  log << MSG::WARNING << "TrigMissingETContainerCnv::persToTrans failed for an element " << endreq;
      }        
    } else {
      log << MSG::WARNING << "TrigMissingETContainerCnv::persToTrans cannot convert NULL persVect" << endreq;

      return;
    }
  }
  

  virtual void transToPers(const TrigMissingETContainer* transVect, TrigMissingETContainer_p1* persVect, MsgStream &log) {
    persVect->clear();
    persVect->reserve( transVect->size() );
    // convert vector entries one by one
    for( TrigMissingETContainer::const_iterator 
	   it   = transVect->begin(),
	   iEnd = transVect->end();
	 it != iEnd;  
	 ++it ) {
      TPObjRef a = toPersistent( &m_elementCnv, *it, log );
      persVect->push_back( a );
      if (a.isNull()){
	log << MSG::WARNING << "TrigMissingETContainerCnv::transToPers failed for an element " << *it << " "
	    << m_elementCnv << endreq;
	
      }
    }       
  }
 
}; //end of class definitions
 
#endif
