/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* package FTKPatternBySector
 *
 * IO interface for reading and writing patterns to files
 * For reading patterns, chains of root-files are supported
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/FTKRootFile.h"
#include <TList.h>
#include <TFile.h>
#include <TObjString.h>

using namespace std;

static TDirectory *OpenRootFileReadonly(char const *path) {
   TFile *r=new TFile(path,"READ");
   if(r && r->IsZombie()) {
      // if TFile can not be opened, return 0
      delete r;
      r=0;
   }
   return r;
}

//================== class FTKPatternBySectorBase ========================

FTKPatternBySectorBase::~FTKPatternBySectorBase() {
}

int FTKPatternBySectorBase::SetNLayers(int nLayer) {
   // set number of layers
   int error=0;
   if(nLayer<=0) {
      if(nLayer<0) {
         Fatal("SetNLayers")
            <<"\""<<fSourceName<<"\" negative number of layers requested "
            <<nLayer<<"\n";
      } else {
         Error("SetNLayers")
            <<"\""<<fSourceName<<"\" zero number of layers requested\n";
      }
   } else {
      if(fNLayers) {
         if(nLayer!=fNLayers) {
            error++;
            Fatal("SetNLayers")
               <<" inconsistent number of layers: "
               <<fNLayers<<" requested: "<<nLayer<<"\n";
         }
      } else {
         fNLayers=nLayer;
         Info("SetNLayers")
            <<"\""<<fSourceName<<"\" number of layers is "<<fNLayers<<"\n";
      }
   }
   return error;
}

//================== class FTKPatternBySectorReader ========================

FTKPatternBySectorReader::FTKPatternBySectorReader(FTKRootFileChain &chain)
   : FTKPatternBySectorBase("FTKPatternBySectorReader") {
   int error(0); // not that this gets incremented but is currently not used!
   fSourceName=
      TString::Format("FTKRootFileChain(length=%d",chain.GetLength());
   for(int i=0;i<chain.GetLength();i++) {
      fSourceName += "<"+chain[i]+">";
   }
   fSourceName += ")";
   for(int i=0;i<chain.GetLength();i++) {
      // open root file
      TString inputFileName=chain[i];
      TDirectory *in=OpenRootFileReadonly(inputFileName);
      if(!in) {
         Error("(constructor)")
            <<"problem to open root file \""<<inputFileName<<"\", skipping\n";
         error++;
      } else {
         // read Sector information
         FTKPatternBySectorReader *input=new FTKPatternBySectorReader(*in);
         SetNLayers(input->GetNLayers());
         // store number of patterns per sector
         for(int sector=input->GetFirstSector();sector>=0;
             sector=input->GetNextSector(sector)) {
            FTKPatternRootTreeReader * &reader=fPatterns[sector];
            FTKPatternRootChainReader *chainReader=
               static_cast<FTKPatternRootChainReader *>(reader);
            if(!reader) {
               chainReader=new FTKPatternRootChainReader(chain,sector);
               reader=chainReader;
            }
            chainReader->Add(inputFileName,input->GetNPatterns(sector));
         }
         if(GetContentType()==CONTENT_EMPTY) {
            // if there is only one non-empty file in the chain 
            // then the whole chain get that attribute
            SetContentType(input->GetContentType());
         } else if(input->GetContentType()!=CONTENT_EMPTY) {
            // if there is more than one file with content,
            // the chain is not merged
            SetContentType(CONTENT_NOTMERGED);
         }
         delete input;
         delete in;
      }
   }
}

void FTKPatternBySectorReader::SelectSubregion(int nSub,int iSub) {
   if(nSub>1) {
      Info("SelectSubregion")
         <<"splitting subregions nSub="<<nSub<<" iSub="<<iSub<<"\n";
      int nAll=fPatterns.size();
      for(PatternTreeBySector_t::iterator iSector=fPatterns.begin();
          iSector!=fPatterns.end();) {
         int sector=(*iSector).first;
         PatternTreeBySector_t::iterator i0=iSector;
         iSector++;
         if((sector%nSub)!=iSub) {
            if((*i0).second) {
               delete (*i0).second;
               (*i0).second=0;
            }
            fPatterns.erase(i0);
         }
      }
      Info("SelectSubregion")<<"selected "<<fPatterns.size()<<"/"<<nAll
                             <<" sectors\n";
   }
}

