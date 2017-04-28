// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IObservableTool.h 782989 2016-11-08 14:27:55Z will $
#ifndef ASGANALYSISINTERFACES_IOBSERVABLETOOL_H
#define ASGANALYSISINTERFACES_IOBSERVABLETOOL_H

// System include(s):

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// Forward declaration
namespace xAOD{
  class IParticle;
}


/// Interface for tools that want to calculate a quantity
/// from an xAOD::IParticle
/// @author Karsten Koeneke <karsten.koeneke@cern.ch>
///


///Main tools implementing this interface:
///  GenericParticleScaleFactorTool   (source code: PhysicsAnalysis/AnalysisCommon/ReweightUtils)
///  McEventWeight                    (source code: PhysicsAnalysis/AnalysisCommon/ReweightUtils)

class IObservableTool : public virtual asg::IAsgTool {

  /// Declare the interface that this class provides
  ASG_TOOL_INTERFACE( IObservableTool )

 public:

  /// Virtual destructor
  virtual ~IObservableTool(){};

  /// returns: the value that was calculated from the xAOD::IParticle
  virtual double
  evaluate( const xAOD::IParticle* ) const = 0;

 }; // class IObservableTool


#endif // PATINTERFACES_IOBSERVABLETOOL_H
