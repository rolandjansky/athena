/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2XML_H
#define Geo2XML_H

#include <fstream>

// helper class to translate GeoModel into XML
class Geo2XML {
public:
	void openFile(std::string);
	void closeFile();
private:
	std::ofstream xmlFile;
};

#endif
