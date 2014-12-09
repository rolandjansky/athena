/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* package FTKPatternWithCoverage
 *
 * Store a hit pattern with coverage and
 * provide classes to be used for comparisons with STL containers
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

#include <iostream>
#include <TTree.h>
#include <TLeaf.h>
#include <TRegexp.h>
#include "TrigFTKSim/FTKPatternWithCoverage.h"
#include "TrigFTKSim/FTKRootFile.h"

using namespace std;

//================== class FTKPatternRootTreeReader =================

FTKPatternRootTreeReader::FTKPatternRootTreeReader
(int sector,const char *name)  : FTKLogging(name) {
   fPatternNumber=0;
   fNumReads=0;
   fTTree=0;
   fSector=sector;
   fTreeName=ConstructTreeName(sector);
   fPattern=0;
}

FTKPatternRootTreeReader::~FTKPatternRootTreeReader() {
   if(fTTree) delete fTTree;
   if(fPattern) delete fPattern;
}

bool FTKPatternRootTreeReader::LoadTree(TDirectory *dir) {
   bool r=false;
   TTree * tree=0;
   dir->GetObject(fTreeName,tree);
   if(tree) {
      if(tree!=fTTree) {
         if(fTTree) delete fTTree;
         fTTree=0;
         TLeaf *leaf=tree->GetLeaf("pattern","pattern");
         int nLayer=0;
         if(leaf) {
            nLayer=leaf->GetLenStatic();
            if(nLayer>0) {
               if(!fPattern) {
                  fPattern=new FTKPatternWithCoverage(nLayer);
               } else if((int)fPattern->GetHitPattern().GetSize()!=nLayer) {
                  Error("LoadTree tree")
                     <<"\""<<fTreeName
                     <<"\" branch/leaf \"pattern\" has bad length="
                     <<nLayer<<" (expected="
                     <<fPattern->GetHitPattern().GetSize()<<")\n";
               }
            } else {
               Error("LoadTree")
                  <<"tree \""<<fTreeName
                  <<"\" branch/leaf \"pattern\" has non-static length="
                  <<nLayer<<"\n";
            }
         } else {
            Error("LoadTree")
               <<"tree \""<<fTreeName
               <<"\" does not have a branch/leaf named \"pattern\"\n";
         }
         if(nLayer>0) {
            bool error=false;
            error |= (tree->SetBranchAddress
                      ("pattern",fPattern->GetHitPattern().GetAddress())
                      !=0 /* TTree::kMatch // enum is protected in 5.30 */);
            error |= (tree->SetBranchAddress
                      ("coverage",fPattern->GetCoverageAddress())
                      !=0 /* TTree::kMatch // enum is protected in 5.30 */);
            if(error) {
               Error("LoadTree")
                  <<"tree \""<<fTreeName<<"\" does not have branches "
                  <<"pattern[NLAYER]/I and coverage/I\n";
            } else {
               fTTree=tree;
            }
         }
         r=(fTTree !=0);
      } else {
         r=true;
      }
   } else {
      if(fTTree) delete fTTree;
      fTTree=0;
      Warning("LoadTree")<<"object \""<<fTreeName<<"\" is not a TTree\n";
   }
   return r;
}

bool FTKPatternRootTreeReader::CreateTree(TDirectory *dir,int nLayer) {
   // create new tree for writing
   if(fPattern) {
      if((int)fPattern->GetHitPattern().GetSize()!=nLayer) {
         Error("CreateTree")
            <<"inconsistent number of layers "<<nLayer
            <<" (expected "<<fPattern->GetHitPattern().GetSize()<<")\n";
      }
   } else {
      fPattern=new FTKPatternWithCoverage(nLayer);
   }
   dir->cd();
   fTTree=new TTree(fTreeName,fTreeName);
   if(fTTree) {
      // create branches
      fTTree->Branch("pattern",fPattern->GetHitPattern().GetAddress(),
                     TString::Format("pattern[%d]/I",nLayer),32768);
      fTTree->Branch("coverage",fPattern->GetCoverageAddress(),
                     "coverage/I",4096);
   }
   return fTTree!=0;
}

