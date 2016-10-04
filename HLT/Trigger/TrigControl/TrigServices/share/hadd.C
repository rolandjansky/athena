/*

  NOTE: This macro is kept for back compatibility only.
  Use instead the executable $ROOTSYS/bin/hadd
  
  This macro will add histograms from a list of root files and write them
  to a target root file. The target file is newly created and must not be
  identical to one of the source files.

  Author: Sven A. Schmidt, sven.schmidt@cern.ch
  Date:   13.2.2001

  This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
  which had a problem with directories more than one level deep.
  (see macro hadd_old.C for this previous implementation).
  
  The macro from Sven has been enhanced by 
     Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
   to automatically add Trees (via a chain of trees).
  
  To use this macro, modify the file names in function hadd.
  
  NB: This macro is provided as a tutorial.
      Use $ROOTSYS/bin/hadd to merge many histogram files

  ATLAS HLT COMMENT by T. Bold:
  This is modified macro used to deal with the sort of strange
  ROOT files generated while athenaMT is run. There possibly
  number of the output files. With this one can work whit standard hadd.
  But in fact there is number of the TTrees in the file 
  having the same name, just subsequent sequence(version) number. 
  This can only be handled by this script. 
 */


#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

TList *FileList;
TFile *Target;

void MergeRootfile( TDirectory *target, TList *sourcelist );


void hadd() {
   // in an interactive ROOT session, edit the file names
   // Target and FileList, then
   // root > .L hadd.C
   // root > hadd()
   
  Target = TFile::Open( "result.root", "RECREATE" );
  
  FileList = new TList();
  FileList->Add( TFile::Open("hsimple1.root") );
  FileList->Add( TFile::Open("hsimple2.root") );
  
  MergeRootfile( Target, FileList );

}   

TTree * mergeCircularTrees(const char* treename, TDirectory *dir, TTree *nTree = 0) {
    
    TString basename(treename);
    TIter nextIter(gDirectory->GetListOfKeys()); 
    gDirectory->GetListOfKeys()->Print();                          
    TKey *obj;
    TTree *tptr;
    TList myTreesList;

    while ( (obj = (TKey *)nextIter()) != 0 ) {                                

	if ( TString(obj->GetClassName()) == TString("TTree") ) {
	    // it is the tree let's read it in
	    tptr = (TTree *)obj->ReadObj();
	    if ( TString(tptr->GetName()) == basename  ) { 
		if ( nTree == 0 ) {
		    nTree = (TTree*)tptr->Clone();
		    nTree->SetCircular(kMaxLong64); // maximum circularity
		}
		else
		    myTreesList.AddLast(tptr);
	    }    
	}
    }

    dir->cd();
    nTree->SetDirectory(dir);
    nTree->Merge(&myTreesList);
    return nTree;
}

void MergeRootfile( TDirectory *target, TList *sourcelist ) {

  //  cout << "Target path: " << target->GetPath() << endl;
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );

  TFile *first_source = (TFile*)sourcelist->First();
  first_source->cd( path );
  TDirectory *current_sourcedir = gDirectory;
  //gain time, do not add the objects in the list in memory
  Bool_t status = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  // loop over all keys in this directory
  TTree *nTree = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {
      //      cout << "found key " << key->GetName() << endl;
      //keep only the highest cycle number for each key
      if (oldkey)
	  if ( strcmp(oldkey->GetName(),key->GetName()) == 0 ) {
	      //      cout << "skipping key " << key->GetName() << endl;
	      continue;
	  }
      // always set oldkey ptr
      oldkey = key;
    
    // read object from first source file
    first_source->cd( path );
    TObject *obj = key->ReadObj();

    if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
      // descendant of TH1 -> merge it

      //      cout << "Merging histogram " << obj->GetName() << endl;
      TH1 *h1 = (TH1*)obj;

      // loop over all source files and add the content of the
      // correspondant histogram to the one pointed to by "h1"
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      while ( nextsource ) {
        
        // make sure we are at the correct directory level by cd'ing to path
        nextsource->cd( path );
        TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
        if (key2) {
           TH1 *h2 = (TH1*)key2->ReadObj();
           h1->Add( h2 );
           delete h2;
        }

        nextsource = (TFile*)sourcelist->After( nextsource );
      }
    }
    else if ( obj->IsA()->InheritsFrom( "TTree" ) ) {
      
      // loop over all source files create a chain of Trees "globChain"
      const char* obj_name= obj->GetName();
      cout << "merging all trees in first file" << endl;
      nTree = mergeCircularTrees(obj_name, target);
      cout << "merged trees in first file" << endl;

      TFile *nextsource = (TFile*)sourcelist->After( first_source );

     while ( nextsource ) {
	 nextsource->cd( path );
	 cout << "merging all trees in file "<< nextsource->GetName() << endl;
	 mergeCircularTrees(obj_name, target, nTree);
	 cout << "merged all trees "<< nextsource->GetName() << endl;
	 nextsource = (TFile*)sourcelist->After( nextsource );
     }

    } else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
      // it's a subdirectory

      cout << "Found subdirectory " << obj->GetName() << endl;

      // create a new subdir of same name and title in the target file
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

      // newdir is now the starting point of another round of merging
      // newdir still knows its depth within the target file via
      // GetPath(), so we can still figure out where we are in the recursion
      MergeRootfile( newdir, sourcelist );

    } else {

      // object is of no type that we know or can handle
      cout << "Unknown object type, name: " 
           << obj->GetName() << " title: " << obj->GetTitle() << endl;
    }

    // now write the merged histogram (which is "in" obj) to the target file
    // note that this will just store obj in the current directory level,
    // which is not persistent until the complete directory itself is stored
    // by "target->Write()" below
    if ( obj ) {
      target->cd();

      //!!if the object is a tree, it is stored in globChain...
      if(obj->IsA()->InheritsFrom( "TTree" )) {
	  nTree->Write();
	  nTree->Print();
      }
      else
	obj->Write( key->GetName() );
    }

  } // while ( ( TKey *key = (TKey*)nextkey() ) )

  // save modifications to target file
  target->SaveSelf(kTRUE);
  TH1::AddDirectory(status);
}
