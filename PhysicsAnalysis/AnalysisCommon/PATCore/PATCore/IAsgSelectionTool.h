///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IAsgSelectionTool.h 
// Header file for class IAsgSelectionTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PATCORE_IASGSELECTIONTOOL_H
#define PATCORE_IASGSELECTIONTOOL_H 1

// STL includes


// FrameWork includes
#include "AsgTools/IAsgTool.h"

// Include the return object
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

// Forward declaration
namespace xAOD{
  class IParticle;
}


class  IAsgSelectionTool
  : virtual public asg::IAsgTool
{ 
  /// Declare the interface ID for this pure-virtual interface class to the Athena framework
  ASG_TOOL_INTERFACE(IAsgSelectionTool)


  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method to get the AcceptInfo to query what cuts are defined. */
  virtual const asg::AcceptInfo& getAcceptInfo( ) const = 0;


  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept( const xAOD::IParticle* /*part*/ ) const = 0;


}; 

#endif //> !PATCORE_IASGSELECTIONTOOL_H
