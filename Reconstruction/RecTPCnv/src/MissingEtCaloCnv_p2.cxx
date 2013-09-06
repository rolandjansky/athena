/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtCaloCnv_p2.cxx
PURPOSE:  Transient/Persisten converter for MissingEtCalo class
********************************************************************/

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
#include "RecTPCnv/MissingEtCaloCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p2.h"

// MissingET converter
static MissingETCnv_p2 metCnv;

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtCaloCnv_p2::persToTrans(  const MissingEtCalo_p2* pers, MissingEtCalo* trans,  MsgStream& /* msg */ ) {
	std::vector<float>::const_iterator i=pers->m_AllTheData.begin();
	int size=(int)(*i);++i;
	trans->m_exCalo.reserve(size);
	trans->m_eyCalo.reserve(size);
	trans->m_etSumCalo.reserve(size);
	trans->m_nCellsCalo.reserve(size);
	std::copy(i, i+size, trans->m_exCalo.begin() );i+=size;
	std::copy(i, i+size, trans->m_eyCalo.begin() );i+=size;
	std::copy(i, i+size, trans->m_etSumCalo.begin() );i+=size;
	union conv{  unsigned int i;   float f;  } c;
	for(int w=0;w<size;++w){
		c.f=(*i); 
		trans->m_nCellsCalo[w]=c.i;
		++i;
		}	
	metCnv.persToTrans(trans,i);

//	std::cout<<"READING MissingEtCalo"<<std::endl;
//  std::cout<<"READING: "<<size<<"\t"<<trans->m_exCalo[0]<<"\t"<<trans->m_eyCalo[6]<<"\t"<<trans->m_etSumCalo[3]<<"\t"<<trans->m_nCellsCalo[6]<<std::endl;
	
  return;
}

void MissingEtCaloCnv_p2::transToPers(  const MissingEtCalo* trans, MissingEtCalo_p2* pers, MsgStream& /* msg */ ){
	pers->m_AllTheData.push_back((float)trans->m_exCalo.size());
	std::copy(trans->m_exCalo.begin(), trans->m_exCalo.end(), std::back_inserter(pers->m_AllTheData) );
	std::copy(trans->m_eyCalo.begin(),trans->m_eyCalo.end(), std::back_inserter(pers->m_AllTheData) );
	std::copy(trans->m_etSumCalo.begin(),trans->m_etSumCalo.end(), std::back_inserter(pers->m_AllTheData) );
	union conv{  unsigned int i;   float f;  } c;
	for(unsigned int w=0;w<trans->m_exCalo.size();++w){
		c.i=trans->m_nCellsCalo[w]; 
		pers->m_AllTheData.push_back(c.f);
	}
	metCnv.transToPers( trans, pers->m_AllTheData );
	
//	std::cout<<"WRITING MissingEtCalo"<<std::endl;
//	std::cout<<"WRITING: "<<trans->m_exCalo.size()<<"\t"<<trans->m_exCalo[0]<<"\t"<<trans->m_eyCalo[6]<<"\t"<<trans->m_etSumCalo[3]<<"\t"<<trans->m_nCellsCalo[6]<<std::endl;
 return;
}
