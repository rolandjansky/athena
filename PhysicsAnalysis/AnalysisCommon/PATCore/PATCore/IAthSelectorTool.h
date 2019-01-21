///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAthSelectorTool.h 
// Header file for class IAthSelectorTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PATCORE_IATHSELECTORTOOL_H
#define PATCORE_IATHSELECTORTOOL_H 1

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Include the return object
#include "PATCore/TAccept.h"

// Forward declaration
class INavigable4Momentum;


static const InterfaceID IID_IAthSelectorTool("IAthSelectorTool", 1, 0);

// this ought to be deprecated, but we have so many clients, that we
// are not doing it yet.
class [[deprecated("do not use for multi-threaded code")]] IAthSelectorTool
  : virtual public ::IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  //virtual ~IAthSelectorTool(){};

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();


  /** Method to get the plain TAccept */
  virtual const Root::TAccept& getTAccept( ) = 0;


  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const INavigable4Momentum* /*part*/ ) = 0;


  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IAthSelectorTool::interfaceID() 
{ 
   return IID_IAthSelectorTool; 
}


#endif //> !OBJECTSELECTORCORE_IATHSELECTORTOOL_H
