/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_ITRACKTRUTHSELECTIONTOOL_H
#define INDETPHYSVALMONITORING_ITRACKTRUTHSELECTIONTOOL_H
/**
 * @file ITrackTruthSelectionTool.h
 * interface for truth track selection
 * @author lmijovic
 * @date Aug 2016
**/

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#include "xAODTruth/TruthParticle.h"

class ITrackTruthSelectionTool : public virtual asg::IAsgTool {
  
  // interface provided by this class:  
  ASG_TOOL_INTERFACE( ITrackTruthSelectionTool );
  
 public:
  // truth "track"/particle selection cuts:
  virtual const Root::TAccept& accept( const xAOD::TruthParticle *p ) const = 0;
    
  // set geometry-specific properties :
  // * inner detector max eta
  // * pixel detector radius ( used for cutting on secondaries )
  virtual void setGeoTrackTruthSelection( double p_maxEta, double p_pixRad ) = 0;
  
}; // end of class ITrackTruthSelectionTool

#endif // INDETPHYSVALMONITORING_ITRACKTRUTHSELECTIONTOOL_H
