///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IxAODClusterCompressor.h 
// Header file for class IxAODClusterCompressor
// Author: Walter Lampl
/////////////////////////////////////////////////////////////////// 
#ifndef CALOINTERFACES_IXAODCLUSTERCOMPRESSOR_H
#define CALOINTERFACES_IXAODCLUSTERCOMPRESSOR_H 1

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

#include "xAODCaloEvent/CaloClusterContainer.h"


static const InterfaceID IID_IxAODClusterCompressor("IxAODClusterCompressor", 1, 0);

class IxAODClusterCompressor
  : virtual public ::IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IxAODClusterCompressor() {};

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();


  virtual void compress(xAOD::CaloClusterContainer* clustercontainer) const=0;
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  

}; 


/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IxAODClusterCompressor::interfaceID() 
{ 
   return IID_IxAODClusterCompressor; 
}


#endif //> !CALOINTERFACES_IXAODCLUSTERCOMPRESSOR_H
