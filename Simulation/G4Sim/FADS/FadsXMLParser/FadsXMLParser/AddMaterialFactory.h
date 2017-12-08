/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AddMaterialFactory_H
#define AddMaterialFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
#include <string>

#include "xercesc/dom/DOMNode.hpp"
XERCES_CPP_NAMESPACE_USE

namespace FADS {

  class AddMaterialFactory: public DescriptionFactory {
  public:
    AddMaterialFactory(std::string s);
    void Build(const DOMNode& n);
  };

} // end namespace

#endif
