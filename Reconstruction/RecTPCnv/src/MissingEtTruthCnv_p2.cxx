/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtTruthCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p2.h"

// MissingET converter
static const MissingETCnv_p2 metCnv;

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtTruthCnv_p2::persToTrans( const MissingEtTruth_p2* pers, MissingEtTruth* trans, MsgStream& /* msg */) const {
	// std::cout << "Loading MissingEtTruth from persistent state..."<< std::endl;
	std::vector<float>::const_iterator i = pers->m_allTheData.begin();
        auto TI0 = static_cast<MissingEtTruth::TruthIndex>(0);
        auto TI1 = static_cast<MissingEtTruth::TruthIndex>(1);
        auto TI2 = static_cast<MissingEtTruth::TruthIndex>(2);
	trans->setExTruth(TI0, *i); ++i;
        trans->setExTruth(TI1, *i); ++i;
	trans->setExTruth(TI2, *i); ++i;
	trans->setEyTruth(TI0, *i); ++i;
	trans->setEyTruth(TI1, *i); ++i;
	trans->setEyTruth(TI2, *i); ++i;
	trans->setEtSumTruth(TI0, *i); ++i;
	trans->setEtSumTruth(TI1, *i); ++i;
	trans->setEtSumTruth(TI2, *i); ++i;

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

void MissingEtTruthCnv_p2::transToPers( const MissingEtTruth* trans, MissingEtTruth_p2* pers, MsgStream& /*msg*/ ) const {
	
	// std::cout << "Creating persistent state of MissingEtTruth..."<< std::endl;

	pers->m_allTheData.reserve(9);
	pers->m_allTheData.push_back(trans->exTruth(static_cast<MissingEtTruth::TruthIndex>(0)));
	pers->m_allTheData.push_back(trans->exTruth(static_cast<MissingEtTruth::TruthIndex>(1)));
	pers->m_allTheData.push_back(trans->exTruth(static_cast<MissingEtTruth::TruthIndex>(2)));
	pers->m_allTheData.push_back(trans->eyTruth(static_cast<MissingEtTruth::TruthIndex>(0)));
	pers->m_allTheData.push_back(trans->eyTruth(static_cast<MissingEtTruth::TruthIndex>(1)));
	pers->m_allTheData.push_back(trans->eyTruth(static_cast<MissingEtTruth::TruthIndex>(2)));
	pers->m_allTheData.push_back(trans->etSumTruth(static_cast<MissingEtTruth::TruthIndex>(0)));
	pers->m_allTheData.push_back(trans->etSumTruth(static_cast<MissingEtTruth::TruthIndex>(1)));
	pers->m_allTheData.push_back(trans->etSumTruth(static_cast<MissingEtTruth::TruthIndex>(2)));
	if( trans->getSource() >= 0 && trans->getSource() < 1000 ){
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
