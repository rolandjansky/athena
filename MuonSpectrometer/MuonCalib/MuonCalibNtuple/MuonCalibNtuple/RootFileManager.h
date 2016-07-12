/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_ROOTFILEMANAGER_H
#define MUONCALIB_ROOTFILEMANAGER_H

#include <string>
#include <map>

class TDirectory;
class TFile;
class TTree;

namespace MuonCalib {
/**
@class RootFileManager
A singleton managing the root file.
*/  
  class RootFileManager  {
  public:
    //    static RootFileManager* getInstance();
    static RootFileManager* getInstance() {
      if( m_instance == 0 ) m_instance = new RootFileManager();
      return m_instance;
    } //!< returns pointer to singleton 
   
    TDirectory* getDirectory( std::string dirname);         //!< get directory for a given name, creates new directory if directory doesn't exists yet 
    bool openFile( std::string filename);                   //!< Open new root file, close previous one and reset the DirectoryMap. All pointers to directories become invalid 
    bool closeFile();                                       //!< close current root file 
    void setPrintLevel(int level) { m_printLevel = level; } //!< set print level 
    void WriteMetaData(const std::map<std::string, std::string> &metadata, const std::string &filename);

  private:
    typedef std::map< std::string, TDirectory* >           DirectoryMap; //!< map to link a name to the corresponding directory 
    typedef std::map< std::string, TDirectory* >::iterator DirectoryIt;  //!< iterator of the map

    RootFileManager();                            //!< default constructor _private_, singleton 
    RootFileManager(const RootFileManager &mgr);  //!< _private_ copy constructor, prevents copying 

    static RootFileManager *m_instance;           //!< instance to RootFileManager

    std::string   m_fileOption;                   //!< option how to create the TFile
    std::map<std::string, TFile *> m_outfiles;
    TFile        *m_outFile;                      //!< TFile current root file
    std::map<std::string, TTree*> m_meta_data_tree;
    std::string *m_key, *m_value;
    DirectoryMap  m_dirMap;                       //!< Directory map to keep track on the set of TDirectories created
    int m_printLevel;                             //!< print level

  };

}  //namespace MuonCalib

#endif  
