/**
 **     @file    makeSmallRefFile.cxx
 **
 **     @brief   modified from copyFiles.C in ROOT tutuorial
 **
 **     @author J.Kirk 
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <string>
#include <iostream>


#include "TROOT.h"
#include "TKey.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"

  //Example of script showing how to copy all objects from specified directories
  //from a source file.
  //After execution of:
  // root > .L makeSmallRefFile.C+
  // root > makeSmallRefFile()
  //the file new-ref.root will contain the new smaller reference file
      
void CopyDir(TDirectory *source, bool mkdirFlag) {
   //copy all objects and subdirs of directory source as a subdir of the current directory   
  printf("CopyDir called with sourcem mkdirFlag: %s , %d\n",source->GetName(), mkdirFlag);
   source->ls();
   TDirectory *savdir = gDirectory;
   TDirectory *adir;
   if (mkdirFlag) {
     adir = savdir->mkdir(source->GetName());
   } else {
     adir = savdir;
   }
   adir->cd();
   //loop on all entries of this directory
   TKey *key;
   TIter nextkey(source->GetListOfKeys());
   while ((key = (TKey*)nextkey())) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom(TDirectory::Class())) {
	const char *dirname = key->GetName();
        printf("Directory: %s\n",dirname);
	if ( (std::string(dirname).find("EF") !=std::string::npos)) printf ("Found EF"); 
	if ( (std::string(dirname).find("L2") !=std::string::npos)) printf ("Found L2"); 
	if ( (std::string(dirname).find("HLT") !=std::string::npos)) printf ("Found HLT"); 
	if ( (std::string(dirname).find("Fast") !=std::string::npos)) printf ("Found Fast"); 
	if ( (std::string(dirname).find("vs") !=std::string::npos)) printf ("Found vs"); 

	if ( (std::string(dirname).find("EF") !=std::string::npos)   || (std::string(dirname).find("L2")!=std::string::npos) || (std::string(dirname).find("HLT")!=std::string::npos) || (std::string(dirname).find("Fast")!=std::string::npos)  || (std::string(dirname).find("vs")!=std::string::npos)   ) {
 	// if ( std::string(dirname).find("slices") !=std::string::npos   || std::string(dirname).find("roi")!=std::string::npos  ) {
         printf("Directory OK will be copied: %s\n",dirname);	
         source->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         adir->cd();
         CopyDir(subdir, true);
         adir->cd();
       	} else {
         printf("Directory will NOT be copied: %s\n",dirname);
	}
	
      
      } else if (cl->InheritsFrom(TTree::Class())) {
         TTree *T = (TTree*)source->Get(key->GetName());
         adir->cd();
         TTree *newT = T->CloneTree(-1,"fast");
         newT->Write();
      } else {
         source->cd();
         TObject *obj = key->ReadObj();
         adir->cd();
	 if ( std::string(key->GetClassName()).find("TH1")!=std::string::npos )   obj->Write();
         delete obj;
     }
      
  }
  adir->SaveSelf(kTRUE);
  savdir->cd();
}


void CopyFile(const char *fname) {
   //Copy all objects and subdirs of file fname as a subdir of the current directory
   TDirectory *target = gDirectory;
   TFile *f = TFile::Open(fname);
   if (!f || f->IsZombie()) {
      printf("Cannot copy file: %s\n",fname);
      target->cd();
      return;
   }
   target->cd();
   CopyDir(f,false);
   delete f;
   target->cd();
}

  
void makeSmallRefFile( const std::string& in, const std::string& out ) {
  //main function copying 4 files as subdirectories of a new file
  TFile *f = new TFile( out.c_str(), "recreate" );
  //   CopyFile("data-tau-merge.root");
  CopyFile( in.c_str() );
  f->ls();
  delete f;
}


int usage(int status=0) {
  std::cout << "Usage: makeSmallRefFile <intputfile> <outputfile>" << std::endl;
  return status;
}


int main( int argc, char** argv ) {

  if ( argc<3 ) return usage(-1);

  std::string infile  = argv[1];
  std::string outfile = argv[2];
  
  makeSmallRefFile( infile, outfile ); 

  return 0;
}
