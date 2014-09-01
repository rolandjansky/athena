/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IEventFilterTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IEVENTFILTERTOOL_H
#define ISF_INTERFACES_IEVENTFILTERTOOL_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace ISF {

  static const InterfaceID IID_IEventFilterTool("IEventFilterTool", 1, 0);
   
  /**
   @class IEventFilterTool
       
   @author John.Chapman -at- cern.ch
   */
     
  class IEventFilterTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IEventFilterTool(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IEventFilterTool; }
       
       /** Check that the current event passes this filter */
       virtual bool eventPassesFilter() const = 0;
  };

} // end of namespace

#endif // ISF_INTERFACES_IEVENTFILTERTOOL_H
