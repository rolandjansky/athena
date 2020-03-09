/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtCaloCnv_p2.cxx
PURPOSE:  Transient/Persisten converter for MissingEtCalo class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtCaloCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p2.h"

// MissingET converter
static const MissingETCnv_p2 metCnv;


void MissingEtCaloCnv_p2::persToTrans(  const MissingEtCalo_p2* pers, MissingEtCalo* trans,  MsgStream& /* msg */ ) const {
	std::vector<float>::const_iterator i=pers->m_AllTheData.begin();
	int size=(int)(*i);++i;
        trans->setExCaloVec (std::vector<double> (i, i+size));  i+= size;
        trans->setEyCaloVec (std::vector<double> (i, i+size));  i+= size;
        trans->setEtSumCaloVec (std::vector<double> (i, i+size));  i+= size;
	union conv{  unsigned int i;   float f;  } c;
	for(int w=0;w<size;++w){
		c.f=(*i);
                trans->setNCellCalo(static_cast<MissingEtCalo::CaloIndex>(w), c.i);
		++i;
		}	
	metCnv.persToTrans(trans,i);

//	std::cout<<"READING MissingEtCalo"<<std::endl;
//  std::cout<<"READING: "<<size<<"\t"<<trans->m_exCalo[0]<<"\t"<<trans->m_eyCalo[6]<<"\t"<<trans->m_etSumCalo[3]<<"\t"<<trans->m_nCellsCalo[6]<<std::endl;
	
  return;
}

void MissingEtCaloCnv_p2::transToPers(  const MissingEtCalo* trans, MissingEtCalo_p2* pers, MsgStream& /* msg */ ) const {
        pers->m_AllTheData.push_back((float)trans->exCaloVec().size());
	std::copy(trans->exCaloVec().begin(), trans->exCaloVec().end(), std::back_inserter(pers->m_AllTheData) );
	std::copy(trans->eyCaloVec().begin(),trans->eyCaloVec().end(), std::back_inserter(pers->m_AllTheData) );
	std::copy(trans->etSumCaloVec().begin(),trans->etSumCaloVec().end(), std::back_inserter(pers->m_AllTheData) );
	union conv{  unsigned int i;   float f;  } c;
	for(unsigned int w=0;w<trans->exCaloVec().size();++w){
                c.i=trans->ncellCalo(static_cast<MissingEtCalo::CaloIndex>(w));
		pers->m_AllTheData.push_back(c.f);
	}
	metCnv.transToPers( trans, pers->m_AllTheData );
	
//	std::cout<<"WRITING MissingEtCalo"<<std::endl;
//	std::cout<<"WRITING: "<<trans->m_exCalo.size()<<"\t"<<trans->m_exCalo[0]<<"\t"<<trans->m_eyCalo[6]<<"\t"<<trans->m_etSumCalo[3]<<"\t"<<trans->m_nCellsCalo[6]<<std::endl;
 return;
}
