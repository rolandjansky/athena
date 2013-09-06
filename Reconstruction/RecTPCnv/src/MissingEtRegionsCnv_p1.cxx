/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtRegionsCnv_p1.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingEtRegions class
********************************************************************/


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingEtRegions.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingEtRegionsCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtRegionsCnv_p1::persToTrans(  const MissingEtRegions_p1* pers,
				           MissingEtRegions* trans, 
				           MsgStream& /*msg*/ ) 
{
//   msg << MSG::DEBUG << "Loading MissingEtRegions from persistent state..."  << endreq;

  trans->m_exReg      = pers->m_exReg;  
  trans->m_eyReg      = pers->m_eyReg; 
  trans->m_etReg      = pers->m_etReg; 

    
    // for (unsigned int vi=0;vi<trans->m_exReg.size();++vi)
    //      std::cout<<"IN  regions ex: "<<trans->m_exReg[vi]<<"\tey: "<<trans->m_eyReg[vi]<<"\tet: "<<trans->m_etReg[vi]<<std::endl;
	     
  return;
}

void MissingEtRegionsCnv_p1::transToPers(  const MissingEtRegions* trans, 
				           MissingEtRegions_p1* pers, 
				           MsgStream& /*msg*/ ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingEtRegions..."   << endreq;

  pers->m_exReg     = trans->m_exReg;  
  pers->m_eyReg     = trans->m_eyReg; 
  pers->m_etReg     = trans->m_etReg; 
  
    // for (unsigned int vi=0;vi<trans->m_exReg.size();++vi)
    //      std::cout<<"OUT regions ex: "<<trans->m_exReg[vi]<<"\tey: "<<trans->m_eyReg[vi]<<"\tet: "<<trans->m_etReg[vi]<<std::endl;
 
 return;
}
