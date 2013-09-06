/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingETCnv_p3.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

PURPOSE:  Transient/Persisten converter for MissingET class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingETCnv_p3.h"
#include "RecTPCnv/MissingEtRegionsCnv_p3.h"


// #include <stdlib.h> // not needed here?

// region converter
static MissingEtRegionsCnv_p3 regCnv;


/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingETCnv_p3::persToTrans(  const MissingET_p3* pers, MissingET* trans, MsgStream& /* msg */ ) {
	std::vector<float>::const_iterator it=pers->m_AllTheData.begin();
	persToTrans(trans, it);
  return;
}

void MissingETCnv_p3::transToPers(  const MissingET* trans, MissingET_p3* pers, MsgStream& /* msg */ ) {
	transToPers(trans, pers->m_AllTheData);	
 return;
}


void MissingETCnv_p3::persToTrans( MissingET* trans, std::vector<float>::const_iterator i) {
	
    //float version;
    //version = (*i);
    ++i; // as there is just one it is not used.
	
	union conv{  unsigned int i;   float f;  } c;
	c.f=(*i); ++i;
	trans->m_source = static_cast<MissingET::Source>(c.i>>1);
	trans->m_ex     = (*i);++i;
	trans->m_ey     = (*i);++i;
	trans->m_etSum  = (*i);++i;
	delete trans->m_regions; // it's a bit crazy that this is always created in MissingET constructor and recreated here.
	if( c.i & 1) {
		MissingEtRegions *theReg = new MissingEtRegions();
		regCnv.persToTrans( theReg, i);
		trans->m_regions = theReg;
	}
	
    // std::cout<<"IN  source: "<<trans->m_source;
    // std::cout<<"\tex: "<<trans->m_ex;
    // std::cout<<"\tey: "<<trans->m_ey;
    // std::cout<<"\tetSum: "<<trans->m_etSum;
    // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
	
	return;
}

void  MissingETCnv_p3::transToPers(  const MissingET* trans,  std::vector<float> &all ) {
    
    all.push_back(3.0); // storing version number
    
    // folding together m_source and if regions are present
	unsigned int tmp=static_cast<unsigned int>(trans->m_source);
	tmp <<= 1;
	if( trans->m_regions != 0) tmp|=1;
	
	union conv{  unsigned int i;   float f;  } c;
	c.i=tmp;
	all.push_back(c.f);
	
	// simple types
	all.push_back(trans->m_ex);
	all.push_back(trans->m_ey);
	all.push_back(trans->m_etSum);
	
	// calling regions tTP
	if( trans->m_regions != 0) regCnv.transToPers(trans->m_regions, all);
	
    // std::cout<<"OUT source: "<<trans->m_source;
    // std::cout<<"\tex: "<<trans->m_ex;
    // std::cout<<"\tey: "<<trans->m_ey;
    // std::cout<<"\tetSum: "<<trans->m_etSum;
    // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
    
	return;
}
