// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TIDARoiDescriptorBuilder.h        
//
//                   
// 
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

  TIDARoiDescriptorBuilder( const IRoiDescriptor& r ) : 
    TIDARoiDescriptor( r.roiWord(), r.l1Id(), r.roiId(), 
		       r.eta(), r.etaMinus(), r.etaPlus(), 
		       r.phi(), r.phiMinus(), r.phiPlus(), 
		       r.zed(), r.zedMinus(), r.zedPlus() )  
  {

    this->m_fullscan =  r.isFullscan();
    
    if ( r.size()>0 ) { 
      for ( unsigned i=0 ; i<r.size() ; i++ ) { 
	const IRoiDescriptor* _r = r.at(i);
	this->push_back(  TIDARoiDescriptor( _r->eta(), _r->etaMinus(), _r->etaPlus(), 
					     _r->phi(), _r->phiMinus(), _r->phiPlus(), 
					     _r->zed(), _r->zedMinus(), _r->zedPlus() ) );  
      }
    }
  }
  
  ~TIDARoiDescriptorBuilder() { } 

};

// inline std::ostream& operator<<( std::ostream& s, const TIDARoiDescriptorBuilder& _r ) { 
//    return s;
// }


#endif  // __ROIDESCRIPTORBUILDER_H 










