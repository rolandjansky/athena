/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
// emacs: this is -*- c++ -*-
//
//   @file    TrigRoiDescriptor.h        
//
//             RoiDescriptor class - includes constructors in preparation
//             for enforcement of correct roi sizes - ingherits almost 
//             everything from DetectorDescription/RoiDescriptor       
//  
//   @author sutt@cern.ch
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiDescriptor.h, v0.0   Fri 08 Jun 2013 23:52:09 CEST sutt $


#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include <cmath>
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/StatusCode.h"




/// non member, non friend interface function 
/// test whether a stub is contained within the roi                                                                                                                                                                  
bool roiContainsZed( const IRoiDescriptor& roi, double _z, double _r ) {
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( roiContainsZed( *roi.at(i), _z, _r ) ) return true;
    return false;
  }
  if ( roi.isFullscan() ) return true;
  double zminus = _r*roi.dzdrMinus()+roi.zedMinus();
  double zplus  = _r*roi.dzdrPlus() +roi.zedPlus();
  return ( _z>=zminus && _z<=zplus );
}



bool roiContains( const IRoiDescriptor& roi, double z, double r, double phi) { 
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( roiContains( *roi.at(i), z, r, phi ) ) return true;
    return false;
  } 
  if ( roi.isFullscan() ) return true;
  return ( roiContainsZed( roi, z, r ) && roi.containsPhi(phi) ); 
}



TrigRoiDescriptor::TrigRoiDescriptor( bool fullscan )
  : RoiDescriptor( fullscan ), 
    m_l1Id(0), m_roiId(0), m_roiWord(0)
{ }



TrigRoiDescriptor::TrigRoiDescriptor(double _eta, double _etaMinus, double _etaPlus, 
				     double _phi, double _phiMinus, double _phiPlus, 
				     double _zed, double _zedMinus, double _zedPlus) 
  : RoiDescriptor( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus ),
    m_l1Id(0), m_roiId(0), m_roiWord(0)
{ } 


TrigRoiDescriptor::TrigRoiDescriptor(unsigned int l1id, unsigned int id, 
				     double _eta, double _etaMinus, double _etaPlus, 
				     double _phi, double _phiMinus, double _phiPlus, 
				     double _zed, double _zedMinus, double _zedPlus) 
  : RoiDescriptor( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus ),
    m_l1Id(l1id), m_roiId(id), m_roiWord(0)
{ }



TrigRoiDescriptor::TrigRoiDescriptor(unsigned int roiWord, unsigned int l1id, unsigned int id, 
				     double _eta, double _etaMinus, double _etaPlus, 
				     double _phi, double _phiMinus, double _phiPlus, 
				     double _zed, double _zedMinus, double _zedPlus) 
  : RoiDescriptor( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus ),
    m_l1Id(l1id), m_roiId(id), m_roiWord(roiWord)
{ }




TrigRoiDescriptor::~TrigRoiDescriptor() { }


  

//////////////////////////////////////////////////////////////////
// helper operators

TrigRoiDescriptor::operator std::string() const {
  std::stringstream ss;
  ss  << " RoIid: " << roiId() << " RoIword: " << roiWord() << "\t" << RoiDescriptor::operator::std::string();
  return ss.str();
}

std::string str( const TrigRoiDescriptor& d ) {
  return std::string(d);
}

std::ostream& operator<<( std::ostream& m, const TrigRoiDescriptor& d ) {
  return m << std::string(d);
}

#endif
