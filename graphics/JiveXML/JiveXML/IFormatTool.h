/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__IFORMATTOOL_H
#define JIVEXML__IFORMATTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "JiveXML/DataType.h"

namespace JiveXML {

  /** Must declare this, with name of interface*/
  static const InterfaceID IID_IFormatTool("IFormatTool", 1, 0);

  /**
   * Abstract interface to event data formatting classes.
   * Currently, one JiveXML format is implemented, but other 
   * text formats can easily be imagined.
   *
   * @author Sebastian Boeser
   */
  class IFormatTool : virtual public IAlgTool {

    public:
      /** The interface identifier */
      static const InterfaceID& interfaceID();
      
      /** Pure virtual interface functions */
      virtual StatusCode initialize() = 0;
      virtual StatusCode finalize() = 0;

      /** Set additional tags */
      virtual StatusCode SetTag( const TagType tag ) = 0;

      /** Start a new event */
      virtual StatusCode StartEvent( const unsigned long EventNumber, const unsigned int RunNumber, 
                                const std::string DateTime,
   			        const unsigned int lumiBlock, std::string eventProperty, std::string geometryVersion ) = 0 ;

      /** Finialize this event */
      virtual StatusCode EndEvent() = 0 ;

      /** Append a formatted version of one event component */
      virtual StatusCode AddToEvent( const std::string component, const std::string key, const DataMap* aMap) = 0 ;

      /** Return the formated string */
      virtual const std::ostringstream* getFormattedEvent() const = 0;
      
  };

  inline const InterfaceID& IFormatTool::interfaceID()
  { 
          return IID_IFormatTool; 
  }

} //namespace

#endif

