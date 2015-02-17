/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKOBJECTINPUT_H
#define FTKOBJECTINPUT_H

#include "TrigFTKSim/FTKSetup.h"

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <unistd.h>

#include <TFile.h>
#include <TTree.h>

using namespace std;

template<class FTKObjectStream, class FTKObject>
class FTKObjectInput {
protected:
  std::list<std::string> m_files_path; // path of the input files

  std::list<std::string>::iterator m_current_path; // current file

  TFile *m_current_file; // current ROOT file

  TTree *m_current_tree; // current ROOT TTree with Roads bank
  
  long int m_entry; // entry number

  long int m_max_entry; // maximum number of entries in the current TTree

  int m_nbanks; // number of the banks
 
  FTKObjectStream **m_data; // objects with input data

  int *cur_iobject; // array with ID of the used road / track
  
  FTKObject **cur_object; // array of current roads/tracks

  bool m_init; // true if the input is initialized

  int nextFile(); // open the next file

public:

  FTKObjectInput();
  virtual ~FTKObjectInput();

  int addFile(const char*); // add a single file
  
  int addFileList(const char *); // add a file with a list of files

  const char* getCurrentFileName() { return (*m_current_path).c_str();}

  void setNBanks(int v) { m_nbanks = v; }
  int getNBanks() const { return m_nbanks; }

  FTKObjectStream* getStream(int id) { return m_data[id]; }

  void init(bool *gooRegions=0x0); // initialize the structur

  virtual int nextEvent() = 0; // read the next event
  unsigned long runNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->runNumber() : 0 ); }
  unsigned long eventNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->eventNumber() : 0 ); }

};


template<class FTKObjectStream, class FTKObject>
FTKObjectInput<FTKObjectStream,FTKObject>::FTKObjectInput() :
  m_current_file(0), m_current_tree(0), m_entry(0),
  m_max_entry(0), m_nbanks(0), m_data(0), cur_iobject(0), cur_object(0), m_init(0)
{
  // nothing to do
}

template<class FTKObjectStream, class FTKObject>
FTKObjectInput<FTKObjectStream,FTKObject>::~FTKObjectInput()
{
  if (m_nbanks>0) {
    // remove objects used for the data input
    for (int ib=0;ib!=m_nbanks;++ib) { // bank loop
      delete m_data[ib];
    } // end bank loop

    // delete arrays
    delete [] m_data;
  }
}

template<class FTKObjectStream, class FTKObject>
int FTKObjectInput<FTKObjectStream,FTKObject>::addFile(const char *path)
{
  int res = access(path,R_OK);
  if (!res) { // add the file
    m_files_path.push_back(path);
    cout << "Added input file: " << path << endl;
    return 0;
  }
  else if (res==EACCES) {
    cerr << "*** warning: accessing the file: " << path << endl;    
  }
  else if (res==ENOENT) {
    cerr << "*** warning: doesn't exist: " << path << endl;
  }
  else {
        cerr << "*** error reading: " << path << endl;
  }

  return -1;
}

template<class FTKObjectStream, class FTKObject>
int FTKObjectInput<FTKObjectStream,FTKObject>::addFileList(const char *pathsfile)
{

  ifstream filelist(pathsfile);
  if (!filelist) {
    cerr << "*** error reading list of paths in: " << pathsfile << endl;
    return 0;
  }

  string line;
  int nfiles(0);
  while(getline(filelist,line)) {
    if (line[0]=='#') {
      // comment, skip tis line
      continue;
    }
    else if (line.size()==0) { 
      // empty line, skip
      continue;
    }
    else {
      int res = addFile(line.c_str());
      if (!res) nfiles += 1;
    }
  }

  // return the number of added files
  return nfiles;
}

/** initialization of the internal structures to loop
    on the input data */
template<class FTKObjectStream, class FTKObject>
void FTKObjectInput<FTKObjectStream,FTKObject>::init(bool *goodRegions)
{
  // reset the input-file iterator
  m_init = false;

  // allocate input structures
  m_data = new FTKObjectStream*[m_nbanks];
  // allocate internal counters
  cur_iobject = new int[m_nbanks];
  cur_object = new FTKObject*[m_nbanks];

  for (int ib=0;ib<m_nbanks;++ib) {
    if (!goodRegions[ib]) {
      // skip not used regions
      m_data[ib] = 0x0;
      continue;
    }

    m_data[ib] = new FTKObjectStream();
    cur_iobject[ib] = -1;
    cur_object[ib] = 0;
  }
  
  if (nextFile()==-1) {
    // no good files in the list
    FTKSetup::PrintMessage(ftk::sevr,"FTKObjectInput found no good files during init step");
  }

}


/** go to the next valid file, return 0 if succeeded */
template<class FTKObjectStream, class FTKObject>
int FTKObjectInput<FTKObjectStream,FTKObject>::nextFile()
{
  if (!m_init) {
    // start for the begin of the path list
    m_current_path = m_files_path.begin();
    m_init = true;
  }
  else {
     // free the memory
     m_current_file->Close();
     m_current_file->Delete();
     // move to the next file. this can advance the iterator to
     // m_files_path.end(), so check for that situation to avoid
     // trying to open a file with a gobbledigook filename.
    ++m_current_path; 
  }

  do { // loop over the paths
    if(m_current_path==m_files_path.end()) { break; }
    // open the next file
    m_current_file = TFile::Open((*m_current_path).c_str());
    if (!m_current_file) {
      FTKSetup::PrintMessageFmt(ftk::warn,"Invalid file: %s\n",(*m_current_path).c_str());
    }
    else {
      // file opened, reallign the TTree
      m_current_tree = (TTree*) m_current_file->Get("ftkdata");
      if (!m_current_tree) {
	FTKSetup::PrintMessageFmt(ftk::warn,"ftkdata TTree doesn't find in: %s\n",(*m_current_path).c_str());	
      }
      else {
	m_entry = 0;
	m_max_entry = m_current_tree->GetEntries();
	// link branches
	for (int ib=0;ib<m_nbanks;++ib) {
	  if (!m_data[ib]) {
	    // thi smeans that the data for the bank are not requested
	    continue;
	  }

	  m_current_tree->SetBranchAddress(Form("FTKBank%d.",ib),&m_data[ib]);
	}
	return 0;
      }
    }    
  } while (++m_current_path!=m_files_path.end());// end loop over the paths

  FTKSetup::PrintMessage(ftk::warn,"End of files list reached\n");

  return -1;
}

#endif // FTKOBJECTINPUT_H
