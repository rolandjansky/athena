/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IACTSMATERIALJSONWRITERTOOL_H
#define ACTSGEOMETRY_IACTSMATERIALJSONWRITERTOOL_H

#include "GaudiKernel/IInterface.h"

class DetectorMaterialMaps;

class IActsMaterialJsonWriterTool : virtual public IInterface {
public:
    
  DeclareInterfaceID(IActsMaterialJsonWriterTool, 1, 0);
    
  void
  virtual
  write(const DetectorMaterialMaps& detMaterial) const = 0;

};


#endif 
