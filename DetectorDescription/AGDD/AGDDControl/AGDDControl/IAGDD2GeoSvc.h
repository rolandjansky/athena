/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDCONTROL_IAGGD2GEOSVC_H
#define AGDDCONTROL_IAGGD2GEOSVC_H

// Include Files
#include "GaudiKernel/IService.h"
#include "CxxUtils/LockedPointer.h"
#include <string>
#include <vector>

// Forward declarations
class XMLHandler;
class AGDDController;

typedef std::vector<std::string>::const_iterator StrVecIterator;

class IAGDDtoGeoSvc : virtual public IService {
public:

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IAGDDtoGeoSvc, 1 , 0);

  using LockedController = CxxUtils::LockedPointer<AGDDController>;

  // accessors
  virtual void addHandler(XMLHandler*) = 0;
  virtual LockedController getController() = 0;
};


#endif



