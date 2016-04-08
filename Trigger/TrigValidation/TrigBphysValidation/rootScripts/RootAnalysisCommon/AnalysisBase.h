/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// AnalysisBase class
///---------------------------------------------------
/// Base class for B2JpsiV0 analyzes
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#ifndef AnalysisBase_H
#define AnalysisBase_H

class AnalysisBase {
  public:
    //constructors
    AnalysisBase() :
      m_OL(INFO), m_fi(0), m_currentFile(NULL), m_corruptedFiles(0)  { };

    AnalysisBase(OutputLevel OL) :
      m_OL(OL), m_fi(0), m_currentFile(NULL), m_corruptedFiles(0)  { };

    void          loadDirectory(std::string dirPath);         // loads root files from the directory
    void          loadFiles(std::vector<std::string> files);  // loads files
    TFile*        getFile(std::string fileName);              // opens root file "fileName"
    void          resetIterator();                            // reset file iterator
    bool          iterateFiles();                             // iterate through m_files
    TTree*        loadTree(std::string name);                 // loads tree from the current file

    //inline functions
    TFile*        currentFile()                       { return m_currentFile; }
    std::string   currentFileName()                   { return m_currentFile!=NULL ? m_currentFile->GetName() : "" ; }
    std::string   dirPath()                           { return m_dirPath; }
    OutputLevel   ol()                                { return m_OL; }
    void          addCorruptedFile(std::string file)  { m_corruptedFiles.push_back(file); }

  private:
    OutputLevel                         m_OL;               // output level used by Log tool

    std::string                         m_dirPath;          // directory containing root files
    std::vector<std::string>            m_files;            // list of file names

    int                                 m_fi;               // file names iterator
    TFile*                              m_currentFile;      // pointer to currently open file
    std::vector<std::string>            m_corruptedFiles;   // list of corrupted files
};

///===================================================
/// Begin of definitions
///===================================================
void AnalysisBase::loadDirectory(std::string dirPath) {
  Log log("AnalysisBase::loadDirectory", ol());

  //check dirPath
  m_dirPath = dirPath;
  if(m_dirPath.length() > 0 && m_dirPath.at(m_dirPath.length()-1) != '/') m_dirPath += "/";

  //read file names
  log.info("Reading directory " + m_dirPath );
  m_files = listFiles(m_dirPath);

  log.info("There is " + iToStr(m_files.size()) + " files in the directory");

  //get ready for iteration through files
  resetIterator();
}
///===================================================
void AnalysisBase::loadFiles(std::vector<std::string> files) {
  Log log("AnalysisBase::loadDirectory", ol());

  m_files = files;

  log.info("There is " + iToStr(m_files.size()) + " files in the list");

  //get ready for iteration through files
  resetIterator();

}
///===================================================
TFile* AnalysisBase::getFile(std::string fileName) {
  Log log("AnalysisBase::getFile", ol());

  log.debug("Opening " + fileName);
  m_currentFile = TFile::Open(fileName.c_str());
  if(m_currentFile!=NULL && m_currentFile->IsZombie()) {
    log.warning("File " + fileName + " is zombie");
    m_currentFile = NULL;
  }

  return m_currentFile;

}
///===================================================
void AnalysisBase::resetIterator() {
  Log log("AnalysisBase::resetIterator", ol());

  m_currentFile = NULL;
  m_fi = 0;
}
///===================================================
bool AnalysisBase::iterateFiles() {
  Log log("AnalysisBase::iterateFiles", ol());

  //close previously opened file
  if(m_currentFile!=NULL)
    m_currentFile->Close();

  // try to load the file. If file is corrupted, try the next one ...
  log.debug("Starting loop");
  for(; m_fi < (int)m_files.size() && getFile( m_dirPath + m_files.at(m_fi) ) == NULL; ++m_fi) {
    log.warning("File " + iToStr(m_fi) + "  " + m_files.at(m_fi) + " can't be opened");
    addCorruptedFile( m_files.at(m_fi) );
  }
  log.debug("end of loop");

  //check if iterator reached end of the list
  if(m_fi >= (int)m_files.size()) {
    m_currentFile = NULL;
    log.info("End of file list reached");
    return false;
  }

  // info output
  log.info(" File " + iToStr(m_fi) + "  " + m_files.at(m_fi) + " opened ");

  // prepare iterator for the next loop
  ++m_fi;

  return true;

}
///===================================================
TTree* AnalysisBase::loadTree(std::string name) {
  Log log("AnalysisBase::loadTree", ol());

  if(currentFile() == NULL) return NULL;

  TTree* tree = (TTree*)currentFile()->Get(name.c_str());
  if(tree == NULL) {
    log.error("Tree " + name + " cannot be loaded from the file");
  }

  return tree;
}

#endif

///===================================================
///      . .
///====o==V==o========================================
