/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/NtupleGoodiesFiller.h"
#include "MissingETGoodness/Goodies.h"
#include <iostream>
#include <TObjArray.h>
#include <TIterator.h>
#include <TBranch.h>
#include <TLeaf.h>


ClassImp(MET::Goodies)


MET::NtupleGoodiesFiller::NtupleGoodiesFiller() :
  TObject(), 
  firstTask(kTRUE), 
  _stripprefix("MET_Goodness_")
{
}


Bool_t 
MET::NtupleGoodiesFiller::execute()  
{
  MET::Goodies& goodies = MET::Goodies::instance();
  // before filling, clear goodies map
  goodies.clear();
  
  std::vector<TTree* >::iterator treeVecItr = _treeVec.begin();
  for(; treeVecItr!=_treeVec.end(); ++treeVecItr) {

    TTree *tree = *treeVecItr;
    if( !tree ) {
      std::cout << "MET::NtupleGoodiesFiller::copyEvent() : error retrieving tree. Return false."
                << std::endl;
      return kFALSE;
    }
    
    // loop over tree branches
    TObjArray* brlist = tree->GetListOfBranches() ;
    TObject* obj ;

    // store names of all variables of interest
    if (firstTask) {
      TIterator* itr = brlist->MakeIterator() ;
      while( (obj = itr->Next())) {

	// skip unfilled branches
	TBranch* branch(0);
	if ( !(branch = dynamic_cast<TBranch*>(obj)) ) continue;
	if ( !((void *)branch->GetAddress()) ) continue;       
	
	TObjArray *ll = branch->GetListOfLeaves() ;
	if (ll->GetEntries()!=1) {
	  std::cout << "MET::NtupleGoodiesFiller::addTree() : WARNING : Clone: skipping branch with multiple leaves" << std::endl ;
	  continue ;
	}
	TLeaf* tl = (TLeaf*) (*ll)[0] ;
	TString tlName = TString(tl->GetTypeName()) ;
	if (tlName!="Int_t" && tlName!="Double_t" && tlName!="Float_t") continue;

	Int_t countVal ;
	TLeaf* tlc = tl->GetLeafCounter(countVal) ;
	// skip arrays
	if (tlc) continue;

	varNameVec.push_back(obj->GetName());
      }
      delete itr;
    }

    //TIterator* iter = brlist->MakeIterator() ;
    //while(obj = iter->Next()) {

    std::vector<TString>::iterator varNameItr = varNameVec.begin();
    for ( ; varNameItr!=varNameVec.end(); ++varNameItr ) { 

      obj = brlist->FindObject((*varNameItr).Data());
      if (!obj) continue;

      // skip unfilled branches
      TBranch* branch(0);
      if ( !(branch = dynamic_cast<TBranch*>(obj)) ) continue;
      void* address(0);
      if ( !(address = (void *)branch->GetAddress()) ) continue;       

      TObjArray *ll = branch->GetListOfLeaves() ;
      if (ll->GetEntries()!=1) {
	std::cout << "MET::NtupleGoodiesFiller::addTree() : WARNING : Clone: skipping branch with multiple leaves" << std::endl ;
	continue ;
      }
      TLeaf* tl = (TLeaf*) (*ll)[0] ;
      TString tlName = TString(tl->GetTypeName()) ;
      if (tlName!="Int_t" && tlName!="Double_t" && tlName!="Float_t") continue;

      Int_t countVal ;
      TLeaf* tlc = tl->GetLeafCounter(countVal) ;
      // skip arrays
      if (tlc) continue;

      // possibility to strip part of name
      TString name = *varNameItr; //(branch->GetName());
      if (name.BeginsWith(_stripprefix)) { name.Replace( 0, _stripprefix.Length(), "", 0 ); }
      
      TString title(branch->GetTitle());

      // copy single int, double, float to event
      if (tlName=="Int_t" || title.EndsWith("/I")) {
	Int_t* adr = static_cast<Int_t*>(address);
	goodies.gInt()[name] = *adr;
      }
      if (tlName=="Double_t" || title.EndsWith("/D")) {
	Double_t* adr = static_cast<Double_t*>(address);
	goodies.gDouble()[name] = *adr;
      }
      if (tlName=="Float_t" || title.EndsWith("/F")) {
	Float_t* adr = static_cast<Float_t*>(address);
	goodies.gDouble()[name] = double(*adr);
      }

    } // end loop over branches
    //delete iter ;

  } // end loop over treeVec

  firstTask=false;
  return kTRUE ;
}


Bool_t 
MET::NtupleGoodiesFiller::addTree (TTree* tree)
{
  if (tree!=0) _treeVec.push_back(tree);
  else {
    std::cout << "MET::NtupleGoodiesFiller::addTree() : WARNING : Trying to add empty tree. Return false."
              << std::endl;
    return kFALSE;
  }

  return kTRUE;
}

