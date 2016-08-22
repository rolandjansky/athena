/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "CscRawDataCollectionCnv_p1.h"
#include <sstream>

void
CscRawDataCollectionCnv_p1::persToTrans(const CscRawDataCollection_p1* persColl, CscRawDataCollection* transColl, MsgStream &log) 
{
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading CscRawDataCollection" << endmsg;

   transColl->setSamplingRate ( persColl->m_rate);
   transColl->setNumSamples ( persColl->m_numSamples );
   transColl->setLatency ( persColl->m_latency );
   transColl->setNumDPU  ( persColl->m_numDPU);

   transColl->setOnlineId ( persColl->m_id);
   transColl->setSubDetectorId( persColl->m_subDetectorId);
   uint16_t offlineID = persColl->m_rodId;
   uint16_t rodId = 0x0;
   switch ( offlineID ) {
     case 0x0 :
        rodId = 0x0005;
        break;
     case 0x1 :
        rodId = 0x0007;
        break;
     case 0x2 :
        rodId = 0x0009;
        break;
     case 0x3 :
        rodId = 0x0011;
        break;
     case 0x4 :
        rodId = 0x0015; // slot 13 reserved
        break;
     case 0x5 :
        rodId = 0x0017;
        break;
     case 0x6 :
        rodId = 0x0019;
        break;
     case 0x7 :
        rodId = 0x0021;
        break;
     default :
        rodId = 0xFFFF;
   }
   transColl->setRodId( rodId );

   // Invoke vector converter from the base template
   CscRawDataCollection_Cnvp1_base_t::persToTrans( persColl, transColl, log );

   // debug output
   // for(CscRawDataCollection::const_iterator it=transColl->begin(); it != transColl->end(); ++it)  {
   //    std::ostringstream ostr;
   //    ostr << **it;
   //    log <<  MSG::VERBOSE << "Retrieved " << ostr.str() << endmsg;
   // }  
}



void
CscRawDataCollectionCnv_p1::transToPers(const CscRawDataCollection* /*transColl*/, CscRawDataCollection_p1* /*persColl*/, MsgStream &/*log*/) 
{
    throw GaudiException("Writing to CscRawDataCollection_p1 is not supported ", "CscRawDataCollectionCnv_p1::transToPers(...)", false);
}






