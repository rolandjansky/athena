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
   m_patternNumber=0;
   m_numReads=0;
   m_TTreePtr=0;
   m_lastReadDir=0;
   m_sector=sector;
   m_treeName=ConstructTreeName(sector);
   m_pattern=0;
   m_doReadPattern=true;
}

FTKPatternRootTreeReader::~FTKPatternRootTreeReader() {
   if(m_TTreePtr) delete m_TTreePtr;
   if(m_pattern) delete m_pattern;
}

bool FTKPatternRootTreeReader::LoadTree(TDirectory *dir) {
   bool r=false;
   TTree * tree=0;
   if(!dir) dir=m_lastReadDir;
   m_lastReadDir=dir;
   dir->GetObject(m_treeName,tree);
   if(tree) {
      if(tree!=m_TTreePtr) {
         if(m_TTreePtr) delete m_TTreePtr;
         m_TTreePtr=0;
         TLeaf *leaf=tree->GetLeaf("pattern","pattern");
         int nLayer=0;
         if(leaf) {
            nLayer=leaf->GetLenStatic();
            if(nLayer>0) {
               if(!m_pattern) {
                  m_pattern=new FTKPatternWithCoverage(nLayer);
               } else if((int)m_pattern->GetHitPattern().GetSize()!=nLayer) {
                  Error("LoadTree tree")
                     <<"\""<<m_treeName
                     <<"\" branch/leaf \"pattern\" has bad length="
                     <<nLayer<<" (expected="
                     <<m_pattern->GetHitPattern().GetSize()<<")\n";
               }
            } else {
               Error("LoadTree")
                  <<"tree \""<<m_treeName
                  <<"\" branch/leaf \"pattern\" has non-static length="
                  <<nLayer<<"\n";
            }
         } else {
            Error("LoadTree")
               <<"tree \""<<m_treeName
               <<"\" does not have a branch/leaf named \"pattern\"\n";
         }
         if(nLayer>0) {
            bool error=false;
            error |= (tree->SetBranchAddress
                      ("pattern",m_pattern->GetHitPattern().GetAddress())
                      !=0 /* TTree::kMatch // enum is protected in 5.30 */);
            error |= (tree->SetBranchAddress
                      ("coverage",m_pattern->GetCoverageAddress())
                      !=0 /* TTree::kMatch // enum is protected in 5.30 */);
            if(error) {
               Error("LoadTree")
                  <<"tree \""<<m_treeName<<"\" does not have branches "
                  <<"pattern[NLAYER]/I and coverage/I\n";
            } else {
               m_TTreePtr=tree;
               m_TTreePtr->SetBranchStatus("pattern",m_doReadPattern);
               m_lastNumPatterns=m_TTreePtr->GetEntries();
            }
         }
         r=(m_TTreePtr !=0);
      } else {
         r=true;
      }
   } else {
      if(m_TTreePtr) delete m_TTreePtr;
      m_TTreePtr=0;
      Warning("LoadTree")<<"object \""<<m_treeName<<"\" is not a TTree\n";
   }
   return r;
}

void FTKPatternRootTreeReader::ReadCoverageOnly(bool noPatternRead) {
   m_doReadPattern= !noPatternRead;
   if(m_TTreePtr) {
      m_TTreePtr->SetBranchStatus("pattern",m_doReadPattern);
   }
}

bool FTKPatternRootTreeReader::CreateTree(TDirectory *dir,int nLayer) {
   // create new tree for writing
   if(m_pattern) {
      if((int)m_pattern->GetHitPattern().GetSize()!=nLayer) {
         Error("CreateTree")
            <<"inconsistent number of layers "<<nLayer
            <<" (expected "<<m_pattern->GetHitPattern().GetSize()<<")\n";
      }
   } else {
      m_pattern=new FTKPatternWithCoverage(nLayer);
   }
   dir->cd();
   if(m_TTreePtr) {
      Error("CreateTree")<<"TTree already existing\n";
      delete m_TTreePtr;
   }
   m_TTreePtr=new TTree(m_treeName,m_treeName);
   m_doReadPattern=true;
   if(m_TTreePtr) {
      // create branches
      m_TTreePtr->Branch("pattern",m_pattern->GetHitPattern().GetAddress(),
                     TString::Format("pattern[%d]/I",nLayer),32768);
      m_TTreePtr->Branch("coverage",m_pattern->GetCoverageAddress(),
                     "coverage/I",4096);
   }
   return m_TTreePtr!=0;
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
   if(m_TTreePtr) delete m_TTreePtr;
   m_TTreePtr=0;
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
      m_patternNumber=position;
   }
   return m_patternNumber;
}

bool FTKPatternRootTreeReader::HasMorePatterns(void) const {
   // check whether there are more patterns
   return m_patternNumber<GetNPatterns();
}

//================== class FTKPatternRootChainReader =================

