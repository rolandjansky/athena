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
  m_nEventsProcessed = 0;

}

TFile* PandaRootTools::OpenNextFile() {

  using namespace std;

  m_currentFile++;

  if (m_fileList.size() == 0) {
    //User was expecting input after all.
    gSystem->Exit(EC_BADINPUT);
  }

  if (m_currentFile == m_fileList.size() + 1) {
    return 0;
  }

  if (m_currentFile > m_fileList.size() + 1) {
    //Error: User persists reading beyond end of list 
    // - maybe we are stuck in a loop?
    //Abort program or this could go on forever...
    cout << "OpenNextFile() called after end of list\n" << endl;
    cout << "Aborting\n" << endl;
    Abort();
  }

  try {
    TFile* f = TFile::Open(m_fileList.at(m_currentFile - 1)); 
    if (f == 0)  
      gSystem->Exit(EC_BADINPUT);
    if (f->IsZombie())
      gSystem->Exit(EC_BADINPUT);
    return f;
  }
  catch (...) {    
    gSystem->Exit(EC_BADINPUT);
  }
  return 0;
}

void PandaRootTools::NotifyNewEvent() {
  m_nEventsProcessed++;
}

void PandaRootTools::NotifyJobFinished() {
  if (m_fileList.size() > m_currentFile + 1) {
    //Error: Did not read all files!
    gSystem->Exit(EC_NOTFINISHED);
  }    
  createJobSummary();
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

int PandaRootTools::GetEventsRead() {
  return m_nEventsProcessed;
}

void PandaRootTools::createJobSummary() {
  std::ofstream summaryfile("../AthSummary.txt");
  if (summaryfile.is_open()) {
    unsigned int nFiles = m_currentFile;
    if (nFiles > m_fileList.size()) 
      nFiles = m_fileList.size();
    summaryfile << "Files read: " << nFiles << std::endl;
    for (unsigned int i = 0; i < nFiles; i++) {
      summaryfile << "  " << m_fileList.at(i) << std::endl;
    }      
    summaryfile << "Events Read:    " << m_nEventsProcessed << std::endl;
    summaryfile.close();
  }
  else {
    //cout << "Failed to write summary file.\n";
  } 
}

void PandaRootTools::Fail() {
  using namespace std;
  try {
    cerr << "Error reported at event " << m_nEventsProcessed
	 << " in file " << m_currentFile << " (" 
	 << m_fileList.at(m_currentFile) << ")\n"
	 << "ending job with status \"failed\"\n";
  } catch (...) {}
  gSystem->Exit(EC_FAIL);
}

void PandaRootTools::Abort() {
  gSystem->Exit(EC_ABORT);
}

