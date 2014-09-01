// emacs: this is -*- c++ -*-
//
//   @file    TIDARoiDescriptorBuilder.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TIDARoiDescriptorBuilder.h, v0.0   Tue 10 Nov 2009 11:12:38 GMT sutt $


#ifndef __ROIDESCRIPTORBUILDER_H
#define __ROIDESCRIPTORBUILDER_H

#include <iostream>

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"


class TIDARoiDescriptorBuilder : public /*TrigInDetAnalysis::*/TIDARoiDescriptor {
  
public:

  TIDARoiDescriptorBuilder( const TrigRoiDescriptor& r ) : 
    TIDARoiDescriptor( r.roiWord(), r.l1Id(), r.roiId(), r.eta(), r.phi(), r.zed() ) 
  {
    m_phiHalfWidth = 0.5*std::fabs( HLT::wrapPhi( r.phiPlus() - r.phiMinus() ) );
    m_etaHalfWidth = 0.5*( r.etaPlus() - r.etaMinus() );
    m_zedHalfWidth = 0.5*( r.zedPlus() - r.zedMinus() );  
    m_etaPlus  = r.etaPlus();
    m_etaMinus = r.etaMinus(); 						
  }
  
  ~TIDARoiDescriptorBuilder() { } 

};

// inline std::ostream& operator<<( std::ostream& s, const TIDARoiDescriptorBuilder& _r ) { 
//    return s;
// }


#endif  // __ROIDESCRIPTORBUILDER_H 










