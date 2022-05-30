/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtTruthCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p1.h"

// MissingET converter
static const MissingETCnv_p1 metCnv;


void MissingEtTruthCnv_p1::persToTrans( const MissingEtTruth_p1* pers,
				        MissingEtTruth* trans, 
				        MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading MissingEtTruth from persistent state..."  << endmsg;

  trans->setExTruthVec      (std::vector<double> (pers->m_exTruth));
  trans->setEyTruthVec      (std::vector<double> (pers->m_eyTruth));
  trans->setEtSumTruthVec   (std::vector<double> (pers->m_etSumTruth));
  
  // use the MissingET converter to convert from pers to trans 
  metCnv.persToTrans(&pers->m_met, trans, msg);
  
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"IN  EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;
	     
  return;
}

void MissingEtTruthCnv_p1::transToPers( const MissingEtTruth* trans, 
				        MissingEtTruth_p1* pers, 
				        MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingEtTruth..."    << endmsg;

  pers->m_exTruth     = trans->exTruthVec();
  pers->m_eyTruth     = trans->eyTruthVec();
  pers->m_etSumTruth  = trans->etSumTruthVec();
  
  
  if( trans->getSource() >= 0 && trans->getSource() < 1000 )  
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