FTKPatternBySectorReader::FTKPatternBySectorReader
(TDirectory &dir,const char *filename,std::set<int> const *sectorlist)
  : FTKPatternBySectorBase(filename) {
   // locate all TTree objects with pattern data
   // and store them in the STL map
   //   fPatterns
   // if a valid tree is found, also determine the number of layers
   fSourceName=dir.GetName();
   fNLayers=0;
   TIter next(dir.GetListOfKeys());
   TObject *o;
   while((o=next())) {
      TString name=((TObjString *)o)->GetString();
      int sector=FTKPatternRootTreeReader::ExtractSectorNumber(name);
      if((sector>=0)&&((!sectorlist)||
                       (sectorlist->find(sector)!=sectorlist->end()))) {
         FTKPatternRootTreeReader * &patternTree=fPatterns[sector];
         if(patternTree) delete patternTree;
         FTKPatternRootTree *tree=new FTKPatternRootTree(dir,sector,0);
         patternTree=tree;
         SetNLayers(tree->GetPattern().GetHitPattern().GetSize());
         // if there is a tree with data but the "merged"
         //   flag has not been found yet, flag as not merged
         if(GetContentType()==CONTENT_EMPTY) {
            SetContentType(CONTENT_NOTMERGED);
         }
      } else if(name=="IS_MERGED") {
         // if the "merged" flag is found
         //   then the file really is merged
         SetContentType(CONTENT_MERGED);
      }
   }
}

FTKPatternBySectorReader::~FTKPatternBySectorReader() {
    // clean up
   Long_t totalPatternsRead=0;
   int totalSectorsWithData=0;
   int totalSectors=0;
   for(PatternTreeBySector_t::iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      Long_t nPatterns=(*i).second->GetNumberOfPatternsRead();
      if(nPatterns>0) totalSectorsWithData++;
      totalPatternsRead += nPatterns;
      totalSectors++;
      delete (*i).second;
   }
   if(totalSectorsWithData) {
      Info("summary")
         <<totalSectorsWithData
         <<"/"<<totalSectors
         <<" sectors with a total of "<<totalPatternsRead
         <<" patterns read from \""<<fSourceName<<"\"\n";
   }
}

Long64_t FTKPatternBySectorReader::GetNPatterns(int sector) const {
   // return number of patterns in one sector
   Long64_t n=0;
   PatternTreeBySector_t::const_iterator i=fPatterns.find(sector);
   if(i != fPatterns.end()) {
      n=(*i).second->GetNPatterns();
   }
   return n;
}

Long64_t FTKPatternBySectorReader::GetNPatterns(void) const {
   // return total number of patterns
   Long64_t n=0;
   for(PatternTreeBySector_t::const_iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      n += (*i).second->GetNPatterns();
   }
   return n;
}

void FTKPatternBySectorReader::GetNPatternsByCoverage
(std::map<int,int> &coverageMap) const {
   if(GetContentType()==CONTENT_NOTMERGED) {
      Fatal("GetNPatternsByCoverage")<<"patterns are not merged\n";
   }
   //Info("GetNPatternsByCoverage")<<"number of sectors "<<fPatterns.size()<<"\n";
   for(PatternTreeBySector_t::const_iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      FTKPatternRootTreeReader *reader=(*i).second;
      reader->ReadCoverageOnly(true);
      reader->Rewind();
      while(reader->ReadNextPattern()) {
         coverageMap[reader->GetPattern().GetCoverage()]++;
      }
      reader->ReadCoverageOnly(false);      
   }
}

