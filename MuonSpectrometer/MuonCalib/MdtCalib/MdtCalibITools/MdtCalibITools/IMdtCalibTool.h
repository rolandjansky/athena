/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMdtCalibTool.h
//   Header file for class IMdtCalibTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_IMDTCALIBTOOL_H
#define MUONCALIB_IMDTCALIBTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <string>

namespace MuonCalib {

  class IMdtCalibration;
  class IMdtCalibrationOutput;



  /** Interface ID for IMdtCalibTool*/  
  static const InterfaceID IID_IMdtCalibTool("MuonCalib::IMdtCalibTool", 1, 0);
  
  /**@class IMdtCalibTool
     Interface class IMdtCalibTool.
     Concrete implementation creates an instance of an IMdtCalibration implementation.
   
     @author Niels.Van.Eldik@cern.ch, 
  */
  class IMdtCalibTool : virtual public IAlgTool {
  
  public:
    /**Virtual destructor*/
    virtual ~IMdtCalibTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMdtCalibTool; };
  
    /** Interface method: return the IMdtCalibration pointer */
    virtual IMdtCalibration* getImp(std::string RegionKey) = 0;

    virtual bool writeToDb( const IMdtCalibrationOutput* result, std::string regionKey ) const = 0;
  };

}

#endif //
