/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtRegionsCnv_p3.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Transient/Persisten converter for MissingEtRegions class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingEtRegions.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingEtRegionsCnv_p3.h"

#include <algorithm>

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////
// not used for now since there is no request to store it standalone.
void MissingEtRegionsCnv_p3::persToTrans(  const MissingEtRegions_p3* /* pers */, MissingEtRegions* /*trans*/, MsgStream& /*msg*/ ) {
  return;
}

// not used for now since there is no request to store it standalone.
void MissingEtRegionsCnv_p3::transToPers(  const MissingEtRegions* /*trans*/, MissingEtRegions_p3* /*pers*/, MsgStream& /*msg*/ ) {
 return;
}


void MissingEtRegionsCnv_p3::persToTrans(  MissingEtRegions* trans, std::vector<float>::const_iterator i ) {
	
    //float version;
    //version = (*i);
    ++i; // as there is just one it is not used.
    int size= static_cast<int>(*i);  ++i;
    
    for (int vi=0;vi<size;++vi){
	    trans->m_exReg[vi] = (*i); ++i;
	    trans->m_eyReg[vi] = (*i); ++i;
        trans->m_etReg[vi] = (*i); ++i;
    } 
    
    // for (unsigned int vi=0;vi<trans->m_exReg.size();++vi)
    //      std::cout<<"IN  regions ex: "<<trans->m_exReg[vi]<<"\tey: "<<trans->m_eyReg[vi]<<"\tet: "<<trans->m_etReg[vi]<<std::endl;
	     
  return;
}

// always adding at the end of the vector
void MissingEtRegionsCnv_p3::transToPers(  const MissingEtRegions* trans,  std::vector<float> &all )  {
    
    all.push_back(3.0); // storing version number
    all.push_back(MissingEtRegions::Size);
    for (int vi=0;vi<MissingEtRegions::Size;++vi){
    	all.push_back(trans->m_exReg[vi]);
    	all.push_back(trans->m_eyReg[vi]);
    	all.push_back(trans->m_etReg[vi]);   
	}
	
    // for (unsigned int vi=0;vi<trans->m_exReg.size();++vi)
    //      std::cout<<"OUT regions ex: "<<trans->m_exReg[vi]<<"\tey: "<<trans->m_eyReg[vi]<<"\tet: "<<trans->m_etReg[vi]<<std::endl;
	     
 return;
}

