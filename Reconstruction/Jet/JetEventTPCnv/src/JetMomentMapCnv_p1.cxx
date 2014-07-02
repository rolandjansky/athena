/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "JetEventTPCnv/JetMomentMapCnv_p1.h"

#include "JetEvent/JetCollection.h"

void
JetMomentMapCnv_p1::transToPers(const JetMomentMap* ,
				JetMomentMap_p1* ,
				MsgStream &reporter )
{
  reporter << MSG::WARNING << "JetMomentMapConverterBase toPers() Template _p1: deprecated ! This should not be used." << endreq;
  
}

void
JetMomentMapCnv_p1::persToTrans(const JetMomentMap_p1* ,
				JetMomentMap* ,
				MsgStream &reporter )
{
  reporter << MSG::WARNING << "JetMomentMapConverterBase toTrans() Template _p1 : unusable yet" << endreq;
  
  // ElementLinkVectorCnv_p1<ElementLinkVector<JetCollection> > m_ELVCnv;
  // m_ELVCnv.persToTrans(&persObj->m_jetsV, &transObj->m_jets, reporter);
  // // clear and reserve space
  // transObj->m_moms.clear();
  // transObj->m_moms.reserve( transObj->m_jets.size() );
  
  // for( unsigned int loop = 0; loop < transObj->m_jets.size(); ++loop  )
  //   {
  //     JetMomentStore store;
  //     for ( unsigned int iloop = 0; iloop < persObj->m_mtypV.at(loop).size(); ++iloop )
  //       {
  //         JetMoment mom( (JetMoment::MomentType)persObj->m_mtypV.at(loop).at(iloop),
  //       		 persObj->m_mvalV.at(loop).at(iloop) );
  //         store.insert( (JetMoment::MomentType)persObj->m_mtypV.at(loop).at(iloop), mom );
  //         reporter << MSG::WARNING << "Jet " << loop << " "
  //       	   << persObj->m_mtypV.at(loop).at(iloop) << " "
  //       	   << persObj->m_mvalV.at(loop).at(iloop) << endreq;
  //       }
  //     transObj->m_moms.push_back(store);
  //   }
}
