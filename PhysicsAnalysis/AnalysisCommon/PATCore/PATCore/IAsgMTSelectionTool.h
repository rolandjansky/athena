///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAsgSelectionTool.h 
// Header file for class IAsgSelectionTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PATCORE_IASGMTSELECTIONTOOL_H
#define PATCORE_IASGMTSELECTIONTOOL_H 1

// STL includes


// FrameWork includes
#include <AsgTools/IAsgTool.h>

// Include the return object
#include <PATCore/TAcceptInfo.h>
#include <PATCore/TAcceptData.h>

// Forward declaration
namespace xAOD{
  class IParticle;
}


//static const InterfaceID IID_IAsgMTSelectionTool("IAsgMTSelectionTool", 1, 0);

class IAsgMTSelectionTool
  : virtual public asg::IAsgTool
{ 
  /// Declare the interface ID for this pure-virtual interface class to the Athena framework
  ASG_TOOL_INTERFACE(IAsgMTSelectionTool)


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
  virtual const Root::TAcceptInfo& getAcceptInfo() const = 0;

  /** Backward compatibility method */
  [[deprecated("please use getAcceptInfo() instead")]]
  inline const Root::TAcceptInfo& getTAccept () const {
    return getAcceptInfo();};

  /** The main accept method: the actual cuts are applied here */
  virtual Root::TAcceptData accept( const xAOD::IParticle* /*part*/ ) const = 0;


}; 

#endif //> !PATCORE_IASGSELECTIONTOOL_H
