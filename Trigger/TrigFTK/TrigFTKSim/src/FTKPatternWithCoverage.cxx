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
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
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
   fTTreePtr=0;
   fLastReadDir=0;
   fSector=sector;
   fTreeName=ConstructTreeName(sector);
   fPattern=0;
   fDoReadPattern=true;
}

FTKPatternRootTreeReader::~FTKPatternRootTreeReader() {
   if(fTTreePtr) delete fTTreePtr;
   if(fPattern) delete fPattern;
}

bool FTKPatternRootTreeReader::LoadTree(TDirectory *dir) {
   bool r=false;
   TTree * tree=0;
   if(!dir) dir=fLastReadDir;
   fLastReadDir=dir;
   dir->GetObject(fTreeName,tree);
   if(tree) {
      if(tree!=fTTreePtr) {
         if(fTTreePtr) delete fTTreePtr;
         fTTreePtr=0;
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
               fTTreePtr=tree;
               fTTreePtr->SetBranchStatus("pattern",fDoReadPattern);
               fLastNumPatterns=fTTreePtr->GetEntries();
            }
         }
         r=(fTTreePtr !=0);
      } else {
         r=true;
      }
   } else {
      if(fTTreePtr) delete fTTreePtr;
      fTTreePtr=0;
      Warning("LoadTree")<<"object \""<<fTreeName<<"\" is not a TTree\n";
   }
   return r;
}

void FTKPatternRootTreeReader::ReadCoverageOnly(bool noPatternRead) {
   fDoReadPattern= !noPatternRead;
   if(fTTreePtr) {
      fTTreePtr->SetBranchStatus("pattern",fDoReadPattern);
   }
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
   if(fTTreePtr) {
      Error("CreateTree")<<"TTree already existing\n";
      delete fTTreePtr;
   }
   fTTreePtr=new TTree(fTreeName,fTreeName);
   fDoReadPattern=true;
   if(fTTreePtr) {
      // create branches
      fTTreePtr->Branch("pattern",fPattern->GetHitPattern().GetAddress(),
                     TString::Format("pattern[%d]/I",nLayer),32768);
      fTTreePtr->Branch("coverage",fPattern->GetCoverageAddress(),
                     "coverage/I",4096);
   }
   return fTTreePtr!=0;
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

void FTKPatternRootTreeReader::Suspend(void) {
   if(fTTreePtr) delete fTTreePtr;
   fTTreePtr=0;
}

void FTKPatternRootTreeReader::Rewind(void) {
   Suspend();
   SeekBeg(0);
}

Long64_t FTKPatternRootTreeReader::SeekBeg(Long64_t position) {
   if(position>=0) {
      if(position>0) {
         Long64_t n=GetNPatterns();
         if(position>n) position=n;
      }
      fPatternNumber=position;
   }
   return fPatternNumber;
}

bool FTKPatternRootTreeReader::HasMorePatterns(void) const {
   // check whether there are more patterns
   return fPatternNumber<GetNPatterns();
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
   if((fCurrentFile!=iFile)|| !fTTreePtr) {
      if(fTTreePtr) delete fTTreePtr;
      fTTreePtr=0;
      fCurrentFile=iFile;
      fChain.Open(fFileNames[fCurrentFile]);
      LoadTree(fChain.GetDirectory());
   }
   // read pattern
   bool r=false;
   if(fTTreePtr) {
      r=fTTreePtr->GetEntry(fPatternNumber-fileMin);
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
    if(fMustSave && fTTreePtr) {
       //cout<<"nentries: "<<fTTree->GetEntries()<<", name="<<fTTree->GetName()<<endl;
       fTTreePtr->Write(0,TObject::kOverwrite);
    }
}


Long64_t FTKPatternRootTree::GetNPatterns(void) const {
   return fLastNumPatterns;
}

bool FTKPatternRootTree::WritePattern(FTKPatternWithCoverage const &pattern) {
   // write one pattern
   fMustSave=true;
   SetPattern(pattern);
   static int debug=0;
   struct rusage r0,r1;
   if(debug) {
      getrusage(RUSAGE_SELF,&r0);
      std::cout<<"WritePattern: sector="<<GetSectorNumber()
               <<" nLayer="<<GetPattern().GetHitPattern().GetSize()
          <<" numWrites="<<fNumWrites
          <<" maxrss="<<r0.ru_maxrss
          <<" ixrss="<<r0.ru_ixrss
          <<" idrss="<<r0.ru_idrss
          <<" isrss="<<r0.ru_isrss
          <<" fTTree->Fill()\n";
   }
   bool r=fTTreePtr->Fill();
   if(debug) {
      getrusage(RUSAGE_SELF,&r1);
      std::cout<<"  .... r="<<r
          <<" maxrss +"<<r1.ru_maxrss-r0.ru_maxrss
          <<" ixrss +"<<r1.ru_ixrss-r0.ru_ixrss
          <<" idrss +"<<r1.ru_idrss-r0.ru_idrss
          <<" isrss +"<<r1.ru_isrss-r0.ru_isrss
          <<"\n";
      debug--;
   }
   if(r) fNumWrites++;
   return r;
}

bool FTKPatternRootTree::WritePattern(FTKHitPattern const &hits,int coverage) {
   // write one pattern
   fMustSave=true;
   SetPattern(coverage,hits);
   static int debug=0;
   static struct rusage r0,r1;
   if(debug) {
      getrusage(RUSAGE_SELF,&r0);
      std::cout<<"WritePattern: sector="<<GetSectorNumber()
               <<" nLayer="<<GetPattern().GetHitPattern().GetSize()
          <<" numWrites="<<fNumWrites
          <<" maxrss="<<r0.ru_maxrss
          <<" ixrss="<<r0.ru_ixrss
          <<" idrss="<<r0.ru_idrss
          <<" isrss="<<r0.ru_isrss
          <<" fTTree->Fill()\n";
   }
   bool r=fTTreePtr->Fill();
   if(debug) {
      std::cout<<"  .... r="<<r
          <<" maxrss +"<<r1.ru_maxrss-r0.ru_maxrss
          <<" ixrss +"<<r1.ru_ixrss-r0.ru_ixrss
          <<" idrss +"<<r1.ru_idrss-r0.ru_idrss
          <<" isrss +"<<r1.ru_isrss-r0.ru_isrss
          <<"\n";
      debug--;
   }
   if(r) fNumWrites++;
   return r;
}

bool FTKPatternRootTree::ReadNextPattern() {
   // read one pattern
   bool r=false;
   if(HasMorePatterns()) {
      if(!fTTreePtr) LoadTree(0);
      r=fTTreePtr->GetEntry(fPatternNumber);
      fPatternNumber++;
   }
   if(r) fNumReads++;
   return r;
}
