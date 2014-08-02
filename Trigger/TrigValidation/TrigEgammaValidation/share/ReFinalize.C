#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TList.h"
#include "TChain.h"

using namespace std;

const char *DirectoriesKeys[] = {"ZeeValidation", "SingleElectronValidation", "HggValidation", "ZeeOfflineValidation", "SingleElectronOfflineValidation", "HggOfflineValidation", "JetValidation", "JpsiValidation"};

const char *DirectoriesPre[] = {"PreEfficiencies", "PreEfficiencies", "PreEfficiencies", "PreOfflineEfficiencies", "PreOfflineEfficiencies", "PreOfflineEfficiencies", "PreRates", "PreEfficiencies"};
const char *DirectoriesEff[] = {"Efficiencies", "Efficiencies", "Efficiencies", "OfflineEfficiencies", "OfflineEfficiencies", "OfflineEfficiencies", "Rates", "Efficiencies"};
const char *DirectoriesEt [] = {"TruthEtHist", "TruthEtHist", "TruthEtHist", "OffEtHist", "OffEtHist", "OffEtHist", "", "TruthEtHist"};
const char *DirectoriesEta[] = {"TruthEtaHist", "TruthEtaHist", "TruthEtaHist", "OffEtaHist", "OffEtaHist", "OffEtaHist", "", "TruthEtaHist"};
const char *DirectoriesPhi[] = {"TruthPhiHist", "TruthPhiHist", "TruthPhiHist", "OffPhiHist", "OffPhiHist", "OffPhiHist", "", "TruthPhiHist"};

int getdir (string dir, vector<string> &files)
{
  void *dirp = gSystem->OpenDirectory(dir.c_str());
  if (dirp == NULL) {
    std::cout << "Error opening " << dir << std::endl;
    return -1;
  }
  const char *entry = gSystem->GetDirEntry(dirp);
  while (entry != 0) {
    string newEntry = dir;
    newEntry += "/";
    newEntry += entry;
    
    ifstream inf(newEntry.c_str(), ios::binary);
    char onebyte;
    inf.read(&onebyte, 1);
    if (!inf.eof()) {
      files.push_back(newEntry);
    }
    inf.close();

    entry = gSystem->GetDirEntry(dirp);
  }
  return 0;
}

void MergeRootFile(TDirectory *target, TList *sourcelist) {
  //  cout << "Target path: " << target->GetPath() << endl;
  TString path((char*) strstr(target->GetPath(), ":" ));
  path.Remove( 0, 2 );

  TFile *first_source = (TFile*) sourcelist->First();
  first_source->cd(path);
  TDirectory *current_sourcedir = gDirectory;

  // loop over all keys in this directory
  TChain *globChain = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key;
  while ((key = (TKey*) nextkey())) {

    // read object from first source file
    first_source->cd(path);
    TObject *obj = key->ReadObj();

    if (obj->IsA()->InheritsFrom("TH1")) {
      // descendant of TH1 -> merge it

      //      cout << "Merging histogram " << obj->GetName() << endl;
      TH1 *h1 = (TH1*) obj;

      // loop over all source files and add the content of the
      // correspondant histogram to the one pointed to by "h1"
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      while ( nextsource ) {
        
        // make sure we are at the correct directory level by cd'ing to path
        nextsource->cd(path);
        TH1 *h2 = (TH1*) gDirectory->Get(h1->GetName());
        if (h2) {
          h1->Add(h2);
          delete h2; // don't know if this is necessary, i.e. if 
                     // h2 is created by the call to gDirectory above.
        }

        nextsource = (TFile*) sourcelist->After( nextsource );
      }
    } else if ( obj->IsA()->InheritsFrom( "TTree" ) ) {
      /*
      // loop over all source files create a chain of Trees "globChain"
      const char* obj_name = obj->GetName();

      globChain = new TChain(obj_name);
      globChain->Add(first_source->GetName());
      TFile *nextsource = (TFile*) sourcelist->After( first_source );
      //      const char* file_name = nextsource->GetName();
      // cout << "file name  " << file_name << endl;
      while (nextsource) {
        globChain->Add(nextsource->GetName());
        nextsource = (TFile*) sourcelist->After( nextsource );
      }
      */
    } else if (obj->IsA()->InheritsFrom("TDirectory")) {
      // it's a subdirectory

      cout << "Found subdirectory " << obj->GetName() << endl;

      // create a new subdir of same name and title in the target file
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

      // newdir is now the starting point of another round of merging
      // newdir still knows its depth within the target file via
      // GetPath(), so we can still figure out where we are in the recursion
      MergeRootFile(newdir, sourcelist);

    } else {

      // object is of no type that we know or can handle
      cout << "Unknown object type, name: " 
           << obj->GetName() << " title: " << obj->GetTitle() << endl;
    }

    // now write the merged histogram (which is "in" obj) to the target file
    // note that this will just store obj in the current directory level,
    // which is not persistent until the complete directory itself is stored
    // by "target->Write()" below
    if (obj) {
      target->cd();

      //!!if the object is a tree, it is stored in globChain...
	//if(obj->IsA()->InheritsFrom( "TTree" ))
	//  globChain->Write( key->GetName() );
	//else
	obj->Write( key->GetName() );
    }

  } // while ( ( TKey *key = (TKey*)nextkey() ) )

  // save modifications to target file
  target->Write();
}