void FTKPatternRootChainReader::Add(char const *name,Long64_t nPattern) {
   m_fileNames.push_back(name);
   m_numEntries.push_back(nPattern);
   Long_t sum=m_numEntriesSummed.size()
      ? m_numEntriesSummed[m_numEntriesSummed.size()-1] : 0;
   sum += nPattern;
   m_numEntriesSummed.push_back(sum);
}

Long64_t FTKPatternRootChainReader::GetNPatterns(void) const {
   int np=m_numEntriesSummed.size()-1;
   return (np>=0) ? m_numEntriesSummed[np] : 0;
}

bool FTKPatternRootChainReader::ReadNextPattern(void) {
   int iFile;
   for(iFile=0;iFile<(int)m_numEntriesSummed.size()-1;iFile++) {
      if(m_patternNumber<m_numEntriesSummed[iFile]) break;
   }
   int fileMin=iFile ? m_numEntriesSummed[iFile-1] : 0;
   if((m_currentFile!=iFile)|| !m_TTreePtr) {
      if(m_TTreePtr) delete m_TTreePtr;
      m_TTreePtr=0;
      m_currentFile=iFile;
      m_chain.Open(m_fileNames[m_currentFile]);
      LoadTree(m_chain.GetDirectory());
   }
   // read pattern
   bool r=false;
   if(m_TTreePtr) {
      r=m_TTreePtr->GetEntry(m_patternNumber-fileMin);
      m_patternNumber++;
   }
   if(r) m_numReads++;
   return r;
}

//================== class FTKPatternRootTree ========================

FTKPatternRootTree::FTKPatternRootTree(TDirectory &dir,int sector,int nLayer)
   : FTKPatternRootTreeReader(sector,"FTKPatternRootTree") {
   m_mustSave=false;
   m_numWrites=0;
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
      m_mustSave=true;
   }
}

FTKPatternRootTree::~FTKPatternRootTree() {
   // save TTree to disk
    // if(m_mustSave && fTTree) {
    //    fTTree->AutoSave("SaveSelf"); //needed?
    // }
    if(m_mustSave && m_TTreePtr) {
       //cout<<"nentries: "<<fTTree->GetEntries()<<", name="<<fTTree->GetName()<<endl;
       m_TTreePtr->Write(0,TObject::kOverwrite);
    }
}


Long64_t FTKPatternRootTree::GetNPatterns(void) const {
   return m_lastNumPatterns;
}

bool FTKPatternRootTree::WritePattern(FTKPatternWithCoverage const &pattern) {
   // write one pattern
   m_mustSave=true;
   SetPattern(pattern);
   static int debug=0;
   struct rusage r0,r1;
   if(debug) {
      getrusage(RUSAGE_SELF,&r0);
      std::cout<<"WritePattern: sector="<<GetSectorNumber()
               <<" nLayer="<<GetPattern().GetHitPattern().GetSize()
          <<" numWrites="<<m_numWrites
          <<" maxrss="<<r0.ru_maxrss
          <<" ixrss="<<r0.ru_ixrss
          <<" idrss="<<r0.ru_idrss
          <<" isrss="<<r0.ru_isrss
          <<" fTTree->Fill()\n";
   }
   bool r=m_TTreePtr->Fill();
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
   if(r) m_numWrites++;
   return r;
}

bool FTKPatternRootTree::WritePattern(FTKHitPattern const &hits,int coverage) {
   // write one pattern
   m_mustSave=true;
   SetPattern(coverage,hits);
   static int debug=0;
   static struct rusage r0,r1;
   if(debug) {
      getrusage(RUSAGE_SELF,&r0);
      std::cout<<"WritePattern: sector="<<GetSectorNumber()
               <<" nLayer="<<GetPattern().GetHitPattern().GetSize()
          <<" numWrites="<<m_numWrites
          <<" maxrss="<<r0.ru_maxrss
          <<" ixrss="<<r0.ru_ixrss
          <<" idrss="<<r0.ru_idrss
          <<" isrss="<<r0.ru_isrss
          <<" fTTree->Fill()\n";
   }
   bool r=m_TTreePtr->Fill();
   if(debug) {
      std::cout<<"  .... r="<<r
          <<" maxrss +"<<r1.ru_maxrss-r0.ru_maxrss
          <<" ixrss +"<<r1.ru_ixrss-r0.ru_ixrss
          <<" idrss +"<<r1.ru_idrss-r0.ru_idrss
          <<" isrss +"<<r1.ru_isrss-r0.ru_isrss
          <<"\n";
      debug--;
   }
   if(r) m_numWrites++;
   return r;
}

bool FTKPatternRootTree::ReadNextPattern() {
   // read one pattern
   bool r=false;
   if(HasMorePatterns()) {
      if(!m_TTreePtr) LoadTree(0);
      r=m_TTreePtr->GetEntry(m_patternNumber);
      m_patternNumber++;
   }
   if(r) m_numReads++;
   return r;
}
