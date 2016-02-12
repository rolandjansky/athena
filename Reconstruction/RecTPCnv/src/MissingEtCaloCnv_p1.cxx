/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtCaloCnv_p1.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingEtCalo class
********************************************************************/


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingEtCaloCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p1.h"

// MissingET converter
static MissingETCnv_p1 metCnv;

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtCaloCnv_p1::persToTrans(  const MissingEtCalo_p1* pers,
				        MissingEtCalo* trans, 
				        MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading MissingEtCalo from persistent state..."   << endreq;

  trans->m_exCalo      = pers->m_exCalo;  
  trans->m_eyCalo      = pers->m_eyCalo; 
  trans->m_etSumCalo   = pers->m_etSumCalo; 
  trans->m_nCellsCalo  = pers->m_nCellsCalo;
  
  {
    // use the MissingET converter to convert from pers to trans 
    metCnv.persToTrans(&pers->m_met, trans, msg);
  } 
  
    // for (unsigned int vi=0;vi<trans->m_exCalo.size();++vi)
    //      std::cout<<"IN  EtCalo ex: "<<trans->m_exCalo[vi]<<"\tey: "<<trans->m_eyCalo[vi]<<"\tet: "<<trans->m_etSumCalo[vi]<<"\tnCells: "<<trans->m_nCellsCalo[vi]<<std::endl;
  
  return;
}

void MissingEtCaloCnv_p1::transToPers(  const MissingEtCalo* trans, 
				        MissingEtCalo_p1* pers, 
				        MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingEtCalo..."  << endreq;

  pers->m_exCalo     = trans->m_exCalo;  
  pers->m_eyCalo     = trans->m_eyCalo; 
  pers->m_etSumCalo  = trans->m_etSumCalo; 
  pers->m_nCellsCalo = trans->m_nCellsCalo;
  
  if( trans->m_source >= 0 && trans->m_source < 1000 )  {
    
    // use the MissingET converter to convert from trans to pers
    MissingET_p1 theMet;
    metCnv.transToPers( trans, &theMet, msg );
    pers->m_met = theMet;
     
  } 
  
    // for (unsigned int vi=0;vi<trans->m_exCalo.size();++vi)
    //      std::cout<<"OUT EtCalo ex: "<<trans->m_exCalo[vi]<<"\tey: "<<trans->m_eyCalo[vi]<<"\tet: "<<trans->m_etSumCalo[vi]<<"\tnCells: "<<trans->m_nCellsCalo[vi]<<std::endl;
  
 return;
}
