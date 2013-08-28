/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMooPatternForCalibrationTool.h
//   Header file for class IMooPatternForCalibrationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef IMOOPATTERNFORCALIBRATIONTOOL_H
#define IMOOPATTERNFORCALIBRATIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

class PhiSegment;
class RZSegmentCombination;
class MooRZSegment;

  
namespace MuonCalib {

  /** Interface ID for IMooPatternForCalibrationTool*/  
  static const InterfaceID IID_IMooPatternForCalibrationTool("MuonCalib::IMooPatternForCalibrationTool", 1, 0);
  
/**@class IMooPatternForCalibrationTool
   Interface class IMooPatternForCalibrationTool (obsolete)
     
   @author Niels.Van.Eldik@cern.ch, 
*/

  class IMooPatternForCalibrationTool : virtual public IAlgTool {
    
  public:
    /**Virtual destructor*/
    virtual ~IMooPatternForCalibrationTool(){};
    
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMooPatternForCalibrationTool; };
  
    /** Interface method: takes a PhiSegment and RZSegmentCombination and extracts the pattern */
    virtual void handlePattern( const RZSegmentCombination* rzCombi, double phi ) = 0;
    
    /** Called in MooMakeRoads. creates a MuonCalibPattern for a combination of a PhiSegment 
	and a MooRZSegment. 
    */
    virtual void handlePattern( const MooRZSegment* rz_seg, double phi) = 0;
    
    
    /** Store the recorded patterns in storegate */
    virtual void storePatterns() = 0;
    
    
  };

}


#endif //
