/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for  LAr Crate in tile Gap
//
// Author: Franck.Martin<Franck.Martin@cern.ch>
// November 18, 2004
//
//************************************************************

#ifndef LArCrateFactory_H
#define LArCrateFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
using namespace FADS;
#include <string>

class DetectorDescriptionStore;

class LArCrateFactory : public DescriptionFactory
{
public:
  LArCrateFactory(std::string factoryName);
  void Build(const DOMNode& n);
};

#endif
