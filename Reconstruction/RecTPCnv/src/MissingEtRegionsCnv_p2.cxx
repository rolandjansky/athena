/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtRegionsCnv_p2.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

PURPOSE:  Transient/Persisten converter for MissingEtRegions class
********************************************************************/


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#define private public
#define protected public
#include "MissingETEvent/MissingEtRegions.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MissingEtRegionsCnv_p2.h"

//#include <stdlib.h>
#include <algorithm>

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////
// not used for now since there is no request to store it standalone.
void MissingEtRegionsCnv_p2::persToTrans(  const MissingEtRegions_p2* /* pers */, MissingEtRegions* /*trans*/, MsgStream& /*msg*/ ) {
  return;
}

// not used for now since there is no request to store it standalone.
void MissingEtRegionsCnv_p2::transToPers(  const MissingEtRegions* /*trans*/, MissingEtRegions_p2* /*pers*/, MsgStream& /*msg*/ ) {
 return;
}


void MissingEtRegionsCnv_p2::persToTrans(  MissingEtRegions* trans, std::vector<float>::const_iterator i ) {
	
	std::copy(i, i+3, trans->m_exReg.begin()); i+=3;
	std::copy(i, i+3, trans->m_eyReg.begin()); i+=3;
	std::copy(i, i+3, trans->m_etReg.begin()); 
	// std::cout<<"READING: ex: "<<trans->m_exReg[0]<<"\t et: "<<trans->m_etReg[2]<<std::endl;
  return;
}

// always adding at the end of the vector
void MissingEtRegionsCnv_p2::transToPers(  const MissingEtRegions* trans,  std::vector<float> &all )  {
	all.push_back(trans->m_exReg[0]);
	all.push_back(trans->m_exReg[1]);
	all.push_back(trans->m_exReg[2]);
	all.push_back(trans->m_eyReg[0]);
	all.push_back(trans->m_eyReg[1]);
	all.push_back(trans->m_eyReg[2]);
	all.push_back(trans->m_etReg[0]);
	all.push_back(trans->m_etReg[1]);
	all.push_back(trans->m_etReg[2]);
	// std::cout<<"WRITING: ex: "<<trans->m_exReg[0]<<"\t et: "<<trans->m_etReg[2]<<std::endl;
 return;
}

