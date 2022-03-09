//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>
#include <unistd.h>
// Suppress a gcc8 warning from boost.
// (Binaries apparently include boost with -I rather than -isystem.)
// Fixed in boost 1.68 (see https://github.com/boostorg/mpl/issues/31)
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wparentheses"
#endif
#include "EventStorage/pickDataReader.h"
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
#include "FileMetaData.h"
#include "CxxUtils/checker_macros.h"

class BSFilePeeker {

public: 
  BSFilePeeker(const std::string& filename);

  const FileMetaData& get() const {return m_fmd;}

private:

  bool extractValue(const std::string& source, const std::string& key, std::string& value);
  
  FileMetaData m_fmd;
  
};


bool BSFilePeeker::extractValue(const std::string& source, const std::string& key, std::string& value) {

  const size_t sep=source.find(':');
  if (sep==std::string::npos || source.compare(0,sep,key)!=0) {
    return false;
  }
  else {
    value=source.substr(sep+1);
    return true;
  }
}

BSFilePeeker::BSFilePeeker(const std::string& fName) {

  m_fmd.m_fileName=fName;
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

  //Accoring to info from Rainer and Guiseppe the beam type is
  //0: No beam
  //1: protons
  //2: ions
  switch (bt) {
  case 0:
    m_fmd.m_beamType="cosmics";
    break;
  case 1: 
  case 2: //actally heavy-ion
    m_fmd.m_beamType="collisions";
    break;
  default:
    std::cerr << "WARNING: Unexpected beam type integer in BS file. Got " << bt << std::endl;
    m_fmd.m_beamType="unknown";
  }


  m_fmd.m_isMC=false; //Generaly, BS-files are real data
  const std::vector<std::string> fmds=pDR->freeMetaDataStrings();
  std::string eventTypeMD;
  for (const std::string& fm : fmds) {
    extractValue(fm,"GeoAtlas",m_fmd.m_geoTag);
    extractValue(fm,"IOVDbGlobalTag",m_fmd.m_condTag);
    extractValue(fm,"Event type",eventTypeMD);
  }

  if (eventTypeMD.find("is sim")!=std::string::npos) {
    m_fmd.m_isMC=true; //This is a simulated bytestream file
  }


  m_fmd.m_valid=true;
  delete pDR;
}


int main ATLAS_NOT_THREAD_SAFE (int argc, char** argv) {

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


 const int nfiles=argc-optind;
 if (nfiles<=0) {
   std::cerr << "Expected at least one file name as parameter" << std::endl;
   return -1;
 }

  
 std::vector<FileMetaData> output;

 for (int iFile=optind;iFile<argc;++iFile) {
   const std::string filename(argv[iFile]);
   if (verbose) std::cout << "Checking file " << filename << std::endl;
    
   BSFilePeeker bsfp(filename);

   output.push_back(bsfp.get());
 }//end loop over input file names


 if (kvDump) {
   for (const auto& o : output) o.keyValueDump();
 }
 else {
   for (const auto& o : output) o.dump();
 }
 
 return 0;
}
