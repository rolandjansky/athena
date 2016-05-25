/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimData/InDetSimData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetSimData_p1.h"
#include "InDetSimDataCnv_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "SGTools/IProxyDictWithPool.h"

typedef std::vector<InDetSimData::Deposit>::const_iterator depositIterator;

InDetSimDataCnv_p1::InDetSimDataCnv_p1()
  : m_sg(nullptr)
{
}


void
InDetSimDataCnv_p1::persToTrans(const InDetSimData_p1* persObj, InDetSimData* transObj, MsgStream &log)
{
   MSG_VERBOSE(log,"InDetSimDataCnv_p1::persToTrans called ");
   HepMcParticleLinkCnv_p1 HepMcPLCnv;
   HepMcParticleLink mcLink (m_sg);
   std::vector<InDetSimData::Deposit> deposits;
   deposits.reserve( persObj->m_enDeposits.size() );
   for (unsigned int icount=0; icount < persObj->m_enDeposits.size(); icount++) {
     HepMcPLCnv.persToTrans(&(persObj->m_links[icount]),&mcLink, log);
     deposits.emplace_back (mcLink, persObj->m_enDeposits[icount]);
   }

   *transObj = InDetSimData (std::move(deposits),
                             persObj->m_word);
}

void
InDetSimDataCnv_p1::transToPers(const InDetSimData* transObj, InDetSimData_p1* persObj, MsgStream &log) 
{
   MSG_VERBOSE(log,"InDetSimDataCnv_p1::transToPers called ");
   HepMcParticleLinkCnv_p1 HepMcPLCnv;

   persObj->m_word = transObj->word();
   const std::vector<InDetSimData::Deposit> &dep(transObj->getdeposits());
   persObj->m_links.resize(dep.size() );
   persObj->m_enDeposits.resize(dep.size() );
   depositIterator it=dep.begin();
   for (int icount=0; it != dep.end(); it++, icount++) {
     HepMcPLCnv.transToPers(&(dep[icount].first), &(persObj->m_links[icount]), log);
     persObj->m_enDeposits[icount] = dep[icount].second;
   }
}

void InDetSimDataCnv_p1::setCurrentStore (IProxyDictWithPool* store)
{
  m_sg = store;
}
