/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysContainerCnv_p1
 *
 * @brief transient persistent converter for TrigL2BphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysContainerCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 * $Id: TrigL2BphysContainerCnv_p1.h,v 1.3 2010-08-11 demelian Exp $
 **********************************************************************************/
#ifndef TTRIGPARTICLETPCNV_RIGL2BPHYSCONTAINER_CNV_P1_H
#define TTRIGPARTICLETPCNV_RIGL2BPHYSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
//#include "TrigParticleTPCnv/TrigL2BphysCnv_p2.h"
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigParticleTPCnv/TrigL2BphysContainer_p1.h"
 
class TrigL2BphysContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigL2BphysContainer, TrigL2BphysContainer_p1, ITPConverterFor<TrigL2Bphys> >
{
 
 public:
  TrigL2BphysContainerCnv_p1() {}

  //this part is implemented in T_AthenaPoolTPConverter.h. 
  //It is here temporarily to override and allow some extra debugging
  virtual void persToTrans(const  TrigL2BphysContainer_p1* persVect, TrigL2BphysContainer* transVect, MsgStream &log) {
    log << MSG::DEBUG << "TrigL2BphysContainerCnv::persToTrans" << endreq;
    if (persVect){
      transVect->clear();
      transVect->reserve( persVect->size() );
      // convert vector entries one by one
      for( TrigL2BphysContainer_p1::const_iterator 
	     it   = persVect->begin(),
	     iEnd = persVect->end();
	   it != iEnd;  ++it ) {
	TrigL2Bphys *p = createTransFromPStore( &m_elementCnv, *it, log ); 
	transVect->push_back( p);
	if (!p)
	  log << MSG::WARNING << "TrigL2BphysContainerCnv::persToTrans failed for an element " << endreq;
      }        
    } else {
      log << MSG::WARNING << "TrigL2BphysContainerCnv::persToTrans cannot convert NULL persVect" << endreq;
    }
  }
  

  virtual void transToPers(const TrigL2BphysContainer* transVect, TrigL2BphysContainer_p1* persVect, MsgStream &log) {
    persVect->clear();
    persVect->reserve( transVect->size() );
    // convert vector entries one by one
    for( TrigL2BphysContainer::const_iterator 
	   it   = transVect->begin(),
	   iEnd = transVect->end();
	 it != iEnd;  
	 ++it ) {
      TPObjRef a = toPersistent( &m_elementCnv, *it, log );
      persVect->push_back( a );
      if (a.isNull()){
	log << MSG::WARNING << "TrigL2BphysContainerCnv::transToPers failed for an element " << *it << " "
	    << m_elementCnv << endreq;
	
      }
    }       
  }
 
}; //end of class definitions
 
#endif