void FTKPatternBySectorReader::GetNPatternsBySectorCoverage
(std::map<int,std::map<int,int> > &sectorCoverageMap) const {
   if(GetContentType()==CONTENT_NOTMERGED) {
      Fatal("GetNPatternsBySectorCoverage")<<"patterns are not merged\n";
   }
   //Info("GetNPatternsBySectorCoverage")<<"number of sectors "<<fPatterns.size()<<"\n";
   for(PatternTreeBySector_t::const_iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      FTKPatternRootTreeReader *reader=(*i).second;
      map<int,int> &coverageMap(sectorCoverageMap[(*i).first]);
      reader->ReadCoverageOnly(true);
      reader->Rewind();
      while(reader->ReadNextPattern()) {
         coverageMap[reader->GetPattern().GetCoverage()]++;
      }
      reader->ReadCoverageOnly(false);      
   }
}

int FTKPatternBySectorReader::GetFirstSector(void) const {
   // return first (lowest) sector number
   if(fPatterns.begin()!=fPatterns.end()) {
      return (*fPatterns.begin()).first;
   } else {
      return -1;
   }
}

int FTKPatternBySectorReader::GetNextSector(int sector) const {
   // return next (higher) sector number
   // if there is none, return -1
   PatternTreeBySector_t::const_iterator i=fPatterns.upper_bound(sector);
   if(i!=fPatterns.end())
      return (*i).first;
   return -1;
}

FTKPatternOneSector *FTKPatternBySectorReader::MergePatterns
(int sector) const {
   // load and merge patterns from one sector into memory
   FTKPatternOneSector *r=new FTKPatternOneSector(sector);
   PatternTreeBySector_t::const_iterator sectorPtr=fPatterns.find(sector);
   if(sectorPtr!=fPatterns.end()) {
      FTKPatternRootTreeReader *patternTree=(*sectorPtr).second;
      patternTree->Rewind();
      while(patternTree->ReadNextPattern()) {
         r->AddPattern(patternTree->GetPattern());
      }
   }
   return r;
}


Long64_t FTKPatternBySectorReader::GetPatternByCoverage(SectorSet_t& sectorByCoverage, int iSub, int nSub) {
   //! Get patterns ordered by coverage
   //! return total number of patterns
   if(GetContentType()==CONTENT_NOTMERGED) {
      Error("GetPatternByCoverage")
         <<"data still have to be merged\n";
   }
   Long64_t nPatTotal=0;
   for(PatternTreeBySector_t::iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      int sector=(*i).first;
      // // skip all but the selected subregion
      if((sector %nSub)!=iSub) continue;
      FTKPatternRootTreeReader* const reader=(*i).second;
      nPatTotal += reader->GetNPatterns();
      reader->Rewind();
      if(reader->ReadNextPattern())
       	 sectorByCoverage.insert(i);
   }
   Info("GetPatternByCoverage")<<"number of sectors: "
			       <<sectorByCoverage.size()<<" total pattern count: "<<nPatTotal<<" (iSub="<<iSub<<", nSub="<<nSub<<")\n";
   return nPatTotal;
}


