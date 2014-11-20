/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for Phantom Barrel
//
// Author: Franck.Martin<Franck.Martin@cern.ch>
// December 15th, 2003.
//
//************************************************************

#ifndef PhantomBarrelFactory_H
#define PhantomBarrelFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
using namespace FADS;
#include <string>

//class DOMNode;
class DetectorDescriptionStore;

class PhantomBarrelFactory : public DescriptionFactory
{
public:
  PhantomBarrelFactory(std::string factoryName);
  void Build(const DOMNode& n);
};

#endif
