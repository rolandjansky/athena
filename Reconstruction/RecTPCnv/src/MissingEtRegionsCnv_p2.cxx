/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "MissingETEvent/MissingEtRegions.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtRegionsCnv_p2.h"

//#include <stdlib.h>
#include <algorithm>

/////////////////////////////////////////////////////////////////// 
// methods: 
///////////////////////////////////////////////////////////////////
// not used for now since there is no request to store it standalone.
void MissingEtRegionsCnv_p2::persToTrans(  const MissingEtRegions_p2* /* pers */, MissingEtRegions* /*trans*/, MsgStream& /*msg*/ ) const {
  return;
}

// not used for now since there is no request to store it standalone.
void MissingEtRegionsCnv_p2::transToPers(  const MissingEtRegions* /*trans*/, MissingEtRegions_p2* /*pers*/, MsgStream& /*msg*/ ) const {
 return;
}


void MissingEtRegionsCnv_p2::persToTrans(  MissingEtRegions* trans, std::vector<float>::const_iterator i ) const {

        trans->setExRegVec (std::vector<double> (i, i+3));  i += 3;
        trans->setEyRegVec (std::vector<double> (i, i+3));  i += 3;
        trans->setEtSumRegVec (std::vector<double> (i, i+3));  i += 3;

	// std::cout<<"READING: ex: "<<trans->m_exReg[0]<<"\t et: "<<trans->m_etReg[2]<<std::endl;
  return;
}

// always adding at the end of the vector
void MissingEtRegionsCnv_p2::transToPers(  const MissingEtRegions* trans,  std::vector<float> &all )  const {
  all.push_back(trans->exReg(static_cast<MissingEtRegions::RegionIndex>(0)));
  all.push_back(trans->exReg(static_cast<MissingEtRegions::RegionIndex>(1)));
  all.push_back(trans->exReg(static_cast<MissingEtRegions::RegionIndex>(2)));
  all.push_back(trans->eyReg(static_cast<MissingEtRegions::RegionIndex>(0)));
  all.push_back(trans->eyReg(static_cast<MissingEtRegions::RegionIndex>(1)));
  all.push_back(trans->eyReg(static_cast<MissingEtRegions::RegionIndex>(2)));
  all.push_back(trans->etSumReg(static_cast<MissingEtRegions::RegionIndex>(0)));
  all.push_back(trans->etSumReg(static_cast<MissingEtRegions::RegionIndex>(1)));
  all.push_back(trans->etSumReg(static_cast<MissingEtRegions::RegionIndex>(2)));
  // std::cout<<"WRITING: ex: "<<trans->m_exReg[0]<<"\t et: "<<trans->m_etReg[2]<<std::endl;
 return;
}

