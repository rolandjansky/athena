/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_IGEOMETRYWRITER_H
#define JIVEXML_IGEOMETRYWRITER_H

#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/IAlgTool.h"

namespace JiveXML{

  static const InterfaceID IID_IGeometryWriter("JiveXML::IGeometryWriter", 1, 0);

  class IGeometryWriter : virtual public IAlgTool{
  public:
    static const InterfaceID& interfaceID();
    
    virtual StatusCode writeGeometry() = 0;

    virtual ~IGeometryWriter(){}
  };
  
  inline const InterfaceID& IGeometryWriter::interfaceID() { return IID_IGeometryWriter; }
}
#endif
