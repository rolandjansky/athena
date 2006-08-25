/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_IMDTSEGMENTSELECTORTOOL_H
#define MUONCALIB_IMDTSEGMENTSELECTORTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"


namespace MuonCalib {

  /** Interface ID for IMdtSegmentSelectorTool*/  
  static const InterfaceID IID_IMdtSegmentSelectorTool("MuonCalib::IMdtSegmentSelectorTool", 1, 0);

  /**@class IMdtSegmentSelectorTool
     Interface class for MdtSegmentSelectorTool's
     Implementations create an instance of a IMdtSegmentSelector implementation.
   
     @author Niels.Van.Eldik@cern.ch, 
  */

  class IMdtSegmentSelector;

  class IMdtSegmentSelectorTool : virtual public IAlgTool {
  
  public:
    /**Virtual destructor*/
    virtual ~IMdtSegmentSelectorTool(){};
     
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMdtSegmentSelectorTool; };

    /** Interface method: return the implementation pointer */
    virtual const IMdtSegmentSelector* getImp() = 0;
  
 
  };

}

#endif //
