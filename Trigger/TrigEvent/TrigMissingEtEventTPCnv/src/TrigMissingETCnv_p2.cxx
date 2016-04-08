/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p2.h"
#include "TrigMissingEtEvent/TrigMissingEtComponent.h"


#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p2.h"

#include <string>


void TrigMissingETCnv_p2::persToTrans(const TrigMissingET_p2* persObj, 
				    TrigMissingET* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p2::persToTrans called " << endreq;

   // auxiliary info
   std::vector<TrigMissingEtComponent> compVec;
   if (persObj->m_comp_number > 0) {
     compVec.reserve( persObj->m_comp_number );

     for (unsigned int i=0; i<persObj->m_comp_number; ++i) {
       TrigMissingEtComponent c;
       
       c.setName(         persObj->m_c_name[i].c_str() );
       c.setStatus(       persObj->m_c_status[i]       );
       c.setEx(           persObj->m_c_ex[i]           );
       c.setEy(           persObj->m_c_ey[i]           );
       c.setEz(           persObj->m_c_ez[i]           );
       c.setSumEt(        persObj->m_c_sumEt[i]        );
       c.setSumE(         persObj->m_c_sumE[i]         );
       c.setCalib0(       persObj->m_c_calib0[i]       );
       c.setCalib1(       persObj->m_c_calib1[i]       );
       c.setSumOfSigns(   persObj->m_c_sumOfSigns[i]   );
       c.setUsedChannels( persObj->m_c_usedChannels[i] );

       compVec.push_back( c );
     }
   }

   *transObj = TrigMissingET (std::move (compVec));

   // basic info
   transObj->setEx       (persObj->m_ex); 
   transObj->setEy       (persObj->m_ey);
   transObj->setEz       (persObj->m_ez);
   transObj->setSumEt	 (persObj->m_sum_et);
   transObj->setSumE 	 (persObj->m_sum_e);
   transObj->setFlag  	 (persObj->m_flag);
   transObj->set_RoIword (persObj->m_roiWord); 
}


void TrigMissingETCnv_p2::transToPers(const TrigMissingET* transObj, 
				       TrigMissingET_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p2::transToPers called " << endreq;

   // basic info
   persObj->m_ex        = transObj->ex(); 
   persObj->m_ey        = transObj->ey();
   persObj->m_ez        = transObj->ez();
   persObj->m_sum_et	= transObj->sumEt();
   persObj->m_sum_e 	= transObj->sumE();
   persObj->m_flag  	= transObj->getFlag();
   persObj->m_roiWord   = transObj->RoIword(); 

   // auxiliary info
   persObj->m_comp_number=0;
   persObj->m_c_name.clear();
   persObj->m_c_status.clear();
   persObj->m_c_ex.clear();
   persObj->m_c_ey.clear();
   persObj->m_c_ez.clear();
   persObj->m_c_sumEt.clear();
   persObj->m_c_sumE.clear();
   persObj->m_c_calib0.clear();
   persObj->m_c_calib1.clear();
   persObj->m_c_sumOfSigns.clear();
   persObj->m_c_usedChannels.clear();
   unsigned int N = transObj->getNumOfComponents();
   if (N>0) {
     persObj->m_comp_number=N;
     persObj->m_c_name.reserve(N);
     persObj->m_c_status.reserve(N);
     persObj->m_c_ex.reserve(N);
     persObj->m_c_ey.reserve(N);
     persObj->m_c_ez.reserve(N);
     persObj->m_c_sumEt.reserve(N);
     persObj->m_c_sumE.reserve(N);
     persObj->m_c_calib0.reserve(N);
     persObj->m_c_calib1.reserve(N);
     persObj->m_c_sumOfSigns.reserve(N);
     persObj->m_c_usedChannels.reserve(N);

     for (unsigned int i=0; i<N; ++i) {
       persObj->m_c_name.push_back(         transObj->getNameOfComponent(i) );
       persObj->m_c_status.push_back(       transObj->getStatus(i) );
       persObj->m_c_ex.push_back(           transObj->getExComponent(i) );
       persObj->m_c_ey.push_back(           transObj->getEyComponent(i) );
       persObj->m_c_ez.push_back(           transObj->getEzComponent(i) );
       persObj->m_c_sumEt.push_back(        transObj->getSumEtComponent(i) );
       persObj->m_c_sumE.push_back(         transObj->getSumEComponent(i) );
       persObj->m_c_calib0.push_back(       transObj->getComponentCalib0(i) );
       persObj->m_c_calib1.push_back(       transObj->getComponentCalib1(i) );
       persObj->m_c_sumOfSigns.push_back(   transObj->getSumOfSigns(i) );
       persObj->m_c_usedChannels.push_back( transObj->getUsedChannels(i) );
     }
   }
}

