/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"

// RecTPCnv includes
#include "RecTPCnv/MissingETCnv_p1.h"
#include "RecTPCnv/MissingEtRegionsCnv_p1.h"

// region converter
static const MissingEtRegionsCnv_p1 regCnv;


void MissingETCnv_p1::persToTrans(  const MissingET_p1* pers,
				    MissingET* trans, 
				    MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading MissingET from persistent state..."  << endmsg;

  auto regions = std::make_unique<MissingEtRegions>();
  regCnv.persToTrans(&pers->m_regions, regions.get(), msg);

  *trans = MissingET (static_cast<MissingET::Source>(pers->m_source),
                      std::move (regions),
                      pers->m_ex,
                      pers->m_ey,
                      pers->m_etSum);
	
    // std::cout<<"IN  source: "<<trans->m_source;
    // std::cout<<"\tex: "<<trans->m_ex;
    // std::cout<<"\tey: "<<trans->m_ey;
    // std::cout<<"\tetSum: "<<trans->m_etSum;
    // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
  return;
}

void MissingETCnv_p1::transToPers(  const MissingET* trans, 
				    MissingET_p1* pers, 
				    MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingET..."    << endmsg;

  pers->m_ex     = trans->etx();
  pers->m_ey     = trans->ety();
  pers->m_etSum  = trans->sumet();
  pers->m_source = trans->getSource();
  
  // use the region converter to convert from trans to pers  
  if( trans->getRegions() != 0)   
  {
    regCnv.transToPers( trans->getRegions(), &pers->m_regions, msg );
  }
  
  // std::cout<<"OUT source: "<<trans->m_source;
  // std::cout<<"\tex: "<<trans->m_ex;
  // std::cout<<"\tey: "<<trans->m_ey;
  // std::cout<<"\tetSum: "<<trans->m_etSum;
  // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
  
 return;
}
