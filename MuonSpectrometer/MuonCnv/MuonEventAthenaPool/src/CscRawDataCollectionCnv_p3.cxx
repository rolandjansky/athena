/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "CscRawDataCollectionCnv_p3.h"
#include <sstream>

void
CscRawDataCollectionCnv_p3::persToTrans(const CscRawDataCollection_p3* persColl, CscRawDataCollection* transColl, MsgStream &log) 
{
   //log << MSG::DEBUG  << " ***  Reading CscRawDataCollection" << endreq;

   transColl->m_id             = persColl->m_id;
   transColl->m_rodId          = persColl->m_rodId ;
   transColl->m_subDetectorId  = persColl->m_subDetectorId;
   transColl->m_numRPU         = persColl->m_numRPU;
   transColl->m_samplingPhase  = persColl->m_samplingPhase ;
   transColl->m_triggerType    = persColl->m_triggerType ;
   transColl->m_firstBitSummary= persColl->m_firstBitSummary;
   transColl->m_eventType      = persColl->m_eventType;
   transColl->m_rpuID          = persColl->m_rpuID;
   transColl->m_dataType       = persColl->m_dataType;
   transColl->m_spuCount       = persColl->m_spuCount;
   transColl->m_scaAddress     = persColl->m_scaAddress;

   // Invoke vector converter from the base template
   CscRawDataCollection_Cnv_p3_base_t::persToTrans( persColl, transColl, log );

   // debug output
   // for(CscRawDataCollection::const_iterator it=transColl->begin(); it != transColl->end(); ++it)  {
   //    std::ostringstream ostr;
   //    ostr << **it;
   //    log <<  MSG::VERBOSE << "Retrieved " << ostr.str() << endreq;
   // }  
}



void
CscRawDataCollectionCnv_p3::transToPers(const CscRawDataCollection* transColl, CscRawDataCollection_p3* persColl, MsgStream &log) 
{
    persColl->m_id             = transColl->m_id;
    persColl->m_rodId          = transColl->m_rodId ;
    persColl->m_subDetectorId  = transColl->m_subDetectorId;
    persColl->m_numRPU         = transColl->m_numRPU;
    persColl->m_samplingPhase  = transColl->m_samplingPhase ;
    persColl->m_triggerType    = transColl->m_triggerType ;
    persColl->m_firstBitSummary= transColl->m_firstBitSummary;
    persColl->m_eventType      = transColl->m_eventType;
    persColl->m_rpuID          = transColl->m_rpuID;
    persColl->m_dataType       = transColl->m_dataType;
    persColl->m_spuCount       = transColl->m_spuCount;
    persColl->m_scaAddress     = transColl->m_scaAddress;
    
    // Invoke vector converter from the base template	 
    CscRawDataCollection_Cnv_p3_base_t::transToPers( transColl, persColl, log );
}

