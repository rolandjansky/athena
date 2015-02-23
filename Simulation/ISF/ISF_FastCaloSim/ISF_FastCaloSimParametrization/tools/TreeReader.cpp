/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TreeReader.h"
#include "TTreeFormula.h"
#include "TTreeFormulaManager.h"
#include "TString.h"
#include <iostream>
#include <TChain.h>
#include <vector>
//////////////////////////////////////////////////
//
//		Class TreeReader
//		TreeReader.cpp
//
//////////////////////////////////////////////////

//______________________________________________________________________________
/*
Class for Tree reading through TFormula.
______________________________________________________________________________*/

ClassImp(TreeReader); // Integrate this class into ROOT

//============================================================
TreeReader::TreeReader()
  //============================================================
{
  // Default constructor.
  
  fTree = 0;
  fCurrentEntry = -1;
  fEntries = -1;
}

//============================================================
TreeReader::TreeReader(TTree* n)
  //============================================================
{
  // Constructor.
  fEntries = -1;
  SetTree(n);
}

//============================================================
TreeReader::~TreeReader()
  //============================================================
{
  // Destructor.
  fFormulae.clear();
}

//============================================================
void TreeReader::SetTree(TTree* n)
  //============================================================
{
  // Set tree.
  fTree = n ;
  fCurrentEntry = -1;
  fFormulae.clear();
  fFormulae["__DUMMY__"] = new TTreeFormula("__DUMMY__","0",fTree);  
  fIsChain = n->IsA() == TClass::GetClass("TChain");
  fCurrentTree = 0;
  if(!n) return;
  fEntries = (int) fTree->GetEntries();
}

//=============================================================
double TreeReader::GetVariable(const char* c, int entry)
  //============================================================
{
  // Get vaviable.
  // Return variable for a given entry (<0 -> current entry).
if(entry>=0 && entry!=fCurrentEntry) this->GetEntry(entry);
std::string s = c;
TTreeFormula *f = fFormulae[s]; 
if(!f)
  {
  f = new TTreeFormula(c,c,fTree);
  f->SetQuickLoad(kTRUE);
//   fManager->Add(f);
//   fManager->Sync();  
  if(f->GetNdim()!=1)  //invalid fomula
    {
    delete f;
    f = fFormulae["__DUMMY__"];
    std::cout << "in [TreeReader] : " << s << " is not valid -> return 0" << std::endl;
    }
//  else {f->Notify();}
  fFormulae[s] = f;     
  }
if(f == fFormulae["__DUMMY__"]) return 0; 
int valid = f->GetNdata()  ;
if(!valid) return 0; 
// std::cout << "Evaluating formula : " << s << std::flush;
// std::cout << "  " << f->EvalInstance(0) << std::endl;
return f->EvalInstance(0);
}


//============================================================
int TreeReader::GetEntry(int entry)
  //============================================================
{
  // Read a given entry in the buffer (-1 -> next entry).
  // Return kFALSE if not found.
//   entry += 1;
  if(fEntries==0) return 0;
  if(entry==-1) entry = fCurrentEntry+1;
  if(entry<fEntries)
    {
    int entryNumber = fTree->GetEntryNumber(entry);
    if (entryNumber < 0) return 0;
    Long64_t localEntry = fTree->LoadTree(entryNumber);   
    if (localEntry < 0) return 0;    
    fCurrentEntry = entry;
      if(fIsChain) // check file change in chain
        {
        int I = static_cast<TChain*>(fTree)->GetTreeNumber();   
        if(I!=fCurrentTree) 
          {
          fCurrentTree = I;
          //fManager->Clear();
          std::map<std::string, TTreeFormula*>::iterator itr = fFormulae.begin();
          std::map<std::string, TTreeFormula*>::iterator itrE= fFormulae.end();  
          TTreeFormula* dummy = fFormulae["__DUMMY__"];     
          for(;itr!=itrE;itr++) 
            { 
            if(itr->second!=dummy) itr->second->Notify(); //itr->second->UpdateFormulaLeaves();
   	        }
          }	
        }     
      return 1;
    }
  return 0;
}
