/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtCaloCnv_p3.cxx
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
#include "RecTPCnv/MissingEtCaloCnv_p3.h"
#include "RecTPCnv/MissingETCnv_p3.h"

// MissingET converter
static MissingETCnv_p3 metCnv;

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingEtCaloCnv_p3::persToTrans(  const MissingEtCalo_p3* pers, MissingEtCalo* trans,  MsgStream& /* msg */ ) {
    
//    std::cout << "Loading MissingEtCalo from persistent state..."<< std::endl;
    std::vector<float>::const_iterator i=pers->m_allTheData.begin();

    union conv{  unsigned int i;   float f;  } c;
	
    //float version;
    //version = (*i); 
    ++i; // as there is just one it is not used.
    
    int size= static_cast<int>(*i);  ++i;
    for (int vi=0;vi<size;++vi){        
    	trans->m_exCalo[vi]    = (*i); ++i;
    	trans->m_eyCalo[vi]    = (*i); ++i;
    	trans->m_etSumCalo[vi] = (*i); ++i;
    	c.f=(*i); trans->m_nCellsCalo[vi]=c.i; ++i;
	}
	
	if ( i != pers->m_allTheData.end()) {
		metCnv.persToTrans(trans, i);
	}
	
//	for (unsigned int vi=0;vi<trans->m_exCalo.size();++vi)
//	     std::cout<<"IN  EtCalo ex: "<<trans->m_exCalo[vi]<<"\tey: "<<trans->m_eyCalo[vi]<<"\tet: "<<trans->m_etSumCalo[vi]<<"\tnCells: "<<trans->m_nCellsCalo[vi]<<std::endl;

  return;
}

void MissingEtCaloCnv_p3::transToPers(  const MissingEtCalo* trans, MissingEtCalo_p3* pers, MsgStream& /* msg */ ){
    
//    std::cout << "Creating persistent state of MissingEtCalo..."<< std::endl;
	
    union conv{  unsigned int i;   float f;  } c;
	
    pers->m_allTheData.push_back(3.0); // storing version number
    
    pers->m_allTheData.reserve(MissingEtCalo::Size * 4 + 1);
	
    pers->m_allTheData.push_back(MissingEtCalo::Size);
    
    for (int vi=0;vi<MissingEtCalo::Size;++vi){        
    	pers->m_allTheData.push_back(trans->m_exCalo[vi]);
    	pers->m_allTheData.push_back(trans->m_eyCalo[vi]);
    	pers->m_allTheData.push_back(trans->m_etSumCalo[vi]);	
    	c.i=trans->m_nCellsCalo[vi]; pers->m_allTheData.push_back(c.f);
    }
    
    if( trans->m_source >= 0 && trans->m_source < 1000 ) 
	    metCnv.transToPers( trans, pers->m_allTheData );

//	for (unsigned int vi=0;vi<trans->m_exCalo.size();++vi)
//	     std::cout<<"OUT EtCalo ex: "<<trans->m_exCalo[vi]<<"\tey: "<<trans->m_eyCalo[vi]<<"\tet: "<<trans->m_etSumCalo[vi]<<"\tnCells: "<<trans->m_nCellsCalo[vi]<<std::endl;

 return;
}
