// emacs: this is -*- c++ -*-
//
//   @file    RoiWriter.h        
//
//                   
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiWriter.h, v0.0   Wed 18 Nov 2015 12:20:40 CET sutt $


#ifndef  ROIWRITER_H
#define  ROIWRITER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

/// RoiDescriptor collection
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


class RoiWriter : public AthAlgorithm {

public:

  RoiWriter( const std::string& name, ISvcLocator* pSvcLocator ); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  virtual ~RoiWriter() { } 

  void deserialiser( TrigRoiDescriptorCollection& collection, 
  		     const std::string key ) const;

};



#endif  // ROIWRITER_H 










