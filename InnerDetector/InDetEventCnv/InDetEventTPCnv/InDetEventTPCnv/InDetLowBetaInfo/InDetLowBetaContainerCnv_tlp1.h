/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  INDETLOWBETACONTAINER_CNV_TLP1_H
#define  INDETLOWBETACONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_tlp1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainerCnv_p1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_p1.h" 
 

class InDetLowBetaContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter<InDetLowBetaContainerCnv_p1, InDetLowBetaContainer_tlp1> {
  
 public:
  
  InDetLowBetaContainerCnv_tlp1();
  virtual ~InDetLowBetaContainerCnv_tlp1() { }
  
  // default methods  
  virtual void setPStorage(InDetLowBetaContainer_tlp1 *storage);
  
 protected:
  InDetLowBetaContainerCnv_p1 m_lowBetaContainerCnv;
  InDetLowBetaCandidateCnv_p1 m_lowBetaCandidateCnv;
    
};


template <>
class T_TPCnv<InDet::InDetLowBetaContainer, InDetLowBetaContainer_tlp1>
  : public InDetLowBetaContainerCnv_tlp1 {

 public:

    // ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
    virtual void persToTrans(const InDetLowBetaContainer_tlp1 *pers,
			     InDet::InDetLowBetaContainer *trans,
			     MsgStream& msg);
    virtual void transToPers(const InDet::InDetLowBetaContainer *trans,
			     InDetLowBetaContainer_tlp1 *pers,
			     MsgStream& msg);
    
};


#endif
