/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETLOWBETACONTAINER_CNV_P1_H
#define INDETLOWBETACONTAINER_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "InDetLowBetaInfo/InDetLowBetaContainer.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_p1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_p1.h"
 

class InDetLowBetaContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv<InDet::InDetLowBetaContainer, InDetLowBetaContainer_p1, InDetLowBetaCandidateCnv_p1> {
 
 public:

    InDetLowBetaContainerCnv_p1() { }


};

/*     // This part is implemented in T_AthenaPoolTPConverter.h.  */
/*     // It is here temporarily to override and allow some extra debugging */
/*     virtual void persToTrans(const InDetLowBetaContainer_p1 *persVect, */
/* 			     InDetLowBetaContainer *transVect, */
/* 			     MsgStream &log) { */
/*       log << MSG::DEBUG << "InDetLowBetaContainerCnv::persToTrans" << endreq; */
/*       if (!persVect) { */
/* 	log << MSG::WARNING << "InDetLowBetaContainerCnv::persToTrans cannot convert NULL persVect" << endreq; */
/* 	return; */
/*       } */
/*       if (!transVect) { */
/* 	log << MSG::WARNING << "InDetLowBetaContainerCnv::persToTrans cannot convert to NULL transVect" << endreq; */
/* 	return; */
/*       } */
      
/*       transVect->clear(); */
/*       transVect->reserve(persVect->size()); */
      
/*       // convert vector entries one by one */
/*       for (InDetLowBetaContainer_p1::const_iterator it = persVect->begin(); */
/* 	   it != persVect->end();  ++it) { */
/* 	InDetLowBetaCandidate *p = createTransFromPStore(&m_elementCnv, *it, log);  */
/* 	transVect->push_back(p); */
/* 	if (!p) */
/* 	  log << MSG::WARNING << "InDetLowBetaContainerCnv::persToTrans failed for an element " << endreq; */
/*       } */
/*     } */
    

/*     virtual void transToPers(const InDetLowBetaContainer *transVect, */
/* 			     InDetLowBetaContainer_p1 *persVect, */
/* 			     MsgStream &log) { */
/*     if (!persVect) { */
/*       log << MSG::WARNING << "InDetLowBetaContainerCnv::transToPers cannot convert NULL persVect" << endreq; */
/*       return; */
/*     } */
/*     if (!transVect) { */
/*       log << MSG::WARNING << "InDetLowBetaContainerCnv::transToPers cannot convert to NULL transVect" << endreq; */
/*       return; */
/*     } */
    
/*     persVect->clear(); */
/*     persVect->reserve(transVect->size()); */
/*     // convert vector entries one by one */
/*     for (InDetLowBetaContainer::const_iterator it = transVect->begin(); */
/*          it != transVect->end(); ++it ) { */
      
/*       TPObjRef a = toPersistent(&m_elementCnv, *it, log); */
/*       persVect->push_back(a); */
/*       if (a.isNull()) { */
/*         log << MSG::WARNING << "InDetLowBetaContainerCnv::transToPers failed for an element " << *it << " " */
/*             << m_elementCnv << endreq; */
/*       }else { */
/*         log << MSG::WARNING << "InDetLowBetaContainerCnv::transToPers succeeded for an element " << *it << " " */
/*             << m_elementCnv << endreq; */
/*       } */
      
/*     }  */
/*   } */
/* }; */

#endif
