/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "MissingETEvent/MissingEtRegions.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtRegionsCnv_p1.h"


void MissingEtRegionsCnv_p1::persToTrans(  const MissingEtRegions_p1* pers,
				           MissingEtRegions* trans, 
				           MsgStream& /*msg*/ ) const
{
//   msg << MSG::DEBUG << "Loading MissingEtRegions from persistent state..."  << endmsg;

  auto getVec = [] (const std::vector<double>& v)
                {
                  std::vector newv (v);
                  if (newv.empty()) newv.resize (MissingEtRegions::Size, 0);
                  return newv;
                };

  trans->setExRegVec      (getVec (pers->m_exReg));
  trans->setEyRegVec      (getVec (pers->m_eyReg));
  trans->setEtSumRegVec   (getVec (pers->m_etReg));

    
    // for (unsigned int vi=0;vi<trans->m_exReg.size();++vi)
    //      std::cout<<"IN  regions ex: "<<trans->m_exReg[vi]<<"\tey: "<<trans->m_eyReg[vi]<<"\tet: "<<trans->m_etReg[vi]<<std::endl;
	     
  return;
}

void MissingEtRegionsCnv_p1::transToPers(  const MissingEtRegions* trans, 
				           MissingEtRegions_p1* pers, 
				           MsgStream& /*msg*/ ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingEtRegions..."   << endmsg;

  pers->m_exReg     = trans->exRegVec();  
  pers->m_eyReg     = trans->eyRegVec(); 
  pers->m_etReg     = trans->etSumRegVec(); 
  
    // for (unsigned int vi=0;vi<trans->m_exReg.size();++vi)
    //      std::cout<<"OUT regions ex: "<<trans->m_exReg[vi]<<"\tey: "<<trans->m_eyReg[vi]<<"\tet: "<<trans->m_etReg[vi]<<std::endl;
 
 return;
}
