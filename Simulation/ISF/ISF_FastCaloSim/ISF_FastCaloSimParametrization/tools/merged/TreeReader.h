/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TREEREADER_H
#define TREEREADER_H

//////////////////////////////////////////////////
//
//		Class TreeReader
//		TreeReader.h
//
// Class for Tree reading through TFomula
//////////////////////////////////////////////////

#include "TTree.h"
#include "TTreeFormula.h"
#include "TTreeFormulaManager.h"
#include "TString.h"
#include <map>
#include <vector>
/*
class std::vector<double>;
class std::vector<int>;
class std::vector<std::vector<double> >;
class std::vector<std::vector<int> >;*/

//////////////////////////////////////////////////
class TreeReader //: public TTreeFormulaManager
{
 private:

  TTree* fTree;
  int              fCurrentEntry; 			// current ntuple entry stored in buffer
  int              fEntries;      			// total number of entries
  bool             fIsChain;
  int              fCurrentTree;
  std::map<std::string, TTreeFormula*>     fFormulae;	// known formulae

 public:

  TreeReader();               // Default ctor
  TreeReader(TTree* n);       // ctor with ntuple
  virtual ~TreeReader();      // dtor

  void SetTree(TTree* n);       //
  double GetVariable(const char* c, int entry=-2); // return variable s for a given entry (<0 -> current entry)
  int    GetEntry(int entry=-1);     // Read a given entry in the buffer (-1 -> next entry);
  int      GetEntries()             { return fEntries ; }
  TTree*   GetTree()                { return fTree    ; }
  void Restart()                    {fCurrentEntry = -1;}
  ClassDef(TreeReader,0);  // Integrate this class into ROOT (must be the last member)
};

#endif
