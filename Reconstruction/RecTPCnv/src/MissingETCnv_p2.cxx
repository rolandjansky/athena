/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingETCnv_p2.cxx
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
#include "RecTPCnv/MissingETCnv_p2.h"
#include "RecTPCnv/MissingEtRegionsCnv_p2.h"


#include <stdlib.h>

// region converter
static MissingEtRegionsCnv_p2 regCnv;


/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////

void MissingETCnv_p2::persToTrans(  const MissingET_p2* pers, MissingET* trans, MsgStream& /* msg */ ) {
	std::vector<float>::const_iterator it=pers->m_AllTheData.begin();
	persToTrans(trans, it);
  return;
}

void MissingETCnv_p2::transToPers(  const MissingET* trans, MissingET_p2* pers, MsgStream& /* msg */ ) {
	transToPers(trans, pers->m_AllTheData);	
 return;
}


void MissingETCnv_p2::persToTrans( MissingET* trans, std::vector<float>::const_iterator i) {
	union conv{  unsigned int i;   float f;  } c;
	c.f=(*i); ++i;
	trans->m_source = static_cast<MissingET::Source>(c.i>>1);
	trans->m_ex     = (*i);++i;
	trans->m_ey     = (*i);++i;
	trans->m_etSum  = (*i);++i;
	delete trans->m_regions;
	if( c.i & 1) {
		MissingEtRegions *theReg = new MissingEtRegions();
		regCnv.persToTrans( theReg, i);
		trans->m_regions = theReg;
	}
	// std::cout<<"READING: source: "<<trans->m_source<<"\t et: "<<trans->m_etSum;
	// if (trans->m_regions != 0) std::cout<<"\t has Regions"<<std::endl; else std::cout<<"\t NO Regions"<<std::endl;
	return;
}

void  MissingETCnv_p2::transToPers(  const MissingET* trans,  std::vector<float> &all ) {
	unsigned int tmp=static_cast<unsigned int>(trans->m_source);
	tmp <<= 1;
	if( trans->m_regions != 0) tmp|=1;
	union conv{  unsigned int i;   float f;  } c;
	c.i=tmp;
	all.push_back(c.f);
	all.push_back(trans->m_ex);
	all.push_back(trans->m_ey);
	all.push_back(trans->m_etSum);
	if( trans->m_regions != 0) regCnv.transToPers(trans->m_regions, all);
	// std::cout<<"WRITING: source: "<<trans->m_source<<"\t et: "<<trans->m_etSum;
	// if (trans->m_regions != 0) std::cout<<"\t has Regions"<<std::endl; else std::cout<<"\t NO Regions"<<std::endl;
	return;
}
