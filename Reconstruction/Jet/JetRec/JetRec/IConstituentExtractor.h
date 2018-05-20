// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ConstituentExtractor.h

#ifndef IConstituentExtractor_H
#define IConstituentExtractor_H

// Johannes Elmsheuser
// November 2016

#include "fastjet/PseudoJet.hh"
#include "xAODJet/Jet.h"
#include <string>
#include <vector>
#include <ostream>


class IConstituentExtractor {

public:
  virtual ~IConstituentExtractor();
  virtual IConstituentExtractor* clone() const = 0;
  // Alow copying of concrete Extractors which reside behind this
  // interface. The ghost flag is set. This flag determines
  // whether EDM objects ar added to jets as consituents (ghost = false) or
  // associated objcts (ghost=true)
  virtual IConstituentExtractor* ghostClone() const = 0;

  virtual void addToJet(xAOD::Jet&,
                        const std::vector<int>& indices) const = 0;

  virtual std::string toString(int level) const = 0 ;
  virtual bool checkIntegrity() const = 0;
};

std::ostream& operator << (std::ostream& os, const IConstituentExtractor* e);

#endif
