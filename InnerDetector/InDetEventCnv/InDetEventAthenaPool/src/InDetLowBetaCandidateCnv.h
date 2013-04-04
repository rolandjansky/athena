/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTATHENAPOOL_INDETLOWBETACANDIDATECNV_H
#define INDETEVENTATHENAPOOL_INDETLOWBETACANDIDATECNV_H

#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class InDetLowBetaCandidate_tlp1;
class InDetLowBetaCandidateCnv_tlp1;


// typedef to the latest persistent version
typedef InDetLowBetaCandidate_tlp1  InDetLowBetaCandidate_PERS;

class MsgStream;

class InDetLowBetaCandidateCnv  : public T_AthenaPoolCustomCnv<InDet::InDetLowBetaCandidate, InDetLowBetaCandidate_PERS> {

  friend class CnvFactory<InDetLowBetaCandidateCnv>;

 protected:
  
  InDetLowBetaCandidateCnv(ISvcLocator* svcloc);
  ~InDetLowBetaCandidateCnv();

  InDetLowBetaCandidate_PERS*  createPersistent(InDet::InDetLowBetaCandidate* transCont);

  InDet::InDetLowBetaCandidate*       createTransient ();

 private:

  InDetLowBetaCandidateCnv_tlp1* m_TPConverter;

};

#endif
