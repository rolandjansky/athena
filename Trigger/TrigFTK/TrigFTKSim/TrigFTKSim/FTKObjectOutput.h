/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKOBJECTOUTPUT_H
#define FTKOBJECTOUTPUT_H

#include "TrigFTKSim/FTKSetup.h"

#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>

using namespace std;

template<class FTKObjectStream>
class FTKObjectOutput {
protected:
   bool m_multifile;        // true if it's working using the multi-out mode
   
   std::string m_filename;  // filename of the single output file
   
   std::string m_outdir;    // base outdir used in multiout mode

   int m_nbanks;            // number of regions (aka banks)

protected:
  TFile *m_file; // pointer the the current output file
  TTree *m_tree; // current TTree used to store the results

  std::string m_datafmt; // generic format string for the branch name: e.g. "FTKBank%d." or "FTKTracks%d."
  FTKObjectStream **m_data; // aray of output stream pointers
  
public:
  FTKObjectOutput();
  virtual ~FTKObjectOutput();

  void setBranchFormat(const char *fmt) { m_datafmt = fmt; }
  const char* getBranchFormat() const { return m_datafmt.c_str(); }

  void setNBanks(int);
  int getNBanks() const { return m_nbanks; }

  void setFileName(const char *fname) 
  { m_filename = fname; }
  const char* getFileName() const { return m_filename.c_str(); }

  void setOutDir(const char *dname)
  { m_outdir = dname; }
  const char* getOutDir() const { return m_outdir.c_str(); }

  void setMultiOut(bool v) { m_multifile = v; }
  bool getMultiOut() const { return m_multifile; }

  void init(int bufsize = 0, bool *gooRegions=0x0);

  FTKObjectStream* getStream(int id) { return m_data[id]; }

  TTree* getTree() { return m_tree; } // expose the TTree, use carefully

  void beginFile(const char *fname=0);

  void eventBegin();

  void eventEnd();

  void endFile();

  unsigned long runNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->runNumber() : 0 ); }
  unsigned long eventNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->eventNumber() : 0 ); }
  void setRunNumber(const unsigned int& ibank,const unsigned long& val) { if( m_data && (m_data[ibank]) ) { (m_data[ibank])->setRunNumber(val); } }
  void setEventNumber(const unsigned int& ibank,const unsigned long& val) { if( m_data && (m_data[ibank]) ) { (m_data[ibank])->setEventNumber(val); } }

};

template<class FTKObjectStream>
FTKObjectOutput<FTKObjectStream>::FTKObjectOutput() :
   m_multifile(false), m_filename(""),
   m_outdir(""), m_nbanks(0),
   m_file(0), m_tree(0),
   m_datafmt("FTKBank%d."),
   m_data(0)
{

}

template<class FTKObjectStream>
FTKObjectOutput<FTKObjectStream>::~FTKObjectOutput()
{
  endFile();

  if (m_nbanks) {
    for (int ib=0;ib<m_nbanks;++ib) {
      delete m_data[ib];
    }

    delete [] m_data;
  }
}


template<class FTKObjectStream>
void FTKObjectOutput<FTKObjectStream>::setNBanks(int nbanks)
{
  m_nbanks = nbanks; 
 
  m_data = new FTKObjectStream*[m_nbanks];
  for (int ib=0;ib<m_nbanks;++ib) { // bank loop
    // create the stream object and its branch
    m_data[ib] = new FTKObjectStream();
  }

}


template<class FTKObjectStream>
void FTKObjectOutput<FTKObjectStream>::init(int bufsize, bool *goodRegions)
{
  TBranch* tmpBranch;

  // check if the tree exists
  if (!m_tree) {
    // if the TTree doesn't exist create it and allocate the branches
    m_tree = new TTree("ftkdata","FTK Simulation results");

    for (int ib=0;ib<m_nbanks;++ib) { // bank loop
      // skip bad regions (with no real simulation done)
      if (!goodRegions[ib]) continue;

      const string bname1 = Form(m_datafmt.c_str(),ib);
      if(bufsize)
	// if buffer size is specified (eg, 16000)
	tmpBranch = m_tree->Branch(bname1.c_str(),&m_data[ib],bufsize);
      else
	// use default buffer size
	tmpBranch = m_tree->Branch(bname1.c_str(),&m_data[ib]);
      if( tmpBranch )
	tmpBranch->SetCompressionLevel(1);
    } // end bank loop
  }

  //m_tree->Print();
}


/** start a new file. Thie routine open it and then move the TTree to be
    in this file */
template<class FTKObjectStream>
void FTKObjectOutput<FTKObjectStream>::beginFile(const char *fname)
{
  // open the file
  if (!fname) 
    m_file = TFile::Open(m_filename.c_str(),"recreate");
  else {
    string realname("");
    if (m_multifile)
      realname = m_outdir+"/";
    realname += fname;
    m_file = TFile::Open(realname.c_str(),"recreate");
  }

  if (!m_file) {
    FTKSetup::PrintMessage(ftk::sevr,"*** error trying to open the output file");
  }

  // "move" the TTree to point to this TDirectory
  m_tree->SetDirectory(m_file);
}


/** close an existing file, the TTree is moved out after the
    content is saved */
template<class FTKObjectStream>
void FTKObjectOutput<FTKObjectStream>::endFile()
{
  // prevent to close a file already closed
  if (!m_file) return;

  // save the file
  m_file->Write();
  m_tree->Reset();

  // move the m_tree in a different existing directory, gROOT is ok
  m_tree->SetDirectory(gROOT);

  // close the file, end clean the memory
  m_file->Close();
  delete m_file;

  m_file = 0x0;
}


/** event begin */
template<class FTKObjectStream>
void FTKObjectOutput<FTKObjectStream>::eventBegin()
{
  for (int ib=0;ib<m_nbanks;++ib) {
    m_data[ib]->clear();
  }
}

template<class FTKObjectStream>
void FTKObjectOutput<FTKObjectStream>::eventEnd()
{
  // store the data int the TTree
  m_tree->Fill();
}

#endif // FTKOBJECTOUTPUT_H
