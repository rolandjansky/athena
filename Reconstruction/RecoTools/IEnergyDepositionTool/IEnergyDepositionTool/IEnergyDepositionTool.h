/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEnergyDepositionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef RECOTOOLS_IENERGYDEPOSITIONTOOL_H
#define RECOTOOLS_IENERGYDEPOSITIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkParameters/TrackParameters.h"
// Calo
#include "CaloIdentifier/CaloCell_ID.h"
// STL
#include <utility>

/** Interface ID for IEnergyDepositionTool*/  
static const InterfaceID IID_IEnergyDepositionTool("IEnergyDepositionTool", 1, 0);
  
  /**@class IEnergyDepositionTool
     
     Interface class IEnergyDepositionTool. 

     This is a simple interface
     for different tools that may perform measurements of energy
     deposited by tracks in the calorimeters. Only one method
     is specified to not constrain any further the functionality
     of the tools implementing this interface.
     
     @author David.Lopez@cern.ch
    */

class IEnergyDepositionTool : virtual public IAlgTool {
      
 public:
  /**Virtual destructor*/
  virtual ~IEnergyDepositionTool(){}
       
  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IEnergyDepositionTool; }

  /** Returns the measurement (result.first) and the noise RMS
   (result.second)
  */
  virtual std::pair<double,double> measurement(CaloCell_ID::CaloSample id, 
					       const Trk::TrackParameters& parm) = 0;

};


#endif // RECOTOOLS_IENERGYDEPOSITIONTOOL_H
