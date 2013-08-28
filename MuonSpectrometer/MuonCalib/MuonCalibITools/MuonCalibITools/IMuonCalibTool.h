/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCalibTool.h
//   Header file for class IMuonCalibTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_IMUONCALIBTOOL_H
#define MUONCALIB_IMUONCALIBTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <string>

#include "MuonCalibEventBase/MuonCalibEvent.h"

namespace MuonCalib {

  /** Interface ID for IMuonCalibTool*/  
  static const InterfaceID IID_IMuonCalibTool("MuonCalib::IMuonCalibTool", 1, 0);
  
  /**@class IMuonCalibTool
     Interface class IMuonCalibTool.
     Concrete implementations steer calibration for a give technology
   
     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */

  class IMuonCalibTool : virtual public IAlgTool {
  
  public:
    virtual ~IMuonCalibTool(){}; //!<Virtual destructor
 
    static const InterfaceID& interfaceID() { return IID_IMuonCalibTool; }; //!< AlgTool and IAlgTool interface methods 
  
    virtual bool handleEvent( const MuonCalibEvent* event ) = 0; //!< pure virtual Interface method: Feeds a MuonCalibEvent to the MuonCalibTool

    virtual bool analyse() = 0;  //!< pure virtual Interface method: perform analysis 
     
  };

}

#endif //
