/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HISTOPILE_H
#define HISTOPILE_H
//
// A hashlist of automatic histograms
//
// - push and pop labels to traverse "tree"
// - call fill to fill histogram belonging to current tree node
// 
// Author: Gerhard Brandt, 2009-09-27 
//

#include <iostream>
#include <list>
#include <string>

#include "TFile.h"
#include "TH1F.h"
#include "THashList.h"

using namespace std;

class HistoPile {

public:
  
  HistoPile(const char * filename, const char * reffilename=0) : 
      m_currHist(0)
    , m_filename(filename) 
  { 
    if (reffilename) {
      m_reffile = TFile::Open(reffilename);
      if ( !m_reffile ) {
        cout << "WARNING Could not open reference file!" << endl;
      } else {
        cout << "Using reference file " << reffilename << endl;
      }
    } else
      m_reffile = 0;  
	  
	// high number to avoid collisions  
	m_histos = new THashList(10000);  	  
  }

  void Push(const char *);          // Descend object tree
  void Pop();                       // Ascend object tree
  void Fill(Float_t);               // Plot value at current node
  const char *Path();               // Print name of current node
  void Save();                      // Save histograms to file
  
private:

  THashList*    m_histos;            // hashlist of histograms at nodes
  TH1F*         m_currHist;          // current histogram
  const char *  m_filename;          // name of output file
  TFile *       m_reffile;           // reference file to take histo definitions from
  
  list<string>  m_path;              // current path
};

#endif
