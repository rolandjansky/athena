/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGROICONVERSION_ROIREADER_H
#define TRIGROICONVERSION_ROIREADER_H

#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"

#include "TrigRoiConversion/IRoiReader.h"

/// RoiDescriptor collection
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


class RoiReader: public virtual IRoiReader, public asg::AsgTool {

  ASG_TOOL_CLASS( RoiReader, IRoiReader )
  
public:

  RoiReader( const std::string& name );

  virtual ~RoiReader() { } 

  virtual void execute( std::vector<std::string>& keys ) override;

  void deserialiser( TrigRoiDescriptorCollection& collection,
                     const std::string key ) const;

};



#endif
