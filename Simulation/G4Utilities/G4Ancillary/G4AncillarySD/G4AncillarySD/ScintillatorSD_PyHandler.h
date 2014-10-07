/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTILLATORSD_PYHANDLER_H
#define SCINTILLATORSD_PYHANDLER_H

#include "G4AncillarySD/ScintillatorSD.h"
#include "FadsXMLParser/DescriptionFactory.h"
#include <string>

class ScintillatorSD;

class  ScintillatorSD_PyHandler{

 public:
  ScintillatorSD_PyHandler();

  void BuildDescription( std::string detectorPart, std::string volumeName,
			 std::string hitCollectionName, bool writeHitsWithZeroEnergy );

 private:
    ScintillatorSD * theScintillatorSD;

};
#endif
