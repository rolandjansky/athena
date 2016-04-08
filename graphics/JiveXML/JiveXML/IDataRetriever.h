/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_IDATARETRIEVER_H
#define JIVEXML_IDATARETRIEVER_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ClassID.h"

#include "JiveXML/IFormatTool.h"

namespace JiveXML{

  static const InterfaceID IID_IDataRetriever("JiveXML::IDataRetriever", 3, 0);

  /**
   * Interface for all DataRetriever classes, that extract event data from
   * StoreGate and pass them on to a IFormatTool The interface will only hold
   * the methods that have to be implemented by all DataRetrievers.
   * 
   * @author Sebastian Boeser   sboeser@hep.ucl.ac.uk
   */
  class IDataRetriever : virtual public IAlgTool{

  public:

    /** Return the interface identifier **/
    static const InterfaceID& interfaceID();
    
    /** Retrieve data and pass it on to the formatting tool **/
    virtual StatusCode retrieve( ToolHandle<IFormatTool> &FormatTool ) = 0;
    
    /** Return the class name of the data class that is handled by this retriever **/
    virtual std::string dataTypeName() const = 0;

  };
  
  inline const InterfaceID& IDataRetriever::interfaceID() { return IID_IDataRetriever; }
}
#endif

