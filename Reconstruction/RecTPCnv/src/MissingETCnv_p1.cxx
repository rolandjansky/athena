/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingETCnv_p1.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingET class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingETCnv_p1.h"
#include "RecTPCnv/MissingEtRegionsCnv_p1.h"

// region converter
static MissingEtRegionsCnv_p1 regCnv;


/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingETCnv_p1::persToTrans(  const MissingET_p1* pers,
				    MissingET* trans, 
				    MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading MissingET from persistent state..."  << endreq;

  trans->m_ex      = pers->m_ex;
  trans->m_ey      = pers->m_ey;
  trans->m_etSum   = pers->m_etSum;
  trans->m_source  = static_cast<MissingET::Source>(pers->m_source);
  
  delete trans->m_regions;
  trans->m_regions = 0;
  {
    // use the region converter to convert from pers to trans 
    MissingEtRegions *theReg = new MissingEtRegions();
    regCnv.persToTrans(&pers->m_regions, theReg, msg);
    trans->m_regions = theReg;
  }
	
    // std::cout<<"IN  source: "<<trans->m_source;
    // std::cout<<"\tex: "<<trans->m_ex;
    // std::cout<<"\tey: "<<trans->m_ey;
    // std::cout<<"\tetSum: "<<trans->m_etSum;
    // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
  return;
}

void MissingETCnv_p1::transToPers(  const MissingET* trans, 
				    MissingET_p1* pers, 
				    MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingET..."    << endreq;

  pers->m_ex     = trans->m_ex;	
  pers->m_ey     = trans->m_ey;	
  pers->m_etSum  = trans->m_etSum;
  pers->m_source = static_cast<unsigned int>(trans->m_source);
  
  // use the region converter to convert from trans to pers  
  if( trans->m_regions != 0)   
  {
    regCnv.transToPers( trans->m_regions, &pers->m_regions, msg );
  }
  
  // std::cout<<"OUT source: "<<trans->m_source;
  // std::cout<<"\tex: "<<trans->m_ex;
  // std::cout<<"\tey: "<<trans->m_ey;
  // std::cout<<"\tetSum: "<<trans->m_etSum;
  // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
  
 return;
}
