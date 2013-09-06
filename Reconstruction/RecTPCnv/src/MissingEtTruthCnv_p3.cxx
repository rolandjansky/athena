/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtTruthCnv_p3.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv
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
#include "RecTPCnv/MissingEtTruthCnv_p3.h"
#include "RecTPCnv/MissingETCnv_p3.h"

// MissingET converter
static MissingETCnv_p3 metCnv;


void MissingEtTruthCnv_p3::persToTrans( const MissingEtTruth_p3* pers, MissingEtTruth* trans, MsgStream& /* msg */) {
	// std::cout << "Loading MissingEtTruth from persistent state..."<< std::endl;
	
    std::vector<float>::const_iterator i = pers->m_allTheData.begin();
    
    //float version;
    //version = (*i);
    ++i; // as there is just one it is not used.
    
    int size= static_cast<int>(*i);  ++i;
    for (int vi=0;vi<size;++vi){        
    	trans->m_exTruth[vi]    = (*i); ++i;
    	trans->m_eyTruth[vi]    = (*i); ++i;
    	trans->m_etSumTruth[vi] = (*i); ++i;
	}
	
	if ( i != pers->m_allTheData.end()) {
		metCnv.persToTrans(trans, i);
	}
	
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"IN  EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;
	     
  return;
}



void MissingEtTruthCnv_p3::transToPers( const MissingEtTruth* trans, MissingEtTruth_p3* pers, MsgStream& /*msg*/ ) {
	
	// std::cout << "Creating persistent state of MissingEtTruth..."<< std::endl;
	
    pers->m_allTheData.push_back(3.0); // storing version number
    
	pers->m_allTheData.reserve(MissingEtTruth::Size * 3 + 1);
	
    pers->m_allTheData.push_back(MissingEtTruth::Size);
    for (int vi=0;vi<MissingEtTruth::Size;++vi){        
    	pers->m_allTheData.push_back(trans->m_exTruth[vi]);
    	pers->m_allTheData.push_back(trans->m_eyTruth[vi]);
    	pers->m_allTheData.push_back(trans->m_etSumTruth[vi]);
    }
    
	if( trans->m_source >= 0 && trans->m_source < 1000 ){
		metCnv.transToPers(trans, pers->m_allTheData);
	}
	
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"OUT EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;

 return;
}
