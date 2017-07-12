/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopDataPreparation/SampleXsectionSvc.h"
#include <iostream>
#include <cstdlib>

SampleXsectionSvc *SampleXsectionSvc::s_instance = 0;

SampleXsectionSvc::SampleXsectionSvc(std::string inputFile):
  m_inputFile(inputFile),
  m_sampleXsection(0) {
}

//-------------------------------------------------------------------------------

SampleXsectionSvc::~SampleXsectionSvc(){
  if(m_sampleXsection) delete m_sampleXsection;
}

//-------------------------------------------------------------------------------

int SampleXsectionSvc::initialize() {
  m_sampleXsection = new SampleXsection();

  char *rootcoreDir = getenv("ROOTCOREBIN");

  if (!m_sampleXsection->readFromFile(m_inputFile.c_str())) {
    if(rootcoreDir) {// if file can't be read, maybe it's in the common directory
      std::string fullpath = std::string(rootcoreDir) + "/data/TopDataPreparation/";
      fullpath += m_inputFile;
      if (!m_sampleXsection->readFromFile(fullpath.c_str())) {
        std::cerr << "SampleXsection::unable to read input file " << fullpath << std::endl;
        return 1;
      }
      else return 0;//all OK
    }
    else {// if common directory can't be found, then surrender
      std::cerr << "SampleXsection::unable to read input file " << m_inputFile << std::endl;
      return 1;
    }
  }

  return 0;
}

//----------------------------------------------------------------

SampleXsectionSvc *SampleXsectionSvc::svc(std::string inputFile) {
  if(s_instance == 0) {
    s_instance = new SampleXsectionSvc(inputFile);
    
    if(s_instance->initialize() != 0) {
      std::cerr << "Error: initialize failed.  Could not create SampleXsectionSvc." << std::endl;
      delete s_instance;
      s_instance = 0;
    }
  }
  
  return s_instance;
}
