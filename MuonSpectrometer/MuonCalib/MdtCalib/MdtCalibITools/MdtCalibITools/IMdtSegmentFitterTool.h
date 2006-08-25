/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMdtSegmentFitterTool.h
//   Header file for class IMdtSegmentFitterTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_IMDTSEGMENTFITTERTOOL_H
#define MUONCALIB_IMDTSEGMENTFITTERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace MuonCalib {

  class IMdtSegmentFitter;


  /** Interface ID for IMdtSegmentFitterTool */  
  static const InterfaceID IID_IMdtSegmentFitterTool("MuonCalib::IMdtSegmentFitterTool", 1, 0);
  
  /**@class IMdtSegmentFitterTool
     Interface class IMdtSegmentFitterTool.
     Implementations create an instance of a IMdtSegmentFitter implementation
  
     @author Niels.Van.Eldik@cern.ch, 
  */
  class IMdtSegmentFitterTool : virtual public IAlgTool {
  
  public:
    /**Virtual destructor*/
    virtual ~IMdtSegmentFitterTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMdtSegmentFitterTool; };
  
    /** Interface method: return the IMdtSegmentFitter pointer */

    virtual IMdtSegmentFitter* getImp() = 0;
    
  };

}

#endif //