TString FTKPatternRootTreeReader::ConstructTreeName(int sector) {
   return TString::Format("sector%d_patterns",sector);
}

int FTKPatternRootTreeReader::ExtractSectorNumber(char const* treeName) {
   int r=-1;
   TString name(treeName);
   if(name.BeginsWith("sector") && name.EndsWith("_patterns")) {
      r=TString(name(TRegexp("[0-9]+"))).Atoi();
      if(!ConstructTreeName(r).EqualTo(name)) {
         r=-1;
      }
   }
   return r;
}

//================== class FTKPatternRootChainReader =================

void FTKPatternRootChainReader::Add(char const *name,Long64_t nPattern) {
   fFileNames.push_back(name);
   fNumEntries.push_back(nPattern);
   Long_t sum=fNumEntriesSummed.size()
      ? fNumEntriesSummed[fNumEntriesSummed.size()-1] : 0;
   sum += nPattern;
   fNumEntriesSummed.push_back(sum);
}

Long64_t FTKPatternRootChainReader::GetNPatterns(void) const {
   int np=fNumEntriesSummed.size()-1;
   return (np>=0) ? fNumEntriesSummed[np] : 0;
}

bool FTKPatternRootChainReader::ReadNextPattern(void) {
   int iFile;
   for(iFile=0;iFile<(int)fNumEntriesSummed.size()-1;iFile++) {
      if(fPatternNumber<fNumEntriesSummed[iFile]) break;
   }
   int fileMin=iFile ? fNumEntriesSummed[iFile-1] : 0;
   if((fCurrentFile!=iFile)|| !fTTree) {
      if(fTTree) delete fTTree;
      fTTree=0;
      fCurrentFile=iFile;
      fChain.Open(fFileNames[fCurrentFile]);
      LoadTree(fChain.GetDirectory());
   }
   // read pattern
   bool r=false;
   if(fTTree) {
      r=fTTree->GetEntry(fPatternNumber-fileMin);
      fPatternNumber++;
   }
   if(r) fNumReads++;
   return r;
}

//================== class FTKPatternRootTree ========================

FTKPatternRootTree::FTKPatternRootTree(TDirectory &dir,int sector,int nLayer)
   : FTKPatternRootTreeReader(sector,"FTKPatternRootTree") {
   fMustSave=false;
   fNumWrites=0;
   if(nLayer==0) {
      // load tree from file
      if(!LoadTree(&dir)) {
         Error("(constructor)")<<"tree \""<<GetTreeName()
                               <<"\" can not be loaded\n";
      }
   } else {
      if(!CreateTree(&dir,nLayer)) {
         Error("(constructor)")<<"tree \""<<GetTreeName()
                               <<"\" can not be created\n";
      }
      fMustSave=true;
   }
}

FTKPatternRootTree::~FTKPatternRootTree() {
   // save TTree to disk
    // if(fMustSave && fTTree) {
    //    fTTree->AutoSave("SaveSelf"); //needed?
    // }
    if(fMustSave && fTTree) {
       //cout<<"nentries: "<<fTTree->GetEntries()<<", name="<<fTTree->GetName()<<endl;
       fTTree->Write(0,TObject::kOverwrite);
    }
}


Long64_t FTKPatternRootTree::GetNPatterns(void) const {
   return fTTree->GetEntries();
}

bool FTKPatternRootTree::WritePattern(FTKPatternWithCoverage const &pattern) {
   // write one pattern
   fMustSave=true;
   SetPattern(pattern);
   bool r=fTTree->Fill();
   if(r) fNumWrites++;
   return r;
}

bool FTKPatternRootTree::WritePattern(FTKHitPattern const &hits,int coverage) {
   // write one pattern
   fMustSave=true;
   SetPattern(coverage,hits);
   bool r=fTTree->Fill();
   if(r) fNumWrites++;
   return r;
}

bool FTKPatternRootTree::ReadNextPattern() {
   // read one pattern
   bool r=false;
   if(HasMorePatterns()) {
      r=fTTree->GetEntry(fPatternNumber);
      fPatternNumber++;
   }
   if(r) fNumReads++;
   return r;
}
