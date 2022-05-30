/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtTruthCnv_p3.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Transient/Persisten converter for MissingEtTruth class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtTruthCnv_p3.h"
#include "RecTPCnv/MissingETCnv_p3.h"

// MissingET converter
static const MissingETCnv_p3 metCnv;


void MissingEtTruthCnv_p3::persToTrans( const MissingEtTruth_p3* pers, MissingEtTruth* trans, MsgStream& /* msg */) const {
	// std::cout << "Loading MissingEtTruth from persistent state..."<< std::endl;
	
    std::vector<float>::const_iterator i = pers->m_allTheData.begin();
    
    //float version;
    //version = (*i);
    ++i; // as there is just one it is not used.
    
    int size= static_cast<int>(*i);  ++i;
    for (int vi=0;vi<size;++vi){        
        auto ti = static_cast<MissingEtTruth::TruthIndex>(vi);
        trans->setExTruth(ti, *i); ++i;
        trans->setEyTruth(ti, *i); ++i;
        trans->setEtSumTruth(ti, *i); ++i;
	}
	
	if ( i != pers->m_allTheData.end()) {
		metCnv.persToTrans(trans, i);
	}
	
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"IN  EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;
	     
  return;
}



void MissingEtTruthCnv_p3::transToPers( const MissingEtTruth* trans, MissingEtTruth_p3* pers, MsgStream& /*msg*/ ) const {
	
	// std::cout << "Creating persistent state of MissingEtTruth..."<< std::endl;
	
    pers->m_allTheData.push_back(3.0); // storing version number
    
	pers->m_allTheData.reserve(MissingEtTruth::Size * 3 + 1);
	
    pers->m_allTheData.push_back(MissingEtTruth::Size);
    for (int vi=0;vi<MissingEtTruth::Size;++vi){        
        auto ti = static_cast<MissingEtTruth::TruthIndex>(vi);
    	pers->m_allTheData.push_back(trans->exTruth(ti));
    	pers->m_allTheData.push_back(trans->eyTruth(ti));
    	pers->m_allTheData.push_back(trans->etSumTruth(ti));
    }
    
    if( trans->getSource() >= 0 && trans->getSource() < 1000 ){
      metCnv.transToPers(trans, pers->m_allTheData);
    }
	
    // for (unsigned int vi=0;vi<trans->m_exTruth.size();++vi)
    //      std::cout<<"OUT EtTruth ex: "<<trans->m_exTruth[vi]<<"\tey: "<<trans->m_eyTruth[vi]<<"\tet: "<<trans->m_etSumTruth[vi]<<std::endl;

 return;
}
