/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p3.h"
#undef private
#undef protected


#include "TrigMissingEtEvent/TrigMissingEtComponent.h"


#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p3.h"

#include <string>


void TrigMissingETCnv_p3::persToTrans(const TrigMissingET_p3* persObj, 
				    TrigMissingET* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p3::persToTrans called " << endreq;

   // basic info
   transObj->m_ex        = persObj->m_allTheFloats[0] ; 
   transObj->m_ey        = persObj->m_allTheFloats[1] ;
   transObj->m_ez        = persObj->m_allTheFloats[2] ;
   transObj->m_sum_et	 = persObj->m_allTheFloats[3] ;
   transObj->m_sum_e 	 = persObj->m_allTheFloats[4] ;

   transObj->m_flag  	 = persObj->m_flag     ;
   transObj->m_roiWord   = persObj->m_roiWord  ; 

   // auxiliary info
   transObj->m_compVec.clear();

   if (persObj->m_comp_number > 0) {
     transObj->m_compVec.reserve( persObj->m_comp_number );

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

       transObj->m_compVec.push_back( c );
     }
   }
}


void TrigMissingETCnv_p3::transToPers(const TrigMissingET* transObj, 
				       TrigMissingET_p3* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p3::transToPers called " << endreq;

   // basic info
   persObj->m_allTheFloats[0] = transObj->m_ex      ; 
   persObj->m_allTheFloats[1] = transObj->m_ey      ;
   persObj->m_allTheFloats[2] = transObj->m_ez      ;
   persObj->m_allTheFloats[3] =  transObj->m_sum_et ;
   persObj->m_allTheFloats[4] = transObj->m_sum_e   ;

   persObj->m_flag  	= transObj->m_flag     ;
   persObj->m_roiWord   = transObj->m_roiWord  ; 

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
       TrigMissingEtComponent c = transObj->m_compVec[i];

       persObj->m_c_name.push_back(         c.name()         );
       persObj->m_c_status.push_back(       c.status()       );
       persObj->m_c_ex.push_back(           c.ex()           );
       persObj->m_c_ey.push_back(           c.ey()           );
       persObj->m_c_ez.push_back(           c.ez()           );
       persObj->m_c_sumEt.push_back(        c.sumEt()        );
       persObj->m_c_sumE.push_back(         c.sumE()         );
       persObj->m_c_calib0.push_back(       c.calib0()       );
       persObj->m_c_calib1.push_back(       c.calib1()       );
       persObj->m_c_sumOfSigns.push_back(   c.sumOfSigns()   );
       persObj->m_c_usedChannels.push_back( c.usedChannels() );
     }
   }
}

