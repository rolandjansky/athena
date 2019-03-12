/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetLowBetaCandidateCnv_TLP1_H
#define InDetLowBetaCandidateCnv_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_tlp1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_p1.h"

class InDetLowBetaCandidateCnv_tlp1 : public AthenaPoolTopLevelTPConverter<InDetLowBetaCandidateCnv_p1, InDetLowBetaCandidate_tlp1> {
  
 public:
  InDetLowBetaCandidateCnv_tlp1();
  virtual ~InDetLowBetaCandidateCnv_tlp1() { }

  virtual void setPStorage(InDetLowBetaCandidate_tlp1 *storage);

 protected:
  
  InDetLowBetaCandidateCnv_p1 m_lowBetaCandidateCnv;

};


template <>
class T_TPCnv<InDet::InDetLowBetaCandidate, InDetLowBetaCandidate_tlp1 >
  : public InDetLowBetaCandidateCnv_tlp1 {
 public:
    virtual void persToTrans (const InDetLowBetaCandidate_tlp1 *pers,
			      InDet::InDetLowBetaCandidate *trans,
			      MsgStream &msg);
    virtual void transToPers (const InDet::InDetLowBetaCandidate *trans,
			       InDetLowBetaCandidate_tlp1 *pers,
			      MsgStream &msg);
};


#endif
