/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <EventLoopGrid/PandaRootTools.h>

ClassImp(PandaRootTools)

PandaRootTools::PandaRootTools() {

  using namespace std;

  ifstream infile("input.txt");
  while (infile) {
    string sLine;
    if (!getline(infile, sLine)) break;
    istringstream ssLine(sLine);
    while (ssLine) {
      string sFile;
      if (!getline(ssLine, sFile, ',')) break;
      m_fileList.push_back(TString(sFile));
    }
  } 
  
  if (m_fileList.size() == 0) {
    //not fatal, maybe input was not expected
  }

  m_currentFile = 0;

}

TString PandaRootTools::getNextFile() {

  using namespace std;

  m_currentFile++;

  if (m_fileList.size() == 0) {
    //User was expecting input after all.
    gSystem->Exit(EC_BADINPUT);
  }

  if (m_currentFile == m_fileList.size() + 1) {
    return "";
  }

  if (m_currentFile > m_fileList.size() + 1) {
    //Error: User persists reading beyond end of list 
    // - maybe we are stuck in a loop?
    //Abort program or this could go on forever...
    cout << "OpenNextFile() called after end of list\n" << endl;
    cout << "Aborting\n" << endl;
    Abort();
  }

  return m_fileList.at(m_currentFile - 1);
}

void PandaRootTools::NotifyJobFinished(uint64_t eventsProcessed) {
  if (m_fileList.size() > m_currentFile + 1) {
    //Error: Did not read all files!
    gSystem->Exit(EC_NOTFINISHED);
  }    
  createJobSummary(eventsProcessed);
}

int PandaRootTools::GetNumberOfInputFiles() {
  return m_fileList.size();
}

int PandaRootTools::GetFilesRead() {
  unsigned int nFiles = m_currentFile;
  if (nFiles > m_fileList.size())
    nFiles = m_fileList.size();
  return nFiles;
}

void PandaRootTools::createJobSummary(uint64_t eventsProcessed) {
  std::ofstream summaryfile("../AthSummary.txt");
  if (summaryfile.is_open()) {
    unsigned int nFiles = m_currentFile;
    if (nFiles > m_fileList.size()) 
      nFiles = m_fileList.size();
    summaryfile << "Files read: " << nFiles << std::endl;
    for (unsigned int i = 0; i < nFiles; i++) {
      summaryfile << "  " << m_fileList.at(i) << std::endl;
    }      
    summaryfile << "Events Read:    " << eventsProcessed << std::endl;
    summaryfile.close();
  }
  else {
    //cout << "Failed to write summary file.\n";
  } 
}

void PandaRootTools::Fail(uint64_t eventsProcessed) {
  using namespace std;
  try {
    cerr << "Error reported at event " << eventsProcessed
	 << " in file " << m_currentFile << " (" 
	 << m_fileList.at(m_currentFile) << ")\n"
	 << "ending job with status \"failed\"\n";
  } catch (...) {}
  gSystem->Exit(EC_FAIL);
}

void PandaRootTools::Abort() {
  gSystem->Exit(EC_ABORT);
}

