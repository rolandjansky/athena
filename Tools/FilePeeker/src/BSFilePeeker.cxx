//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>
#include <unistd.h>
#include "EventStorage/pickDataReader.h"
#include "FileMetaData.h"

class BSFilePeeker {

public: 
  BSFilePeeker(const std::string& filename);

  const FileMetaData& get() const {return m_fmd;}

private:

  bool extractValue(const std::string& source, const std::string& key, std::string& value);
  
  FileMetaData m_fmd;
  
};


bool BSFilePeeker::extractValue(const std::string& source, const std::string& key, std::string& value) {

  const size_t sep=source.find(":");
  if (sep==std::string::npos || source.compare(0,sep,key)!=0) {
    return false;
  }
  else {
    value=source.substr(sep);
    return true;
  }
}

BSFilePeeker::BSFilePeeker(const std::string& fName) {

  DataReader *pDR = pickDataReader(fName);

  if(!pDR) {
    std::cerr << "Problem opening or reading the bytestream file " 
	      << fName << std::endl;
    return;
  }

  if(!pDR->good()) {
    std::cerr << "No events in file "<< fName << std::endl;
  }

  m_fmd.m_runNumbers.insert(pDR->runNumber());
  m_fmd.m_lbNumbers.insert(pDR->lumiblockNumber());
  m_fmd.m_project=pDR->projectTag();
  m_fmd.m_stream=pDR->stream();
  m_fmd.m_nEvents=pDR->eventsInFile();
  m_fmd.m_beamEnergy=pDR->beamEnergy()*1000;
  m_fmd.m_guid=pDR->GUID();

  const unsigned bt=pDR->beamType();

  switch (bt) {
  case 0:
    m_fmd.m_beamType="cosmics";
    break;
  case 1: 
  case 2: //actally heavy-ion
    m_fmd.m_beamType="collisions";
    break;
  default:
    m_fmd.m_beamType="unknown";
  }

  const std::vector<std::string> fmds=pDR->freeMetaDataStrings();
  for (const std::string& fm : fmds) {
    extractValue(fm,"GeoAtlas",m_fmd.m_geoTag);
    extractValue(fm,"IOVDbGlobalTag",m_fmd.m_condTag);
  }

  m_fmd.m_valid=true;
}


int main(int argc, char** argv) {

  bool verbose=false;
  bool kvDump=false;
  char c;

  while ((c = getopt (argc, argv, "vk")) != -1) {
    switch (c) {
    case 'v':
      verbose=true;
      break;
    case 'k':
      kvDump=true;
      break;
    default:
      std::cerr << "Unkown command line option" << std::endl;
    return -1;
    }
  }

  if (optind+1!=argc) {
    std::cerr << "Expected exactly one file name as parameter" << std::endl;
    return -1;
  }

  const char* filename=argv[optind];
  if (verbose) std::cout << "Checking file " << filename << std::endl;

  BSFilePeeker bsfile(filename);

  if (kvDump) {
    bsfile.get().keyValueDump();
  }
  else {
    bsfile.get().dump();
  }

  if (bsfile.get().m_valid) 
    return 0;
  else
    return -1;
}
