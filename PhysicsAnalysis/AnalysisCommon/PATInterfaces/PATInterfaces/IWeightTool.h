///////////////////////// -*- C++ -*- /////////////////////////////
// Interface for a per-event weight computation
// The getWeight() function computes and returns the weight
//
// Author: Olivier Arnaez <olivier.arnaez@cern.ch>
///////////////////////////////////////////////////////////////////
// $Id: IWeightTool.h $
#ifndef PATINTERFACES_IWEIGHTTOOL_H
#define PATINTERFACES_IWEIGHTTOOL_H

// System include(s):

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// Forward declaration


/// Interface for tools that want to calculate a weight from different event information
/// @author Olivier Arnaez <olivier.arnaez@cern.ch>
class IWeightTool : virtual public asg::IAsgTool {

  /// Declare the interface that this class provides
  ASG_TOOL_INTERFACE( IWeightTool )

 public:

  /// Virtual destructor
  virtual ~IWeightTool(){};

  /// returns: the value that was calculated from the usual Athena storegate
  virtual double getWeight() const = 0;


 }; // class IWeightTool


#endif // PATINTERFACES_IWEIGHTTOOL_H
