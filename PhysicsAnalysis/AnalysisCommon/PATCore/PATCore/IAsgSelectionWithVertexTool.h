///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAsgSelectionWithVertexTool.h.h
// Header file for class IAsgSelectionWithVertexTool.h
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef PATCORE_IASGSELECTIONWITHVERTEXTOOL_H
#define PATCORE_IASGSELECTIONWITHVERTEXTOOL_H 1

// STL includes


// FrameWork includes
#include "AsgTools/IAsgTool.h"

// Include the return object
#include "PATCore/TAccept.h"

// Forward declaration
namespace xAOD{
  class IParticle;
  class Vertex_v1;
  typedef Vertex_v1 Vertex;
}



// this ought to be deprecated, but we have so many clients, that we
// are not doing it yet.
class [[deprecated("do not use for multi-threaded code")]] IAsgSelectionWithVertexTool
  : virtual public asg::IAsgTool
{
  /// Declare the interface ID for this pure-virtual interface class to the Athena framework
  ASG_TOOL_INTERFACE(IAsgSelectionWithVertexTool)


  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  /** Method to get the plain TAccept.
      This is needed so that one can already get the TAccept
      and query what cuts are defined before the first object
      is passed to the tool. */
  virtual const Root::TAccept& getTAccept( ) const = 0;


  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::IParticle* /*part*/,
                                       const xAOD::Vertex* /*vertex=0*/ ) const = 0;


};

#endif //> !PATCORE_IASGSELECTIONWITHVERTEXTOOL_H
