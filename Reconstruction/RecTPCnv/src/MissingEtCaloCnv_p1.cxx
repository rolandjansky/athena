/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtCaloCnv_p1.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persistent converter for MissingEtCalo class
********************************************************************/


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtCaloCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p1.h"

// MissingET converter
static const MissingETCnv_p1 metCnv;


void MissingEtCaloCnv_p1::persToTrans(  const MissingEtCalo_p1* pers,
				        MissingEtCalo* trans, 
				        MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading MissingEtCalo from persistent state..."   << endmsg;

  trans->setExCaloVec      (std::vector<double> (pers->m_exCalo));
  trans->setEyCaloVec      (std::vector<double> (pers->m_eyCalo));
  trans->setEtSumCaloVec   (std::vector<double> (pers->m_etSumCalo));
  trans->setNCellCaloVec   (std::vector<unsigned int> (pers->m_nCellsCalo));
  
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
				        MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of MissingEtCalo..."  << endmsg;

  pers->m_exCalo     = trans->exCaloVec();  
  pers->m_eyCalo     = trans->eyCaloVec(); 
  pers->m_etSumCalo  = trans->etSumCaloVec(); 
  pers->m_nCellsCalo = trans->ncellCaloVec();
  
  if( trans->getSource() >= 0 && trans->getSource() < 1000 )  {
    
    // use the MissingET converter to convert from trans to pers
    MissingET_p1 theMet;
    metCnv.transToPers( trans, &theMet, msg );
    pers->m_met = theMet;
     
  } 
  
    // for (unsigned int vi=0;vi<trans->m_exCalo.size();++vi)
    //      std::cout<<"OUT EtCalo ex: "<<trans->m_exCalo[vi]<<"\tey: "<<trans->m_eyCalo[vi]<<"\tet: "<<trans->m_etSumCalo[vi]<<"\tnCells: "<<trans->m_nCellsCalo[vi]<<std::endl;
  
 return;
}