int FTKPatternBySectorReader::WritePatternsToASCIIstream(std::ostream &out,int iSub,int nSub) {
   // read all patterns from all sectors
   // write data ordered by
   //   coverage
   //   then sector number
   //   then keep the order of the hits in the input TTree 
   int error=0;
   if(GetContentType()==CONTENT_NOTMERGED) {
      Error("WritePatternsToASCIIstream")
         <<"data can not be saved as ASCII (still have to be merged)\n";
   }

   SectorSet_t sectorByCoverage;
   Long64_t nPatTotal = GetPatternByCoverage(sectorByCoverage,iSub,nSub);

   // write header
   out<<nPatTotal<<" "<<GetNLayers()<<"\n";

   // write patterns
   int number=0;   
   while(sectorByCoverage.begin()!=sectorByCoverage.end()) {
      PatternTreeBySector_t::iterator oneSector=*sectorByCoverage.begin();
      sectorByCoverage.erase(sectorByCoverage.begin());
      FTKPatternRootTreeReader *tree=(*oneSector).second;
      int sector=(*oneSector).first;
      int coverage=tree->GetPattern().GetCoverage();
      //if(coverage<minCoverage) break;
      bool isEmpty=false;
      do {
         if(!(number %1000)) {
            ShowProgress(TString::Format("%d/%d/%d",number,sector,coverage));
         }
         out<<number;
         number++;
         FTKHitPattern const &patternData=tree->GetPattern().GetHitPattern();
         for(unsigned iLayer=0;iLayer<patternData.GetSize();iLayer++) {
            out<<' '<<patternData.GetHit(iLayer);
         }
         out<<' '<<sector<<' '<<coverage<<"\n";
         if(out.fail()) {
            error++;
            Error("WritePatternsToASCIIstream")<<"write operation failed\n";
            break;
         }
         if(!tree->ReadNextPattern()) {
            isEmpty=true;
            break;
         }
      } while(tree->GetPattern().GetCoverage()==coverage);
      if(!isEmpty) {
         sectorByCoverage.insert(oneSector);
      }
   }
   Info("WritePatternsToASCIIstream")
      <<" total patterns written: "<<number<<"\n";
   return error;
}

//================== class FTKPatternBySectorBlockReader ===================

FTKPatternBySectorBlockReader::FTKPatternBySectorBlockReader
(TDirectory &dir,std::set<int> const *sectorList) :
   FTKPatternBySectorReader(dir,"FTKPatternBySectorBlockReader",sectorList) {
}

void FTKPatternBySectorBlockReader::Rewind(void) {
   if(GetContentType()==CONTENT_NOTMERGED) {
      Fatal("Rewind")<<"patterns are not merged\n";
   }
   for(PatternTreeBySector_t::iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      FTKPatternRootTreeReader *reader=(*i).second;
      reader->Rewind();
      /*bool returned =*/ reader->ReadNextPattern();
   }
}

FTKPatternOneSector *FTKPatternBySectorBlockReader::Read
(int sector,int minCoverage) {
   if(GetContentType()==CONTENT_NOTMERGED) {
      Fatal("Rewind")<<"patterns are not merged\n";
   }
   FTKPatternOneSector *r=0;
   PatternTreeBySector_t::iterator isector=fPatterns.find(sector);
   if(isector!=fPatterns.end()) {
      FTKPatternRootTreeReader *reader=(*isector).second;
      do {
         FTKPatternWithCoverage const &pattern=reader->GetPattern();
         if(pattern.GetCoverage()<minCoverage) break;
         if(!r) {
            r=new FTKPatternOneSector(sector);
         }
         r->AddPattern(pattern);
      } while( reader->ReadNextPattern());
   }
   return r;
}

//================== class FTKPatternBySectorWriter ========================

FTKPatternBySectorWriter::FTKPatternBySectorWriter(TDirectory &dir)
   : FTKPatternBySectorBase("FTKPatternBySectorWriter"),fDir(dir) {
   fSourceName=fDir.GetName();
}

FTKPatternBySectorWriter::~FTKPatternBySectorWriter() {
   // clean up
   Info("~FTKPatternBySectorWriter")<<"saving trees\n";
   int n=0;
   Long_t totalPatternsWritten=0;
   int totalSectorsWithData=0;
   int nPrint=0;
   TDirectory* lastDir=gDirectory;
   for(PatternTreeBySector_t::iterator i=fPatterns.begin();
       i!=fPatterns.end();i++) {
      if(!(nPrint %100)) {
	 ShowProgress(TString::Format("%d",(*i).first));
      }
      nPrint++;
      Long_t nPatterns=i->second->GetNumberOfPatternsWritten();
      if(nPatterns>0) totalSectorsWithData++;
      totalPatternsWritten += nPatterns;
      delete (*i).second;
      n++;
   }
   if(GetContentType()==CONTENT_MERGED) {
      fDir.cd();
      TObjString *flagAsOrdered=new TObjString("IS_MERGED");
      flagAsOrdered->Write();
      delete flagAsOrdered;
   }
   fDir.cd();
   fDir.SaveSelf();
   Info("summary")<<totalSectorsWithData
                  <<" sectors with a total of "<<totalPatternsWritten
                  <<" patterns written to: \""<<fDir.GetName()<<"\"\n";
   lastDir->cd(); // go back
}

