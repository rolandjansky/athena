/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_UPDATECALIBHIT_H
#define MUONCALIB_UPDATECALIBHIT_H

#include "MdtCalibInterfaces/IRtRelation.h"
#include "MuonCalibEvent/MdtCalibHit.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

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
	double t = hit->driftTime();
	hit->setDriftRadius( m_rt->radius(t), m_rt->sigmaR(t) );
      }else{
        MsgStream log(Athena::getMessageSvc(), "UpdateCalibHit");
        log<<MSG::WARNING<<"UpdateCalibHit failed, invalid rt"<<endmsg;
      }
    }
  private:
    const IRtRelation* m_rt;
  };

}

#endif
