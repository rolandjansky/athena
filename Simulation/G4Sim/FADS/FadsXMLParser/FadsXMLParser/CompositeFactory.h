/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CompositeFactory_H
#define CompositeFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
#include <string>

#include "xercesc/dom/DOMNode.hpp"
XERCES_CPP_NAMESPACE_USE

namespace FADS {

  class CompositeFactory: public DescriptionFactory {
  public:
    CompositeFactory(std::string s);
    void Build(const DOMNode& n);
  };

} // end namespace

#endif
