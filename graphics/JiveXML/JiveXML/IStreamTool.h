/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__ISTREAMTOOL_H
#define JIVEXML__ISTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace JiveXML {

  /** Must declare this, with name of interface*/
  static const InterfaceID IID_IStreamTool("IStreamTool", 1, 0);

  /**
   * Abstract interface to JiveXML event streaming classes.
   * Events can be streamed e.g. in a file, to a server, etc.
   *
   * @author Sebastian Boeser
   */
  class IStreamTool : virtual public IAlgTool {

    public:
      /** The interface identifier */
      static const InterfaceID& interfaceID();
      
      /** Pure virtual interface functions */
      virtual StatusCode initialize() = 0;
      virtual StatusCode finalize() = 0;

      /** Stream one event */
      virtual StatusCode StreamEvent( const unsigned long EventNumber, const unsigned int RunNumber, const std::ostringstream* EventBuffer ) = 0;
      
  };

  inline const InterfaceID& IStreamTool::interfaceID()
  { 
          return IID_IStreamTool; 
  }

} //namespace

#endif

