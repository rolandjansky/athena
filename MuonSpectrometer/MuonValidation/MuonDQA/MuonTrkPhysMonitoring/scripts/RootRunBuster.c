/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 *  RootRunBuster.c
 *  RootMacroAnalysis
 *
 *  Created by Austin Basye on 6/27/12.
 *
 */


void RootRunBuster(TString Filename){
  
  std::cout <<  "\n";
  std::cout <<  "***************************************************\n";
  std::cout <<  "* RootRunBuster.c                                 *\n";
  std::cout <<  "* MuDQA Utilities Packages                        *\n";
  std::cout <<  "* Source by: Austin Basye                         *\n";
  std::cout <<  "* USAGE: root -l -q -b \"RootRunBuster.c(\"*.root\")\"*\n";
  std::cout <<  "***************************************************\n";
  
  
  
  std::cout <<  " Bustin' up " << Filename << ": \n\n";
  
  TFile * InputFile = TFile::Open(Filename,"READ");
  
  TFile * CurrentOutput;
  
  TIter nextkey(InputFile->GetListOfKeys());
  TKey* key1;
  
  while ((key1 = (TKey*)nextkey())) { 
    
    TString classname = key1->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    
    if (cl->InheritsFrom(TDirectory::Class()) && 
        TString(key1->GetName()).Contains("run_")) 
    {
      
      TDirectory *dir1= dynamic_cast<TDirectory*> (key1->ReadObj());
      
      CurrentOutput = TFile::Open(TString(dir1->GetName())+".root","RECREATE");
      
      CloneDirectory(CurrentOutput,dir1);
      
      CurrentOutput->SaveSelf(kTRUE);
      
      
      std::cout <<  (TString(dir1->GetName())+".root") << " created." << "\n";
      
    }else {
      continue;
    }
    
  } 

  
  
  std::cout << "\n" <<  Filename << " successfully busted!" << "\n";
  return;
}

void CloneDirectory(TDirectory * Dir0,TDirectory * OldDir){
  

  TString NewDirectory(OldDir->GetName());
  TDirectory * Dir1 = Dir0->mkdir(NewDirectory);
  
  OldDir->cd();
  
  TIter nextkey(OldDir->GetListOfKeys());
  TKey* key1;
  
  while ((key1 = (TKey*)nextkey())) { 
    
    
    TString classname = key1->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    
    if (cl->InheritsFrom(TDirectory::Class())) 
    {
      
      TDirectory *NextOldDir= dynamic_cast<TDirectory*> (key1->ReadObj());
      
      CloneDirectory(Dir1,NextOldDir);
      
    }else if(cl->InheritsFrom(TTree::Class()))
    { 
      TTree *T = (TTree*)OldDir->Get(key1->GetName());
   
      Dir1->cd();
      TTree *newT = T->CloneTree(-1,"fast");
      newT->Write();
      OldDir->cd();  
      
    }
    else
    {
    
      TObject *obj = key1->ReadObj();
      Dir1->cd();
      obj->Write();
      delete obj;
      OldDir->cd();      
    
    }
  } 
  
  Dir1->SaveSelf(kTRUE);
  
  return;
}

