/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for Muon Wall
//
// Author: Franck.Martin<Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#ifndef MuonWallFactory_H
#define MuonWallFactory_H

#include "FadsXMLParser/DescriptionFactory.h"
using namespace FADS;
#include <string>

//class DOMNode;
class DetectorDescriptionStore;

class MuonWallFactory : public DescriptionFactory
{
public:
  MuonWallFactory(std::string factoryName);
  void Build(const DOMNode& n);
};

#endif
