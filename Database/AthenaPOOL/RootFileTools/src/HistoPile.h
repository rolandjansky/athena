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
      fCurrHist(0)
    , fFilename(filename) 
  { 
    if (reffilename) {
      fReffile = TFile::Open(reffilename);
      if ( !fReffile ) {
        cout << "WARNING Could not open reference file!" << endl;
      } else {
        cout << "Using reference file " << reffilename << endl;
      }
    } else
      fReffile = 0;  
	  
	// high number to avoid collisions  
	fHistos = new THashList(10000);  	  
  }

  void Push(const char *);          // Descend object tree
  void Pop();                       // Ascend object tree
  void Fill(Float_t);               // Plot value at current node
  const char *Path();               // Print name of current node
  void Save();                      // Save histograms to file
  
private:

  THashList*    fHistos;            // hashlist of histograms at nodes
  TH1F*         fCurrHist;          // current histogram
  const char *  fFilename;          // name of output file
  TFile *       fReffile;           // reference file to take histo definitions from
  
  list<string>  fPath;              // current path
};

#endif
