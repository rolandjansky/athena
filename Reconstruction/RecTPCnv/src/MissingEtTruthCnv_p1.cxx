/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtTruthCnv_p1.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingEtTruth class
********************************************************************/


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingEtTruthCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p1.h"

// MissingET converter
static MissingETCnv_p1 metCnv;

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtTruthCnv_p1::persToTrans( const MissingEtTruth_p1* pers,
				        MissingEtTruth* trans, 
				        MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading MissingEtTruth from persistent state..."  << endreq;

  trans->m_exTruth      = pers->m_exTruth;
  trans->m_eyTruth      = pers->m_eyTruth;
  trans->m_etSumTruth   = pers->m_etSumTruth;
  
  // use the MissingET converter to convert from pers to trans 
  metCnv.persToTrans(&pers->m_met, trans, msg);
  
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"IN  EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;
	     
  return;
}

void MissingEtTruthCnv_p1::transToPers( const MissingEtTruth* trans, 
				        MissingEtTruth_p1* pers, 
				        MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingEtTruth..."    << endreq;

  pers->m_exTruth     = trans->m_exTruth;
  pers->m_eyTruth     = trans->m_eyTruth;
  pers->m_etSumTruth  = trans->m_etSumTruth;
  
  
  if( trans->m_source >= 0 && trans->m_source < 1000 )  
  {
    // use the MissingET converter to convert from trans to pers
    MissingET_p1 theMet;
    metCnv.transToPers( trans, &theMet, msg );
    pers->m_met = theMet;
     
  }  
  
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"OUT EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;

 return;
}
