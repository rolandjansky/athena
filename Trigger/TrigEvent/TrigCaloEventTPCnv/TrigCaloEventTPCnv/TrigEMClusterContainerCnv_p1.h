/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterContainerCnv_p1
 *
 * @brief transient persistent converter for TrigEMClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterContainerCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
 
class TrigEMClusterContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigEMClusterContainer, TrigEMClusterContainer_p1, ITPConverterFor<TrigEMCluster> >
{
 
 public:

  TrigEMClusterContainerCnv_p1() {}

  //this part is implemented in T_AthenaPoolTPConverter.h. 
  //It is here temporarily to override and allow some extra debugging
  virtual void persToTrans(const  TrigEMClusterContainer_p1* persVect, TrigEMClusterContainer* transVect, MsgStream &log) {
    log << MSG::DEBUG << "TrigEMClusterContainerCnv::persToTrans" << endreq;
    if (persVect){
      transVect->clear();
      transVect->reserve( persVect->size() );
      // convert vector entries one by one
      for( TrigEMClusterContainer_p1::const_iterator 
	     it   = persVect->begin(),
	     iEnd = persVect->end();
	   it != iEnd;  ++it ) {
	TrigEMCluster *p = createTransFromPStore( &m_elementCnv, *it, log ); 
	transVect->push_back( p);
	if (!p)
	  log << MSG::WARNING << "TrigEMClusterContainerCnv::persToTrans failed for an element " << endreq;
      }        
    } else {
      log << MSG::WARNING << "TrigEMClusterContainerCnv::persToTrans cannot convert NULL persVect" << endreq;
    }
  }
  

  virtual void transToPers(const TrigEMClusterContainer* transVect, TrigEMClusterContainer_p1* persVect, MsgStream &log) {
    persVect->clear();
    persVect->reserve( transVect->size() );
    // convert vector entries one by one
    for( TrigEMClusterContainer::const_iterator 
	   it   = transVect->begin(),
	   iEnd = transVect->end();
	 it != iEnd;  
	 ++it ) {
      TPObjRef a = toPersistent( &m_elementCnv, *it, log );
      persVect->push_back( a );
      if (a.isNull()){
	log << MSG::WARNING << "TrigEMClusterContainerCnv::transToPers failed for an element " << *it << " "
	    << m_elementCnv << endreq;
	
      }
    }       
  }
  
 
}; //end of class definitions
 
#endif
