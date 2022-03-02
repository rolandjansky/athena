/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimData/InDetSimData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetSimData_p2.h"
#include "InDetSimDataCnv_p2.h"
#include "AthenaBaseComps/AthMessaging.h"


using depositIterator = std::vector<InDetSimData::Deposit>::const_iterator;

InDetSimDataCnv_p2::InDetSimDataCnv_p2()
  : m_sg(nullptr)
{
}


void
InDetSimDataCnv_p2::persToTrans(const InDetSimData_p2* persObj, InDetSimData* transObj, MsgStream &log)
{
  MSG_VERBOSE(log,"InDetSimDataCnv_p2::persToTrans called ");
  std::vector<InDetSimData::Deposit> deposits;
  const unsigned int ndeposits = persObj->m_enDeposits.size();
  deposits.reserve( ndeposits );
  for (unsigned int icount=0; icount < ndeposits; icount++) {
    HepMcParticleLink mcLink (m_sg);
    HepMcPLCnv.persToTrans(&(persObj->m_links[icount]),&mcLink, log);
    deposits.emplace_back (mcLink, persObj->m_enDeposits[icount]);
  }

  *transObj = InDetSimData (std::move(deposits),
                            persObj->m_word);
}

void
InDetSimDataCnv_p2::transToPers(const InDetSimData* transObj, InDetSimData_p2* persObj, MsgStream &log)
{
  MSG_VERBOSE(log,"InDetSimDataCnv_p2::transToPers called ");
  HepMcParticleLinkCnv_p2 HepMcPLCnv;

  persObj->m_word = transObj->word();
  const std::vector<InDetSimData::Deposit> &dep(transObj->getdeposits());
  const unsigned int ndeposits = dep.size();
  persObj->m_links.resize(ndeposits);
  persObj->m_enDeposits.resize(ndeposits);
  for (unsigned int icount(0); icount < ndeposits; ++icount) {
    HepMcPLCnv.transToPers(&(dep[icount].first), &(persObj->m_links[icount]), log);
    persObj->m_enDeposits[icount] = dep[icount].second;
  }
}

void InDetSimDataCnv_p2::setCurrentStore (IProxyDict* store)
{
  m_sg = store;
}
