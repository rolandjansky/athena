/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EnvelopesXMLHandler_H
#define EnvelopesXMLHandler_H

#include "FadsXMLParser/DescriptionFactory.h"
//  #include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"
#include <string>

class EnvelopeGeometryManager;

class EnvelopesXMLHandler: public FADS::DescriptionFactory {
public:
	EnvelopesXMLHandler(std::string s, EnvelopeGeometryManager *gm);
	void BuildDescription();
private:
	EnvelopeGeometryManager *geometryManager;
};

#endif
