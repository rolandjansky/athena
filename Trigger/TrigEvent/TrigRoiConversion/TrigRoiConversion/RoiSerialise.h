// emacs: this is -*- c++ -*-
//
//   @file    RoiSerialise.h        
//
//            utilities to serialise and deserialise to and from  
//            std::vector<IRoiDescriptor*> TrigRoiDescriptorCollection etc
//                   
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: serialise.h, v0.0   Sat 31 Oct 2015 08:21:54 CET sutt $

#ifndef ROIUTIL_SERIALISE_H
#define ROIUTIL_SERIALISE_H


#include <vector>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
 


namespace RoiUtil { 

  typedef uint32_t roitype_t;
  typedef std::vector< std::vector<roitype_t> > roiserial_type;

  /// serialise an entire vector of IRoiDescriptors
  void serialise( const std::vector<const IRoiDescriptor*>& rois, roiserial_type& s );
  
  /// serialise an entire TrigRoiDescriptor collection
  void serialise( const TrigRoiDescriptorCollection& rois, roiserial_type& s );
  

  /// deserialise uint32_t vector into a full vector of IRoiDescriptors 
  void deserialise( const roiserial_type& s, std::vector<const IRoiDescriptor*>& rois );

   /// deserialise uint32_t vector into a TrigRoiDescriptorCollection 
  void deserialise( const roiserial_type& s, TrigRoiDescriptorCollection& rois );

  /// serialise a single IRoiDescriptor
  void serialise( const IRoiDescriptor* roi, std::vector<roitype_t>& s );
  
  /// deserialise to a single IRoiDescriptors (including constituents) 
  IRoiDescriptor*  deserialise( const roitype_t*& s_end, const roitype_t* s_last );
  
}

#endif  // ROIUTIL_SERIALISE_H