void ReFinalize(const TString &file, const TString &dirToMerge) {

  // List files that were merged
  // We need this to merge the global rates and histograms
  vector<string> filesToMerge;
  if (getdir(string((const char *) dirToMerge), filesToMerge) != 0) {
    return;
  }

  TFile *File = TFile::Open(file, "RECREATE");
  if (!File) {
    cout << "It wasn't possible to open the file " << file << endl;
    return;
  }

  TList *FileList = new TList();
  for (unsigned int i = 0; i < filesToMerge.size(); ++i) {
    if (filesToMerge[i].find(".root") != string::npos) {
      TFile *f = TFile::Open(filesToMerge[i].c_str());
      if (f) {
        FileList->Add(f);
      }
    }
  }
  
  MergeRootFile(File, FileList);

  
  // Re-divide/normalize efficiency/rates histograms

  TIter next(File->GetListOfKeys());
  TKey *key;

  while ((key = (TKey *) next())) {
    if (std::string(key->GetClassName()).find("TDirectory", 0) != std::string::npos) {
      
      cout << "On Directory " << key->GetName() << endl;
      TString BasePath = key->GetName();

      float tsL = 0,tTotal = 0,tL1 = 0,tL2 = 0,tL2IDScan = 0,tEFScanCalo = 0,tL2IDSi = 0,tEFSiCalo = 0,tEFScanTrack = 0,tEFSiTrack = 0;
      bool copiedCuts = false;
      for (unsigned int i = 0; i < filesToMerge.size(); ++i) {
        //cout << "On File " << filesToMerge[i] << endl;
        if (filesToMerge[i].find(".root") != string::npos) {
          TFile *oneFileToMerge = new TFile(filesToMerge[i].c_str());

          TString tabledir = TString(BasePath);
          tabledir += "/Dump/EffTable";
          float sL,Total,L1,L2,L2IDScan,EFScanCalo,L2IDSi,EFSiCalo,EFScanTrack,EFSiTrack;
          TNtuple *ntable = (TNtuple *) oneFileToMerge->Get(tabledir);

          if (ntable == 0) {
            cout << "Error extracting info for the table " << tabledir << " from the root file!" << endl;
          } else {
            ntable->SetBranchAddress("sL", &sL);
            ntable->SetBranchAddress("Total", &Total);
            ntable->SetBranchAddress("L1", &L1);
            ntable->SetBranchAddress("L2", &L2);
            ntable->SetBranchAddress("L2IDScan", &L2IDScan);
            ntable->SetBranchAddress("EFScanCalo" , &EFScanCalo);
            ntable->SetBranchAddress("L2IDSi", &L2IDSi);
            ntable->SetBranchAddress("EFSiCalo", &EFSiCalo);
            ntable->SetBranchAddress("EFScanTrack", &EFScanTrack);
            ntable->SetBranchAddress("EFSiTrack", &EFSiTrack);
            ntable->GetEntry(0);
            tsL = sL;
            tTotal += Total; tL1 += L1; tL2 += L2; tL2IDScan += L2IDScan; tL2IDSi += L2IDSi; tEFScanCalo += EFScanCalo; tEFSiCalo += EFSiCalo;
            tEFScanTrack += EFScanTrack; tEFSiTrack += EFSiTrack;
          }

          if (!copiedCuts) {
            cout << "Copying cuts!" << endl;
            TString cutsPath = TString(BasePath);
            cutsPath += "/Dump/CutTable";
            TNtuple *cuts = (TNtuple *) oneFileToMerge->Get(cutsPath);
            if (!cuts) {
              cout << "Couldn't copy cuts!" << endl;
            } else {
              TString cutsDir = BasePath;
              cutsDir += "/Dump";
              TNtuple *myCuts = (TNtuple *) cuts->Clone();
              File->cd(cutsDir);
              myCuts->Write(NULL, TObject::kOverwrite);
              copiedCuts = true;
            }
          }
          delete oneFileToMerge;
        }
      }

      BasePath += "/";
      
      int keyNb = -1;
      for (unsigned int i = 0; i < sizeof(DirectoriesKeys)/sizeof(const char *); ++i) {
        if (string(DirectoriesKeys[i]) == string((const char *) key->GetName())) {
	  keyNb = i;
	  break;
	}
      }
      
      if (keyNb != -1) {
        
        if (File->cd(BasePath + DirectoriesEff[keyNb]));
          gDirectory->Delete("*;*");
      
        File->cd(BasePath);
	gDirectory->mkdir(DirectoriesEff[keyNb]);
	
        File->cd(BasePath + DirectoriesPre[keyNb]);
        TIter next1(gDirectory->GetListOfKeys());
        TKey *key1;

        TH1F *TruthEtHist;
        TH1F *TruthEtaHist;
        TH1F *TruthPhiHist;
	if ( (string(DirectoriesEt[keyNb]) != "") && (string(DirectoriesEta[keyNb]) != "") && (string(DirectoriesPhi[keyNb]) != "") ) {
          TruthEtHist = (TH1F *) gDirectory->Get(DirectoriesEt[keyNb]);
          TruthEtaHist = (TH1F *) gDirectory->Get(DirectoriesEta[keyNb]);
          TruthPhiHist = (TH1F *) gDirectory->Get(DirectoriesPhi[keyNb]);
          TruthEtaHist->Sumw2();
          TruthEtHist->Sumw2();
          TruthPhiHist->Sumw2();
        }

        while ((key1 = (TKey *) next1())) {
          TString key1name = key1->GetName();
          if ((!key1name.Contains("Hist")) && (std::string(key1->GetClassName()).find("TH1", 0) != std::string::npos) ) {
	    TH1F *myhist = new TH1F(*((TH1F *) key1->ReadObj()));
            myhist->Sumw2();
	    std::string key1namestd = (const char *) key1name;
	    if (string(DirectoriesKeys[keyNb]) == "JetValidation") {
	      myhist->Scale(tsL/tTotal);
	    } else {
	      if (key1namestd.find_last_of("Et") == (key1namestd.size() - 1)) {
	        myhist->Divide(TruthEtHist);
	      } else if (key1namestd.find_last_of("Eta") == (key1namestd.size() - 1)) {
	        myhist->Divide(TruthEtaHist);
	      } else if (key1namestd.find_last_of("Phi") == (key1namestd.size() - 1)) {
	        myhist->Divide(TruthPhiHist);
	      }
	    }
	    File->cd(BasePath + DirectoriesEff[keyNb]);
	    myhist->Write();
          }
        }
      
        // Generate Tables
        cout << "Generating efficiency table" << endl;
        TString tabledir = TString(BasePath);
        tabledir += "Dump";
        File->cd(tabledir);
        gDirectory->Delete("EffTable;*");
        TNtuple *ntable = new TNtuple("EffTable", "", "sL:Total:L1:L2:L2IDScan:L2IDSi:EFScanCalo:EFSiCalo:EFScanTrack:EFSiTrack");
        ntable->Fill(tsL, tTotal, tL1, tL2, tL2IDScan, tL2IDSi, tEFScanCalo, tEFSiCalo, tEFScanTrack, tEFSiTrack);
        ntable->Write(NULL, TObject::kOverwrite);

       
        if (File->cd(BasePath + "DumpNotScaled")) {
          cout << "  On dump" << endl;
          File->cd(BasePath + "DumpNotScaled");
          TIter nextdump(gDirectory->GetListOfKeys());
          TKey *keydump;
          while ((keydump = (TKey *) nextdump())) {
            TString key1name = keydump->GetName();
            if (std::string(keydump->GetClassName()).find("TH1", 0) != std::string::npos) {
              cout << "  On dump histogram "<< key1name << endl;
	      File->cd(BasePath + "Dump");
	      TH1F *myhist = new TH1F(*((TH1F *) keydump->ReadObj()));
              myhist->Sumw2();
	      myhist->Scale(1/myhist->Integral());
	      File->cd(BasePath + "Dump");
	      myhist->Write(NULL, TObject::kOverwrite);
            }
          }
        }
      }
    }
  }

  delete File;

  cout << "End of macro." << endl;
  
}

