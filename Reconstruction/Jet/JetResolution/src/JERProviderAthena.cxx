/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetResolution/JERProviderAthena.h"

using namespace JetResolution;

// Constructor
JERProviderAthena::JERProviderAthena(std::string CollectionName, std::string MethodName,
				     std::string FileName) :
  JERProvider(CollectionName, MethodName, FileName)
{

  // Use pathresolver to find the inputfile
  std::string pathToFile = PathResolver::find_file (FileName, "DATAPATH");

  cout << "JERProvider: Input file is at " << pathToFile << endl;
  JERProvider::setFileName(pathToFile);

}

// Destructor
JERProviderAthena::~JERProviderAthena()
{
//we don't own anything anymore now!
}
