/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetSimData/InDetSimData.h"
#undef private
#undef protected

#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetSimData_p1.h"
#include "InDetSimDataCnv_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

typedef std::vector<InDetSimData::Deposit>::const_iterator depositIterator;

void
InDetSimDataCnv_p1::persToTrans(const InDetSimData_p1* persObj, InDetSimData* transObj, MsgStream &log)
{
   MSG_VERBOSE(log,"InDetSimDataCnv_p1::persToTrans called ");
   HepMcParticleLinkCnv_p1 HepMcPLCnv;
   transObj->m_word = persObj->m_word;
   transObj->m_deposits.resize( persObj->m_enDeposits.size() );
   for (unsigned int icount=0; icount < persObj->m_enDeposits.size(); icount++) {
     HepMcParticleLink mcLink;
     HepMcPLCnv.persToTrans(&(persObj->m_links[icount]),&mcLink, log);
     transObj->m_deposits[icount] = std::make_pair(mcLink, persObj->m_enDeposits[icount]);
   }
}

void
InDetSimDataCnv_p1::transToPers(const InDetSimData* transObj, InDetSimData_p1* persObj, MsgStream &log) 
{
   MSG_VERBOSE(log,"InDetSimDataCnv_p1::transToPers called ");
   HepMcParticleLinkCnv_p1 HepMcPLCnv;

   persObj->m_word = transObj->m_word;
   const std::vector<InDetSimData::Deposit> &dep(transObj->getdeposits());
   persObj->m_links.resize(dep.size() );
   persObj->m_enDeposits.resize(dep.size() );
   depositIterator it=dep.begin();
   for (int icount=0; it != dep.end(); it++, icount++) {
     HepMcPLCnv.transToPers(&(dep[icount].first), &(persObj->m_links[icount]), log);
     persObj->m_enDeposits[icount] = dep[icount].second;
   }
}