int FTKPatternBySectorWriter::AppendPatternsFromASCIIstream
(std::istream &in) {
   // read patterns from a stream in ASCII format and append each pattern
   // to the TTree, determined by the sector number
   int error=0;
   Long64_t nPattern;
   int nLayerFile;
   // first two words are 
   in>>nPattern>>nLayerFile;
   if(in.fail()) {
      Error("AppendPatternsFromASCIIstream")
         <<"could not read number of patterns and layers\n";
      error++;
      nPattern=0;
   } else {
      SetNLayers(nLayerFile);
      Info("AppendPatternsFromASCIIstream")
         <<"nPattern="<<nPattern<<" nLayer="<<GetNLayers()<<"\n";
   }
   FTKPatternWithCoverage pattern(GetNLayers());
   int sector;
   fDir.cd();
   int nread=0;
   for(Long64_t i=0;i<nPattern;i++) {
      Long64_t number;
      Int_t data,coverage;
      if(!(i%100000)) {
         ShowProgress(TString::Format("%d00000",(int)(i/100000)));
      }
      in>>number;
      for(int iLayer=0;iLayer<GetNLayers();iLayer++) {
         in>>data;
         pattern.GetHitPattern().SetHit(iLayer,data);
      }
      in>>sector>>coverage;
      pattern.SetCoverage(coverage);
      if(in.fail()) {
         error++;
         Warning("AppendPatternsFromASCIIstream")
            <<"reading failed pos="<<i<<"\n";
         break;
      }
      error =AppendPattern(sector,pattern);
      if(error) {
         Error("AppendPatternsFromASCIIstream")
            <<"writing to root file failed pos="<<i<<"\n";
      }
      if(error) break;
      nread++;
   }
   if(nread!=nPattern) {
      Warning("AppendPatternsFromASCIIstream")
         <<"only "<<nread<<"/"<<nPattern<<" patterns imported successfully\n";
   }
   return error;
}

