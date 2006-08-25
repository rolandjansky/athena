/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPATTERNSELECTORTOOL_H
#define IPATTERNSELECTORTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace MuonCalib {

  /** Interface ID for IMdtSegmentSelectorTool*/  
  static const InterfaceID IID_IPatternSelectorTool("MuonCalib::IPatternSelectorTool", 1, 0);

  /**@class IPatternSelectorTool
     Interface class for PatternSelectorTool's
     Implementations create an instance of a IMuonCalibPatternSelector implementation.
   
     @author Niels.Van.Eldik@cern.ch, 
  */

  class ICalibPatternSelector;

  class IPatternSelectorTool : virtual public IAlgTool {
  
  public:
    /**Virtual destructor*/
    virtual ~IPatternSelectorTool(){};
     
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IPatternSelectorTool; };

    /** Interface method: return the implementation pointer */
    virtual const ICalibPatternSelector* getImp() = 0;
  
 
  };

}

#endif //
