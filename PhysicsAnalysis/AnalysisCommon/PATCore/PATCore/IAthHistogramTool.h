///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAthHistogramTool.h 
// Header file for class IAthHistogramTool
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef PATCORE_IATHHISTOGRAMTOOL_H
#define PATCORE_IATHHISTOGRAMTOOL_H 1

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Forward declaration
//namespace Trk
//{
//  class VxCandidate;
//}


static const InterfaceID IID_IAthHistogramTool("IAthHistogramTool", 1, 0);

class IAthHistogramTool
  : virtual public ::IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  //virtual ~IAthHistogramTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// Main method to fill histograms
  virtual StatusCode fill( const double weight = 1.0) = 0;

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
inline const InterfaceID& IAthHistogramTool::interfaceID() 
{ 
   return IID_IAthHistogramTool; 
}


#endif //> !PATCORE_IATHHISTOGRAMTOOL_H