int FTKPatternBySectorWriter::AppendMergedPatterns
(FTKPatternBySectorReader &source,int minCoverage) {
   // loop over all sectors
   //   for each sector, import-and-merge patterns
   int error=0;
   SetNLayers(source.GetNLayers());
   // sector index, to be saved as extra TTree
   map<int,map<int,Long64_t> > coverageSectorBeginEnd; 
   Info("AppendMergedPatterns")<<"loop over sectors\n";
   int nTotal=0;
   int totalCoverage=0;
   int printed=0;
   double nextPrint=1.0;
   for(int sector=source.GetFirstSector();sector>=0;
       sector=source.GetNextSector(sector)) {
      //ShowProgress(TString::Format("%d",sector));
      FTKPatternOneSector *mergedPatterns=source.MergePatterns(sector);
      nTotal +=  mergedPatterns->GetNumberOfPatterns();
      FTKPatternOneSectorOrdered *orderedByCoverage=
         mergedPatterns->OrderPatterns(FTKPatternOrderByCoverage(minCoverage));
      totalCoverage += orderedByCoverage->GetSummedCoverage();
      printed++;
      if(printed>=nextPrint) {
         ShowProgress(TString::Format("%d %d %d %d",sector,
                                      mergedPatterns->GetNumberOfPatterns(),
                                      nTotal,totalCoverage));
         nextPrint=printed*1.3;
      }
      error=AppendMergedPatterns(sector,orderedByCoverage);
      if(error) {delete orderedByCoverage; delete mergedPatterns; break;}
      int coverage=0;
      int begin=0;
      int nPat=0;
      for(FTKPatternOneSectorOrdered::Ptr_t i=orderedByCoverage->Begin();
          i!=orderedByCoverage->End();i++) {
         if(orderedByCoverage->GetCoverage(i)<minCoverage) break;
         if(orderedByCoverage->GetCoverage(i)!=coverage) {
            if(nPat!=begin) {
               coverageSectorBeginEnd[coverage][sector]=
                  (((Long64_t)begin)<<32)|nPat;
            }
            begin=nPat;
            coverage=orderedByCoverage->GetCoverage(i);
         }
         nPat++;
      }
      if(nPat!=begin) {
         coverageSectorBeginEnd[coverage][sector]=
            (((Long64_t)begin)<<32)|nPat;
         
      }
      delete orderedByCoverage;
      delete mergedPatterns;
   }
   Info(TString::Format("nsector=%d npattern=%d npattern*coverage=%d",printed,
                        nTotal,totalCoverage));
   // save sector index as TTree
#ifdef WRITE_INDEX_TREE
   fDir.cd();
   TTree *tree=new TTree("sector_index","sector_index");
   int coverage,sector,begin,end;
   tree->Branch("coverage",&coverage,"coverage/I");
   tree->Branch("sector",&sector,"sector/I");
   tree->Branch("begin",&begin,"begin/I");
   tree->Branch("end",&end,"end/I");
   for(map<int,map<int,Long64_t> >::const_reverse_iterator
          i=coverageSectorBeginEnd.rbegin();i!=coverageSectorBeginEnd.rend();
       i++) {
      coverage=(*i).first;
      int nPattern=0;
      for(map<int,Long64_t>::const_iterator j=(*i).second.begin();
          j!=(*i).second.end();j++) {
         sector=(*j).first;
         begin=(*j).second>>32;
         end=(*j).second;
         nPattern += end-begin;
         tree->Fill();
      }
      Info("summary")
         <<"coverage="<<coverage<<" nsector="<<(*i).second.size()
         <<" npattern="<<nPattern<<"\n";
   }
   tree->AutoSave("SaveSelf");
   delete tree;
#endif
   return error;
}

int FTKPatternBySectorWriter::AppendMergedPatterns
(int sector,FTKPatternOneSectorOrdered const *orderedByCoverage) {
   // order patterns of one sector by coverage, then store them to disk
   int error=0;
   if(orderedByCoverage->Begin()!=orderedByCoverage->End()) {
      FTKPatternRootTree * &patternTree=fPatterns[sector];
      if(!patternTree) {
         patternTree=new FTKPatternRootTree(fDir,sector,GetNLayers());
         if(GetContentType()==CONTENT_EMPTY) {
            SetContentType(CONTENT_MERGED);
         }
      } else {
         SetContentType(CONTENT_NOTMERGED);
      }
      for(FTKPatternOneSectorOrdered::Ptr_t i=orderedByCoverage->Begin();
          i!=orderedByCoverage->End();i++) {
         error=
            !patternTree->WritePattern(orderedByCoverage->GetHitPattern(i),
                                       orderedByCoverage->GetCoverage(i));
         if(error) break;
      }
   }
   return error;
}

int FTKPatternBySectorWriter::AppendPattern
(int sector,FTKPatternWithCoverage const &pattern) {
   // add one pattern to the output
   int error=0;
   FTKPatternRootTree * &patternTree=fPatterns[sector];
   if(!patternTree) {
      patternTree=new FTKPatternRootTree(fDir,sector,GetNLayers());
      if(GetContentType()==CONTENT_EMPTY) {
         SetContentType(CONTENT_MERGED);
      }
   } else {
      SetContentType(CONTENT_NOTMERGED);
   }
   error= !patternTree->WritePattern(pattern);
   return error;
}

//================== class FTKPatternRootTreeReader ================

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

