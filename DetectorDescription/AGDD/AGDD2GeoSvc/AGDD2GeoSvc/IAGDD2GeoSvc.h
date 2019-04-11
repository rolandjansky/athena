/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAGGDtoGeoSvc_H
#define IAGGDtoGeoSvc_H

// Include Files
#include "GaudiKernel/IService.h"
#include <string>
#include <vector>

// Forward declarations
class XMLHandler;

typedef std::vector<std::string>::const_iterator StrVecIterator;

class IAGDDtoGeoSvc : virtual public IService {
public:

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IAGDDtoGeoSvc, 1 , 0);

  // accessors
  virtual void addHandler(XMLHandler*) = 0;

};


#endif



