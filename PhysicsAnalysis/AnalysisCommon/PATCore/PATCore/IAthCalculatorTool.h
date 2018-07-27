///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAthCalculatorTool.h 
// Header file for class IAthCalculatorTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PATCORE_IATHCALCULATORTOOL_H
#define PATCORE_IATHCALCULATORTOOL_H 1

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Include the return object
#include "PATCore/TResult.h"

// Forward declaration
class INavigable4Momentum;


static const InterfaceID IID_IAthCalculatorTool("IAthCalculatorTool", 1, 0);


class [[deprecated("do not use for multi-threaded code")]] IAthCalculatorTool
  : virtual public ::IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  //virtual ~IAthCalculatorTool(){};

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();


  /** Method to get the plain TAccept */
  virtual const Root::TResult& getTResult( ) = 0;


  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TResult& calculate( const INavigable4Momentum* /*part*/ ) = 0;


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
inline const InterfaceID& IAthCalculatorTool::interfaceID() 
{ 
   return IID_IAthCalculatorTool; 
}


#endif //> !PATCORE_IATHSELECTORTOOL_H
