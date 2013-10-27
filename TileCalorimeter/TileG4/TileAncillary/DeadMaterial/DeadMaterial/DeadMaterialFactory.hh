/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for Dead Material
//
// Author: Franck.Martin<Franck.Martin@cern.ch>
// July 12, 2004
//
//************************************************************

#ifndef DeadMaterialFactory_H
#define DeadMaterialFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
using namespace FADS;
#include <string>

//class DOMNode;
class DetectorDescriptionStore;

class DeadMaterialFactory : public DescriptionFactory
{
public:
  DeadMaterialFactory(std::string factoryName);
  void Build(const DOMNode& n);
};

#endif
