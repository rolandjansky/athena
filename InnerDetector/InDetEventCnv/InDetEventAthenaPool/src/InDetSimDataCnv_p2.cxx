/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimData/InDetSimData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetSimData_p2.h"
#include "InDetSimDataCnv_p2.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "SGTools/IProxyDictWithPool.h"

typedef std::vector<InDetSimData::Deposit>::const_iterator depositIterator;

InDetSimDataCnv_p2::InDetSimDataCnv_p2()
  : m_sg(nullptr)
{
}


void
InDetSimDataCnv_p2::persToTrans(const InDetSimData_p2* persObj, InDetSimData* transObj, MsgStream &log)
{
  MSG_VERBOSE(log,"InDetSimDataCnv_p2::persToTrans called ");
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  std::vector<InDetSimData::Deposit> deposits;
  deposits.reserve( persObj->m_enDeposits.size() );
  for (unsigned int icount=0; icount < persObj->m_enDeposits.size(); icount++) {
    HepMcParticleLink mcLink;
    HepMcPLCnv.persToTrans(&(persObj->m_links[icount]),&mcLink, log);
    deposits.emplace_back (mcLink, persObj->m_enDeposits[icount]);
  }

  *transObj = InDetSimData (std::move(deposits),
                            persObj->m_word);
}

void
InDetSimDataCnv_p2::transToPers(const InDetSimData*, InDetSimData_p2*, MsgStream &/*log*/)
{
  throw std::runtime_error("InDetSimDataCnv_p2::transToPers is not supported in this release!");}

void InDetSimDataCnv_p2::setCurrentStore (IProxyDictWithPool* store)
{
  m_sg = store;
}
