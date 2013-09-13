/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/MuCTPI_RDOCnv_p1.h"

/**
 * Default constructor.
 */
MuCTPI_RDOCnv_p1::MuCTPI_RDOCnv_p1()
   : T_AthenaPoolTPCnvBase< MuCTPI_RDO, MuCTPI_RDO_p1 >() {

}

/**
 * Function transferring the information from a persistent MuCTPI_RDO_p1 object
 * to a transient MuCTPI_RDO object.
 */
void MuCTPI_RDOCnv_p1::persToTrans( const MuCTPI_RDO_p1* persObj, MuCTPI_RDO* transObj,
                                    MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting MuCTPI_RDO from persistent state..." << endreq;

   transObj->m_candidateMultiplicity.clear();
   for( std::vector< uint32_t >::const_iterator it = persObj->m_candidateMultiplicity.begin();
        it != persObj->m_candidateMultiplicity.end(); ++ it ) {
      transObj->m_candidateMultiplicity.push_back( *it );
   }

   transObj->m_dataWord.clear();
   for( std::vector< uint32_t >::const_iterator it = persObj->m_dataWord.begin();
        it != persObj->m_dataWord.end(); ++ it ) {
      transObj->m_dataWord.push_back( *it );
   }

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting MuCTPI_RDO from persistent state [OK]" << endreq;

   return;

}

/**
 * Function transferring the information from a transient MuCTPI_RDO object
 * to a persistent MuCTPI_RDO_p1 object.
 */
void MuCTPI_RDOCnv_p1::transToPers( const MuCTPI_RDO* transObj, MuCTPI_RDO_p1* persObj,
                                    MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of MuCTPI_RDO..." << endreq;

   persObj->m_candidateMultiplicity.clear();
   for( std::vector< uint32_t >::const_iterator it = transObj->m_candidateMultiplicity.begin();
        it != transObj->m_candidateMultiplicity.end(); ++ it ) {
      persObj->m_candidateMultiplicity.push_back( *it );
   }

   persObj->m_dataWord.clear();
   for( std::vector< uint32_t >::const_iterator it = transObj->m_dataWord.begin();
        it != transObj->m_dataWord.end(); ++ it ) {
      persObj->m_dataWord.push_back( *it );
   }

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of MuCTPI_RDO [OK]" << endreq;

   return;

}
