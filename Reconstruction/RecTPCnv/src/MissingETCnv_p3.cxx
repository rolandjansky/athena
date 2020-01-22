/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingETCnv_p3.cxx
PACKAGE:  offline/Reconstruction/RecTPCnv

PURPOSE:  Transient/Persisten converter for MissingET class
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// MissingETEvent includes
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"

// RecTPCnv includes
#include "RecTPCnv/MissingETCnv_p3.h"
#include "RecTPCnv/MissingEtRegionsCnv_p3.h"


// #include <stdlib.h> // not needed here?

// region converter
static const MissingEtRegionsCnv_p3 regCnv;



void MissingETCnv_p3::persToTrans(  const MissingET_p3* pers, MissingET* trans, MsgStream& /* msg */ ) const {
	std::vector<float>::const_iterator it=pers->m_AllTheData.begin();
	persToTrans(trans, it);
  return;
}

void MissingETCnv_p3::transToPers(  const MissingET* trans, MissingET_p3* pers, MsgStream& /* msg */ ) const {
	transToPers(trans, pers->m_AllTheData);	
 return;
}


void MissingETCnv_p3::persToTrans( MissingET* trans, std::vector<float>::const_iterator i) const {
	
    //float version;
    //version = (*i);
    ++i; // as there is just one it is not used.
	
	union conv{  unsigned int i;   float f;  } c;
	c.f=(*i); ++i;
        MissingET::Source source = static_cast<MissingET::Source>(c.i>>1);
	double ex     = (*i);++i;
	double ey     = (*i);++i;
	double etSum  = (*i);++i;
        std::unique_ptr<MissingEtRegions> regions;
	if( c.i & 1) {
          regions = std::make_unique<MissingEtRegions>();
          regCnv.persToTrans( regions.get(), i);
	}
        *trans = MissingET (source,
                            std::move (regions),
                            ex, ey, etSum);
	
    // std::cout<<"IN  source: "<<trans->m_source;
    // std::cout<<"\tex: "<<trans->m_ex;
    // std::cout<<"\tey: "<<trans->m_ey;
    // std::cout<<"\tetSum: "<<trans->m_etSum;
    // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
	
	return;
}

void  MissingETCnv_p3::transToPers(  const MissingET* trans,  std::vector<float> &all ) const {
    
    all.push_back(3.0); // storing version number
    
    // folding together m_source and if regions are present
        unsigned int tmp=static_cast<unsigned int>(trans->getSource());
	tmp <<= 1;
        if( trans->getRegions() != 0) tmp|=1;
	
	union conv{  unsigned int i;   float f;  } c;
	c.i=tmp;
	all.push_back(c.f);
	
	// simple types
        all.push_back(trans->etx());
	all.push_back(trans->ety());
	all.push_back(trans->sumet());
	
	// calling regions tTP
	if( trans->getRegions() != 0) regCnv.transToPers(trans->getRegions(), all);
	
    // std::cout<<"OUT source: "<<trans->m_source;
    // std::cout<<"\tex: "<<trans->m_ex;
    // std::cout<<"\tey: "<<trans->m_ey;
    // std::cout<<"\tetSum: "<<trans->m_etSum;
    // if (trans->m_regions != 0) std::cout<<"\thas Regions"<<std::endl; else std::cout<<"\tNO Regions"<<std::endl;
    
	return;
}
