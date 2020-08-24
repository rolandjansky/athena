/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTPCnv/LArLATOMEHeaderContainerCnv_p1.h"
#include "LArTPCnv/LArLATOMEHeaderContainer_p1.h"



void
LArLATOMEHeaderContainerCnv_p1::persToTrans(const LArLATOMEHeaderContainer_p1* pers, 
				     LArLATOMEHeaderContainer* trans, MsgStream &log)
{
  const unsigned nchan=pers->m_sourceID.size();
  if (nchan != pers->m_nChannels.size() || nchan != pers->m_BCID.size()) {
    log << MSG::ERROR << "LArLATOMEHeaderContainer_p1 is inconsistent! " << std::endl;
    return;
  }

  trans->clear();
  trans->reserve(nchan);

  for (unsigned i=0;i<nchan;i++) {

    trans->push_back (new LArLATOMEHeader (pers->m_sourceID[i], pers->m_latomeID[i], pers->m_nChannels[i], pers->m_BCID[i],pers->m_l1ID[i]));

  }
}


void
LArLATOMEHeaderContainerCnv_p1::transToPers(const LArLATOMEHeaderContainer* trans, 
				     LArLATOMEHeaderContainer_p1* pers, MsgStream &/*log*/)
{
  
  unsigned int nlatome=trans->size();
  pers->m_sourceID.reserve(nlatome);
  pers->m_latomeID.reserve(nlatome);
  pers->m_nChannels.reserve(nlatome);
  pers->m_BCID.reserve(nlatome);
  pers->m_l1ID.reserve(nlatome);

  for (const LArLATOMEHeader* transHeader : *trans) {
    pers->m_sourceID.push_back(transHeader->SourceId());
    pers->m_latomeID.push_back(transHeader->LatomeId());
    pers->m_nChannels.push_back(transHeader->NChans());
    pers->m_BCID.push_back(transHeader->BCId());
    pers->m_l1ID.push_back(transHeader->L1Id());
  }
}

