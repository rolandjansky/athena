/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtTruthCnv_p2.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  I. Vukotic
CREATED:  May 2009

PURPOSE:  Transient/Persisten converter for MissingEtTruth class
********************************************************************/

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
#include "RecTPCnv/MissingEtTruthCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p2.h"

// MissingET converter
static MissingETCnv_p2 metCnv;

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtTruthCnv_p2::persToTrans( const MissingEtTruth_p2* pers, MissingEtTruth* trans, MsgStream& /* msg */) {
	// std::cout << "Loading MissingEtTruth from persistent state..."<< std::endl;
	std::vector<float>::const_iterator i = pers->m_allTheData.begin();
	trans->m_exTruth[0]    = (*i); ++i;
	trans->m_exTruth[1]    = (*i); ++i;
	trans->m_exTruth[2]    = (*i); ++i;
	trans->m_eyTruth[0]    = (*i); ++i;
	trans->m_eyTruth[1]    = (*i); ++i;
	trans->m_eyTruth[2]    = (*i); ++i;
	trans->m_etSumTruth[0] = (*i); ++i;
	trans->m_etSumTruth[1] = (*i); ++i;
	trans->m_etSumTruth[2] = (*i); ++i;
	if (pers->m_allTheData.size()>9) {
		metCnv.persToTrans(trans, i);
		// std::cout<<"==> "<<trans->m_ex<<"\t"<<trans->m_ey<<"\t"<<trans->m_etSum<<"\t";
		// if (trans->m_regions) 
		// 	std::cout<<"has regions"<<trans->m_regions->m_exReg[0]<<std::endl; 
		// else 
		// 	std::cout<<"no regions"<<std::endl;
	}
	// std::cout<<trans->m_exTruth[0]<<"\t"<<trans->m_exTruth[1]<<"\t"<<trans->m_exTruth[2]<<std::endl;
	// std::cout<<trans->m_etSumTruth[0]<<"\t"<<trans->m_etSumTruth[1]<<"\t"<<trans->m_etSumTruth[2]<<std::endl;
	// std::cout<<pers->m_allTheData.size()<<std::endl;
  return;
}

void MissingEtTruthCnv_p2::transToPers( const MissingEtTruth* trans, MissingEtTruth_p2* pers, MsgStream& /*msg*/ ) {
	
	// std::cout << "Creating persistent state of MissingEtTruth..."<< std::endl;

	pers->m_allTheData.reserve(9);
	pers->m_allTheData.push_back(trans->m_exTruth[0]);
	pers->m_allTheData.push_back(trans->m_exTruth[1]);
	pers->m_allTheData.push_back(trans->m_exTruth[2]);
	pers->m_allTheData.push_back(trans->m_eyTruth[0]);
	pers->m_allTheData.push_back(trans->m_eyTruth[1]);
	pers->m_allTheData.push_back(trans->m_eyTruth[2]);
	pers->m_allTheData.push_back(trans->m_etSumTruth[0]);
	pers->m_allTheData.push_back(trans->m_etSumTruth[1]);
	pers->m_allTheData.push_back(trans->m_etSumTruth[2]);
	if( trans->m_source >= 0 && trans->m_source < 1000 ){
		metCnv.transToPers(trans, pers->m_allTheData);
		// std::cout<<"==> "<<trans->m_ex<<"\t"<<trans->m_ey<<"\t"<<trans->m_etSum<<"\t";
		// if (trans->m_regions) 
		// 	std::cout<<"has regions"<<trans->m_regions->m_exReg[0]<<std::endl; 
		// else 
		// 	std::cout<<"no regions"<<std::endl;
	}
	// std::cout<<trans->m_exTruth[0]<<"\t"<<trans->m_exTruth[1]<<"\t"<<trans->m_exTruth[2]<<std::endl;
	// std::cout<<trans->m_etSumTruth[0]<<"\t"<<trans->m_etSumTruth[1]<<"\t"<<trans->m_etSumTruth[2]<<std::endl;
	// std::cout<<pers->m_allTheData.size()<<std::endl;
 return;
}
