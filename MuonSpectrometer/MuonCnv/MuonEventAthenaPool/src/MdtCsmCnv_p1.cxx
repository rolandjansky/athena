/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/MdtCsm.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "MdtCsmCnv_p1.h"
#include <sstream>

void
MdtCsmCnv_p1::persToTrans(const MdtCsm_p1* persColl, MdtCsm* transColl, MsgStream &log) 
{
    //  log << MSG::DEBUG  << " ***  Reading MdtCsm" << endreq;

    transColl->m_Id        = Identifier(Identifier32(persColl->m_Id)) ;
    transColl->m_idHash    = persColl->m_idHash;        
    transColl->m_SubDetId  = persColl->m_SubDetId ;
    transColl->m_MrodId    = persColl->m_MrodId ;
    transColl->m_CsmId     = persColl->m_CsmId ;
  
   // Invoke vector converter from the base template
   MdtCsm_Cnvp1_base_t::persToTrans( persColl, transColl, log );

   // debug output
//   for(MdtCsm::const_iterator it=transColl->begin(); it != transColl->end(); ++it)  {
//      std::ostringstream ostr;
//      ostr << **it;
//      log <<  MSG::VERBOSE << "Retrieved " << ostr.str() << endreq;
//   }  
}



void
MdtCsmCnv_p1::transToPers(const MdtCsm* transColl, MdtCsm_p1* persColl, MsgStream &log) 
{
    // log <<  MSG::DEBUG << " ***  Writing out MdtCsm" << endreq;

    persColl->m_Id       = transColl->m_Id.get_identifier32().get_compact();
    persColl->m_idHash   = transColl->m_idHash;

    persColl->m_SubDetId = transColl->m_SubDetId;
    persColl->m_MrodId   = transColl->m_MrodId;
    persColl->m_CsmId    = transColl->m_CsmId;
    
    // Invoke vector converter from the base template
   MdtCsm_Cnvp1_base_t::transToPers( transColl, persColl, log );

   // debug output
//   for(MdtCsm::const_iterator it=transColl->begin(); it != transColl->end(); ++it)  {
//      std::ostringstream ostr;
//      ostr << **it;
//      log <<  MSG::VERBOSE << "Wrote " << ostr.str() << endreq;
//   }  
}






