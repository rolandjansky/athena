/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_UPDATECALIBHIT_H
#define MUONCALIB_UPDATECALIBHIT_H

#include "MdtCalibInterfaces/IRtRelation.h"
#include "MuonCalibEvent/MdtCalibHit.h"

#include <iostream>

namespace MuonCalib {

/**
@class UpdateCalibHit
Provides the operator to update drift radius and error of a given 
MdtCalibHit using the pointer to the IRtRelation passed to the constructor. 
*/

  class UpdateCalibHit {
  public:
    UpdateCalibHit(const IRtRelation* rt ) : m_rt(rt) {}
  
    void operator()(MdtCalibHit* hit){
      if( m_rt ){
	//     std::cout << "UpdateCalibHit hit " << std::endl; 
	double t = hit->driftTime();
	//      std::cout << " updating " << t << "  -> " << m_rt->radius(t) << "  s " << m_rt->sigmaR(t) << std::endl;
	hit->setDriftRadius( m_rt->radius(t), m_rt->sigmaR(t) );
      }else{
	std::cout << "UpdateCalibHit failed, invalid rt " << std::endl;
      }
    }
  private:
    const IRtRelation* m_rt;
  };

}

#endif
