// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigRoiDescriptor.h        
//
//             RoiDescriptor class - includes constructors in preparation
//             for enforcement of correct roi sizes - ingherits almost 
//             everything from DetectorDescription/RoiDescriptor       
//  
//   @author sutt@cern.ch
//
//   $Id: RoiDescriptor.h, v0.0   Fri 08 Jun 2013 23:52:09 CEST sutt $


#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <cmath>
#include <sstream>




TrigRoiDescriptor::TrigRoiDescriptor( bool fullscan )
  : RoiDescriptor( fullscan ), 
    m_l1Id(0), m_roiId(0), m_roiWord(0)
{ }



TrigRoiDescriptor::TrigRoiDescriptor(double eta, double etaMinus, double etaPlus, 
				     double phi, double phiMinus, double phiPlus, 
				     double zed, double zedMinus, double zedPlus) 
  : RoiDescriptor( eta, etaMinus, etaPlus, phi, phiMinus, phiPlus, zed, zedMinus, zedPlus ),
    m_l1Id(0), m_roiId(0), m_roiWord(0)
{ } 


TrigRoiDescriptor::TrigRoiDescriptor(unsigned int l1id, unsigned int id, 
				     double eta, double etaMinus, double etaPlus, 
				     double phi, double phiMinus, double phiPlus, 
				     double zed, double zedMinus, double zedPlus) 
  : RoiDescriptor( eta, etaMinus, etaPlus, phi, phiMinus, phiPlus, zed, zedMinus, zedPlus ),
    m_l1Id(l1id), m_roiId(id), m_roiWord(0)
{ }



TrigRoiDescriptor::TrigRoiDescriptor(unsigned int roiWord, unsigned int l1id, unsigned int id, 
				     double eta, double etaMinus, double etaPlus, 
				     double phi, double phiMinus, double phiPlus, 
				     double zed, double zedMinus, double zedPlus) 
  : RoiDescriptor( eta, etaMinus, etaPlus, phi, phiMinus, phiPlus, zed, zedMinus, zedPlus ),
    m_l1Id(l1id), m_roiId(id), m_roiWord(roiWord)
{ }



TrigRoiDescriptor::TrigRoiDescriptor( const IRoiDescriptor& roi ) 
  : RoiDescriptor( roi ),
    m_l1Id(roi.l1Id()), m_roiId(roi.roiId()), m_roiWord(roi.roiWord()) {  
}


TrigRoiDescriptor& TrigRoiDescriptor::operator=( const IRoiDescriptor& roi ) {
  if ( this==&roi ) return *this;

  m_l1Id    = roi.l1Id();
  m_roiId   = roi.roiId();
  m_roiWord = roi.roiWord();

  construct(roi);
  m_l1Id    = roi.l1Id();
  m_roiId   = roi.roiId();
  m_roiWord = roi.roiWord();

  if ( roi.size()>0 ) { 
    if ( m_manageConstituents ) { 
      /// manging it's own constituents, so take a deep copy
      for ( unsigned i=0 ; i<roi.size() ; i++ ) push_back( new TrigRoiDescriptor( *roi.at(i) ) );
    }
    else { 
      /// these are already managed elsewhere, just copy the pointers
      for ( unsigned i=0 ; i<roi.size() ; i++ ) push_back( roi.at(i) );      
    }
  }

  return *this;
}



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

