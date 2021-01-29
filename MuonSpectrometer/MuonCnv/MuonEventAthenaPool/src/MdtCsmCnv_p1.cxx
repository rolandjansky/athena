/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/MdtCsm.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCsmCnv_p1.h"
#include <sstream>

void
MdtCsmCnv_p1::persToTrans(const MdtCsm_p1* persColl, MdtCsm* transColl, MsgStream &log) 
{
  *transColl = MdtCsm (Identifier(Identifier32(persColl->m_Id)),
                       persColl->m_idHash,
                       persColl->m_SubDetId,
                       persColl->m_MrodId,
                       persColl->m_CsmId);
  // The assignment above will leave *transColl as a view container.
  // But it should own it's elements, so convert it back
  // to an owning container.
  transColl->clear (SG::OWN_ELEMENTS);
  
  // Invoke vector converter from the base template
  MdtCsm_Cnvp1_base_t::persToTrans( persColl, transColl, log );
}


void
MdtCsmCnv_p1::transToPers(const MdtCsm* transColl, MdtCsm_p1* persColl, MsgStream &log) 
{
    // log <<  MSG::DEBUG << " ***  Writing out MdtCsm" << endmsg;

    persColl->m_Id       = transColl->identify().get_identifier32().get_compact();
    persColl->m_idHash   = transColl->identifyHash();

    persColl->m_SubDetId = transColl->SubDetId();
    persColl->m_MrodId   = transColl->MrodId();
    persColl->m_CsmId    = transColl->CsmId();
    
    // Invoke vector converter from the base template
   MdtCsm_Cnvp1_base_t::transToPers( transColl, persColl, log );

   // debug output
//   for(MdtCsm::const_iterator it=transColl->begin(); it != transColl->end(); ++it)  {
//      std::ostringstream ostr;
//      ostr << **it;
//      log <<  MSG::VERBOSE << "Wrote " << ostr.str() << endmsg;
//   }  
}






