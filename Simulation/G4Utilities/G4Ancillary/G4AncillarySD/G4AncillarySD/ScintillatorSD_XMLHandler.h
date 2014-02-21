/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTILLATORSD_XMLHANDLER_H
#define SCINTILLATORSD_XMLHANDLER_H

#include "FadsXMLParser/DescriptionFactory.h"
#include <string>

class ScintillatorSD;

class  ScintillatorSD_XMLHandler : public FADS::DescriptionFactory {
 public:
  ScintillatorSD_XMLHandler( std::string s, ScintillatorSD * );
  void BuildDescription();
 private:
  ScintillatorSD * theScintillatorSD;
};

#endif
