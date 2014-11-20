/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for scintillator between Tile & LAr
//
// Author: Franck.Martin<Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#ifndef CombinedScintillatorFactory_H
#define CombinedScintillatorFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
using namespace FADS;
#include <string>

// class DOMNode;

class DetectorDescriptionStore;

class CombinedScintillatorFactory : public DescriptionFactory
{
public:
  CombinedScintillatorFactory(std::string factoryName);
  void Build(const DOMNode& n);
};

#endif
