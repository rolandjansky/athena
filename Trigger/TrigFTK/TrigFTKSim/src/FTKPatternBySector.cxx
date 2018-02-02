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
#include "TrigFTKSim/FTKSSMap.h"
#include <TList.h>
#include <TFile.h>
#include <TChain.h>
#include <TObjString.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

//================== class FTKPatternBySectorBase ========================

FTKPatternBySectorBase::~FTKPatternBySectorBase() {
}

int FTKPatternBySectorBase::SetNLayers(int nLayer) {
   // set number of layers
   int error=0;
   if(nLayer<=0) {
      if(nLayer<0) {
         Fatal("SetNLayers")
            <<"\""<<m_sourceName<<"\" negative number of layers requested "
            <<nLayer<<"\n";
      } else {
         Error("SetNLayers")
            <<"\""<<m_sourceName<<"\" zero number of layers requested\n";
      }
   } else {
      if(m_NLayers) {
         if(nLayer!=m_NLayers) {
            error++;
            Fatal("SetNLayers")
               <<" inconsistent number of layers: "
               <<m_NLayers<<" requested: "<<nLayer<<"\n";
         }
      } else {
         m_NLayers=nLayer;
         Info("SetNLayers")
            <<"\""<<m_sourceName<<"\" number of layers is "<<m_NLayers<<"\n";
      }
   }
   return error;
}

//================== class FTKPatternBySectorReader ====

FTKPatternBySectorReader::FTKPatternBySectorReader(char const *name)
   : FTKPatternBySectorBase(name) {
}

FTKPatternBySectorReader::~FTKPatternBySectorReader() {
}

Long64_t FTKPatternBySectorReader::GetNPatternsTotal(void) const {
   // return total number of patterns
   Long64_t n=0;
   for(int sector=GetFirstSector();sector>=0;sector=GetNextSector(sector)) {
      n += GetNPatterns(sector);
   }
   return n;
}

void FTKPatternBySectorReader::GetNPatternsByCoverage
(map<int,int> &coverageMap) const {
   coverageMap.clear();
   for(SectorByCoverage_t::const_iterator c=m_SectorByCoverage.begin();
       c!=m_SectorByCoverage.end();c++) {
      int &n=coverageMap[(*c).first];
      for(SectorMultiplicity_t::const_iterator s=(*c).second.begin();
          s!=(*c).second.end();s++) {
         n+= (*s).second;
      }
   }
}

int FTKPatternBySectorReader::GetMaxCoverage(void) const {
   int r=0;
   if(!m_SectorByCoverage.empty())
      r=(*m_SectorByCoverage.rbegin()).first;
   return r;
}

int FTKPatternBySectorReader::GetNextCoverage(int coverage) const {
   SectorByCoverage_t::const_iterator c=m_SectorByCoverage.find(coverage);
   int r=0;
   if(c!=m_SectorByCoverage.end()) {
      --c;
      if(c!=m_SectorByCoverage.end()) r=(*c).first;
   }
   return r;
}

int FTKPatternBySectorReader::GetCoverageFirstSector(int coverage) const {
   int r=-1;
   SectorByCoverage_t::const_iterator c=m_SectorByCoverage.find(coverage);
   if(c!=m_SectorByCoverage.end()) {
      SectorMultiplicity_t::const_iterator s=(*c).second.begin();
      if(s!=(*c).second.end()) r=(*s).first;
   }
   return r;
}

int FTKPatternBySectorReader::GetCoverageNextSector
(int coverage,int sector) const {
   int r=-1;
   SectorByCoverage_t::const_iterator c=m_SectorByCoverage.find(coverage);
   if(c!=m_SectorByCoverage.end()) {
      SectorMultiplicity_t::const_iterator s=(*c).second.find(sector);
      if(s!=(*c).second.end()) {
         ++s;
         if(s!=(*c).second.end()) {
            r=(*s).first;
         }
      }
   }
   return r;
}

int FTKPatternBySectorReader::WritePatternsToASCIIstream
(ostream &out,int iSub,int nSub) {
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
   // write header
   out<<GetNPatternsTotal()<<" "<<GetNLayers()<<"\n";

   // write patterns
   int number=0;
   Rewind();
   // coverage loop
   for(int coverage=GetMaxCoverage();coverage;
       coverage=GetNextCoverage(coverage)) {
      // sector loop
      for(int sector=GetCoverageFirstSector(coverage);sector>=0;
          sector=GetCoverageNextSector(coverage,sector)) {
         if((nSub>1)&&(sector%nSub)!=iSub) continue;
         FTKPatternOneSector *data=Read(sector,coverage);
         // pattern loop
         for(FTKPatternOneSector::Ptr_t i=data->Begin();i!=data->End();i++) {
            // progress monitor
            if(!(number %1000)) {
               ShowProgress(TString::Format("%d/%d/%d",number,sector,coverage));
            }
            // write pattern
            out<<number;
            FTKHitPattern const &patternData=data->GetHitPattern(i);
            for(unsigned iLayer=0;iLayer<patternData.GetSize();iLayer++) {
               out<<' '<<patternData.GetHit(iLayer);
            }
            out<<' '<<sector<<' '<<coverage<<"\n";
            number++;
         }
         delete data;
         if(out.fail()) {
            error++;
            Error("WritePatternsToASCIIstream")<<"write operation failed\n";
            break;
         }
      }
   }
   Info("WritePatternsToASCIIstream")
      <<" total patterns written: "<<number<<"\n";
   return error;
}

void FTKPatternBySectorReader::Rewind(void) {
   for(int sector=GetFirstSector();sector>=0;sector=GetNextSector(sector)) {
      RewindSector(sector);
   }
}

FTKPatternBySectorReader *FTKPatternBySectorReader::Factory
(TDirectory &dir,set<int> const *sectorList)  {
   FTKPatternBySectorReader *reader;
   int errorIndex=0;
   int errorForest=0;
   reader=new FTKPatternBySectorIndexedReader(dir,sectorList,&errorIndex);
   if(errorIndex) {
     if (reader) delete reader;
      reader=new FTKPatternBySectorForestReader(dir,sectorList,&errorForest);
      if(errorForest) {
 	 if (reader) delete reader;
         reader=new FTKPatternBySectorIndexedReader(dir,sectorList,0);
         delete reader;
         reader=new FTKPatternBySectorForestReader(dir,sectorList,&errorForest);
         delete reader;
         reader=0;
      }
   }
   return reader;
}

FTKPatternBySectorReader *FTKPatternBySectorReader::Factory
(FTKRootFileChain &chain) {
   FTKPatternBySectorReader *reader;
   int errorIndex=0;
   int errorForest=0;
   reader=new FTKPatternBySectorIndexedReader(chain,&errorIndex);
   if(errorIndex) {
      delete reader;
      reader=new FTKPatternBySectorForestReader(chain,&errorForest);
      if(errorForest) {
         ostream &out=reader->Error("FTKPatternBySectorReader");
         out<<"Can not read some files of the chain [";
         for(int i=0;i<chain.GetLength();i++) {
            if(i) out<<",";
            out<<chain[i];
         }
         out<<"]\n";
         reader=new FTKPatternBySectorIndexedReader(chain,0);
         delete reader;
         reader=new FTKPatternBySectorForestReader(chain,0);
         delete reader;
         reader=0;
      }
   }
   return reader;
}

int FTKPatternBySectorReader::ReadRaw
(int firstSector,list<FTKPatternOneSector *> &sectorList,uint64_t minPattern) {
   int sector;
   uint64_t total=0;
   for(sector=firstSector;sector>=0;sector=GetNextSector(sector)) {
      FTKPatternOneSector *sectorData=Read(sector,0);
      if(!sectorData) {
         sector=-1;
         break;
      }
      sectorList.push_back(sectorData);
      total+= sectorData->GetNumberOfPatterns();
      if(total>=minPattern) break;
   }
   return sector;
}


//================== class FTKPatternBySectorWriter ====

FTKPatternBySectorWriter *FTKPatternBySectorWriter::Factory
(TDirectory &dir,WRITERTYPE_t type) {
   if(type==kWriterForest)
      return new FTKPatternBySectorForestWriter(dir);
   else if(type==kWriterIndexed)
      return new FTKPatternBySectorIndexedWriter(dir);
   // default
   return new FTKPatternBySectorIndexedWriter(dir);
}

#ifdef OLD_ALGO
int FTKPatternBySectorWriter::AppendMergedPatterns
(FTKPatternBySectorReader &source,int minCoverage) {
   // loop over all sectors
   //   for each sector, import-and-merge patterns
   int error=0;
   SetNLayers(source.GetNLayers());
   //Info("AppendMergedPatterns")<<"loop over sectors\n";
   int nTotal=0;
   int64_t totalCoverage=0;
   int printed=0;
   double nextPrint=1.0;
   static int printSector=0;

   struct rusage startUsage;
   int numSectorUsage=0;
   getrusage(RUSAGE_SELF,&startUsage);

   for(int sector=source.GetFirstSector();sector>=0;
       sector=source.GetNextSector(sector)) {
      //ShowProgress(TString::Format("sector %d",sector));
      FTKPatternOneSector *mergedPatterns=source.MergePatterns(sector);
      if(printSector) {
         printSector--;
         int n=mergedPatterns->GetNumberOfPatterns();
         FTKPatternOneSector::Ptr_t p=mergedPatterns->Begin();
         for(int k=0;k<2;k++) {
            for(int i=0;(i<5)&&(p!=mergedPatterns->End());i++,p++) {
               cout<<mergedPatterns->GetNumberOfPatterns()-n
                   <<" "<<sector<<" "<<mergedPatterns->GetCoverage(p);
               for(int j=0;j<GetNLayers();j++) {
                  cout<<" "<<mergedPatterns->GetHitPattern(p).GetHit(j);
               }
               cout<<"\n";
            }
            --n;
            if(n>10) {
               cout<<" ...\n";
               while(n>10) {
                  --n;
                  ++p;
               }
            }
         }
         cout<<"    ...\n";
      }
      nTotal +=  mergedPatterns->GetNumberOfPatterns();
      FTKPatternOneSectorOrdered *orderedByCoverage=
         mergedPatterns->OrderPatterns(FTKPatternOrderByCoverage(minCoverage));
      totalCoverage += orderedByCoverage->GetSummedCoverage();
      printed++;
      struct rusage currentUsage;
      numSectorUsage++;
      getrusage(RUSAGE_SELF,&currentUsage);
      struct timeval tdiff;
      timersub(&currentUsage.ru_utime,&startUsage.ru_utime,&tdiff);
      double dt=(tdiff.tv_sec+tdiff.tv_usec/1.E6)/numSectorUsage;
      if(printed>=nextPrint) {
         ShowProgress(TString::Format
                      ("sector %d patterns %d total %d %ld time/sector %.3f (nsector=%d)",
                       sector,
                       mergedPatterns->GetNumberOfPatterns(),nTotal,
                       totalCoverage,dt,numSectorUsage));
         nextPrint=printed*1.3;
      }
      error=AppendMergedPatternsSector(sector,orderedByCoverage);
      if(error) {delete orderedByCoverage; delete mergedPatterns; break;}
      delete orderedByCoverage;
      delete mergedPatterns;
   }
   Info("AppendMergedPatterns")
      <<"nsector="<<printed<<" npattern="<<nTotal
      <<" npattern*coverage="<<totalCoverage<<"\n";
   // save sector index as TTree
   return error;
}
#else

const uint64_t FTKPatternBySectorWriter::PATTERN_CHUNK=20000000;


int FTKPatternBySectorWriter::AppendMergedPatterns
(FTKPatternBySectorReader &source,int minCoverage) {
   // loop over all sectors
   //   for each sector, import-and-merge patterns
   int error=0;
   SetNLayers(source.GetNLayers());
   Info("AppendMergedPatterns")<<"loop over sectors\n";
   int nTotal=0;
   int64_t totalCoverage=0;
   int printed=1;
   double nextPrint=1.0;
   struct rusage startUsage;
   int numSectorUsage=0;
   getrusage(RUSAGE_SELF,&startUsage);

   int readSector=source.GetFirstSector();
   while(readSector>=0) {
      std::list<FTKPatternOneSector *> sectorList;
      readSector=source.ReadRaw(readSector,sectorList,PATTERN_CHUNK);
      int nSector=0;
      int nPattern=0;
      for(std::list<FTKPatternOneSector *>::iterator iSector=sectorList.begin();
          iSector!=sectorList.end();iSector++) {
         FTKPatternOneSector *mergedPatterns=(*iSector);
         nTotal +=  mergedPatterns->GetNumberOfPatterns();
         FTKPatternOneSectorOrdered *orderedByCoverage=
            mergedPatterns->OrderPatterns
            (FTKPatternOrderByCoverage(minCoverage));
         totalCoverage += orderedByCoverage->GetSummedCoverage();
         error+=AppendMergedPatternsSector(mergedPatterns->GetSector(),
                                           orderedByCoverage);
         delete orderedByCoverage;
         delete (*iSector);
         nSector++;
         nPattern+=  mergedPatterns->GetNumberOfPatterns();
      }
      struct rusage currentUsage;
      numSectorUsage+=nSector;
      getrusage(RUSAGE_SELF,&currentUsage);
      struct timeval tdiff;
      timersub(&currentUsage.ru_utime,&startUsage.ru_utime,&tdiff);
      double dt=(tdiff.tv_sec+tdiff.tv_usec/1.E6)/numSectorUsage;
      if(printed>=nextPrint) {
         ShowProgress(TString::Format
                      ("patterns %d total %d %ld time/sector %.3f (nsector=%d)",
                       nPattern,nTotal,
                       totalCoverage,dt,numSectorUsage));
         nextPrint=printed*1.3;
      }
      printed++;
   }
   Info("AppendMergedPatterns")
      <<"nsector="<<printed<<" npattern="<<nTotal
      <<"npattern*coverage="<<totalCoverage<<"\n";
   // save sector index as TTree
   return error;
}
#endif

int FTKPatternBySectorWriter::AppendPatternsFromASCIIstream
(istream &in) {
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
   m_dir.cd();
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

int FTKPatternBySectorWriter::AppendMergedPatternsSector
(int sector,FTKPatternOneSectorOrdered const *orderedByCoverage) {
   int error=0;
   for(FTKPatternOneSectorOrdered::Ptr_t i=orderedByCoverage->Begin();
       i!=orderedByCoverage->End();i++) {
      error=AppendPattern(sector,orderedByCoverage->GetCoverage(i),
                          orderedByCoverage->GetHitPattern(i));
      if(error) {
         Error("AppendMergedPatternsSector")<<"error="<<error<<"\n";
         break;
      }
   }
   return error;
}

//================== class FTKPatternBySectorForestReader ====

FTKPatternBySectorForestReader::FTKPatternBySectorForestReader(FTKRootFileChain &chain,int *error)
   : FTKPatternBySectorReader("FTKPatternBySectorForestReader") {
   m_sourceName=
      TString::Format("FTKRootFileChain(length=%d",chain.GetLength());
   for(int i=0;i<chain.GetLength();i++) {
      m_sourceName += "<"+chain[i]+">";
   }
   m_sourceName += ")";
   for(int i=0;i<chain.GetLength();i++) {
      // open root file
      TString inputFileName=chain[i];
      TDirectory *in=
         FTKRootFile::Instance()->OpenRootFileReadonly(inputFileName);
      if(!in) {
         if(error) (*error)++;
         else
            Error("(constructor)")
               <<"problem to open root file \""<<inputFileName<<"\", skipping\n";
      } else {
         // read Sector information
         FTKPatternBySectorForestReader *input=
            new FTKPatternBySectorForestReader(*in);
         SetNLayers(input->GetNLayers());
         // store number of patterns per sector
         for(int sector=input->GetFirstSector();sector>=0;
             sector=input->GetNextSector(sector)) {
            FTKPatternRootTreeReader * &reader=m_patterns[sector];
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
   InitializeSectorByCoverageTable();
}

bool FTKPatternBySectorForestReader::CheckConsistency
(FTKSSMap *ssMap,int tower,int hwmodeid) const {
   if(ssMap &&(tower>=0) &&(hwmodeid>=0)) {
      Warning("CheckConsistency")<<"not implemented\n";
   }
   return true;
}

FTKPatternBySectorForestReader::FTKPatternBySectorForestReader
(TDirectory &dir,set<int> const *sectorlist,
 int *error)
  : FTKPatternBySectorReader("FTKPatternBySectorForestReader") {
   // locate all TTree objects with pattern data
   // and store them in the STL map
   //   m_patterns
   // if a valid tree is found, also determine the number of layers
   m_sourceName=dir.GetName();
   m_NLayers=0;
   TIter next(dir.GetListOfKeys());
   TObject *o;
   while((o=next())) {
      TString name=((TObjString *)o)->GetString();
      int sector=FTKPatternRootTreeReader::ExtractSectorNumber(name);
      // for testing
      //if(sector>100) continue;
      if((sector>=0)&&((!sectorlist)||
                       (sectorlist->find(sector)!=sectorlist->end()))) {
         FTKPatternRootTreeReader * &patternTree=m_patterns[sector];
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
   InitializeSectorByCoverageTable();
   if(GetContentType()==CONTENT_EMPTY) {
      if(error) {
         (*error)++;
      } else {
         Warning("FTKPatternBySectorForestReader")
            <<"no pattern bank found in "<<dir.GetName()<<"\n";
      }
   }
}

void FTKPatternBySectorForestReader::InitializeSectorByCoverageTable(void) {
   if(GetContentType()!=CONTENT_NOTMERGED) {
      int nSector=0;
      int nPattern=0;
      for(PatternTreeBySectorRO_t::const_iterator i=m_patterns.begin();
          i!=m_patterns.end();i++) {
         int sector=(*i).first;
         FTKPatternRootTreeReader *reader=(*i).second;
         reader->ReadCoverageOnly(true);
         reader->Rewind();
         while(reader->ReadNextPattern()) {
            m_SectorByCoverage
               [reader->GetPattern().GetCoverage()][sector]++;
            nPattern++;
         }
         reader->Rewind();
         reader->ReadCoverageOnly(false);      
         nSector++;
      }
      Info("InitializeSectorByCoverageTable")
         <<"nSector="<<nSector<<" nPattern="<<nPattern
         <<" nCoverage="<<m_SectorByCoverage.size()<<"\n";
   } else {
      Info("InitializeSectorByCoverageTable")<<"not ordered\n";
   }
}

FTKPatternBySectorForestReader::~FTKPatternBySectorForestReader() {
    // clean up
   Long_t totalPatternsRead=0;
   int totalSectorsWithData=0;
   int totalSectors=0;
   for(PatternTreeBySectorRO_t::iterator i=m_patterns.begin();
       i!=m_patterns.end();i++) {
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
         <<" patterns read from \""<<m_sourceName<<"\"\n";
   }
}

Long64_t FTKPatternBySectorForestReader::GetNPatterns(int sector) const {
   // return number of patterns in one sector
   Long64_t n=0;
   PatternTreeBySectorRO_t::const_iterator i=m_patterns.find(sector);
   if(i != m_patterns.end()) {
      n=(*i).second->GetNPatterns();
   }
   return n;
}

int FTKPatternBySectorForestReader::GetFirstSector(void) const {
   // return first (lowest) sector number
   if(m_patterns.begin()!=m_patterns.end()) {
      return (*m_patterns.begin()).first;
   } else {
      return -1;
   }
}

int FTKPatternBySectorForestReader::GetNextSector(int sector) const {
   // return next (higher) sector number
   // if there is none, return -1
   PatternTreeBySectorRO_t::const_iterator i=m_patterns.upper_bound(sector);
   if(i!=m_patterns.end())
      return (*i).first;
   return -1;
}

void FTKPatternBySectorForestReader::RewindSector(int sector) {
   PatternTreeBySectorRO_t::iterator i=m_patterns.find(sector);
   if(i!=m_patterns.end()) {
      FTKPatternRootTreeReader *reader=(*i).second;
      reader->Rewind();
      reader->ReadNextPattern();
      reader->Suspend();
   }
}

FTKPatternOneSector *FTKPatternBySectorForestReader::Read
(int sector,int minCoverage) {
   if((GetContentType()==CONTENT_NOTMERGED)&&(minCoverage>1)) {
      Fatal("Read")<<"patterns are not merged\n";
   }
   FTKPatternOneSector *r=new FTKPatternOneSector(sector);
   PatternTreeBySectorRO_t::iterator isector=m_patterns.find(sector);
   if(isector!=m_patterns.end()) {
      FTKPatternRootTreeReader *reader=(*isector).second;
      while(reader->ReadNextPattern()) {
         FTKPatternWithCoverage const &pattern=reader->GetPattern();
         if(pattern.GetCoverage()<minCoverage) {
            Long64_t pos=reader->SeekBeg(-1);
            reader->SeekBeg(pos-1);
            break;
         }
         r->AddPattern(pattern);
      }
      reader->Suspend();
   }
   return r;
}

//================== class FTKPatternBySectorForestWriter ==================

FTKPatternBySectorForestWriter::FTKPatternBySectorForestWriter(TDirectory &dir)
   : FTKPatternBySectorWriter("FTKPatternBySectorWriter",dir) {
   m_sourceName=m_dir.GetName();
   Info("FTKPatternBySectorForestWriter")<<"dir="<<m_sourceName<<"\n";
}

FTKPatternBySectorForestWriter::~FTKPatternBySectorForestWriter() {
   // clean up
   Info("~FTKPatternBySectorWriter")<<"saving trees\n";
   int n=0;
   Long_t totalPatternsWritten=0;
   int totalSectorsWithData=0;
   int nPrint=0;
   TDirectory* lastDir=gDirectory;
   for(PatternTreeBySectorRW_t::iterator i=m_patterns.begin();
       i!=m_patterns.end();i++) {
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
      m_dir.cd();
      TObjString *flagAsOrdered=new TObjString("IS_MERGED");
      flagAsOrdered->Write();
      delete flagAsOrdered;
   }
   m_dir.cd();
   m_dir.SaveSelf();
   Info("summary")<<totalSectorsWithData
                  <<" sectors with a total of "<<totalPatternsWritten
                  <<" patterns written to: \""<<m_dir.GetName()<<"\"\n";
   lastDir->cd(); // go back
}

int FTKPatternBySectorForestWriter::AppendMergedPatternsSector
(int sector,FTKPatternOneSectorOrdered const *orderedByCoverage) {
   // order patterns of one sector by coverage, then store them to disk
   int error=0;
   if(orderedByCoverage->Begin()!=orderedByCoverage->End()) {
      FTKPatternRootTree * &patternTree=m_patterns[sector];
      if(!patternTree) {
         patternTree=new FTKPatternRootTree(m_dir,sector,GetNLayers());
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

int FTKPatternBySectorForestWriter::AppendPattern
(int sector,int coverage,FTKHitPattern const &pattern) {
   // add one pattern to the output
   int error=0;
   FTKPatternRootTree * &patternTree=m_patterns[sector];
   if(!patternTree) {
      patternTree=new FTKPatternRootTree(m_dir,sector,GetNLayers());
      if(GetContentType()==CONTENT_EMPTY) {
         SetContentType(CONTENT_MERGED);
      }
   } else {
      SetContentType(CONTENT_NOTMERGED);
   }
   error= !patternTree->WritePattern(pattern,coverage);
   return error;
}

//================== class FTKPatternBySectorIndexed ==================

/* int FTKPatternBySectorIndexed::GetBitsPerPlane(void) const {
   return 64/GetNLayers();
   } */

char const *FTKPatternBySectorIndexed::s_cdataTreeName="CpatternData";
char const *FTKPatternBySectorIndexed::s_indexTreeName="CpatternIndex";

char const *FTKPatternBySectorIndexed::s_cnpattBranchName="npatt";
char const *FTKPatternBySectorIndexed::s_cnpattBranchID="npatt/i";

char const *FTKPatternBySectorIndexed::GetCPatternBranchName(uint32_t plane) {
   while(s_cpatternBranch.size()<=plane) {
      int n=s_cpatternBranch.size();
      s_cpatternBranch.push_back(Form("cpattern%d",n));
   }
   return s_cpatternBranch[plane].c_str();
}

char const *FTKPatternBySectorIndexed::GetCPatternBranchID(uint32_t plane) {
   while(s_cpatternID.size()<=plane) {
      int n=s_cpatternID.size();
      s_cpatternID.push_back(Form("cpattern%d[npatt]/b",n));
   }
   return s_cpatternID[plane].c_str();
}

char const *FTKPatternBySectorIndexed::s_sectorBranchName="sector";
char const *FTKPatternBySectorIndexed::s_sectorBranchID="sector/i";

char const *FTKPatternBySectorIndexed::s_ncovBranchName="ncov";
char const *FTKPatternBySectorIndexed::s_ncovBranchID="ncov/i";

char const *FTKPatternBySectorIndexed::s_npatternBranchName="npattern";
char const *FTKPatternBySectorIndexed::s_npatternBranchID="npattern[ncov]/i";

char const *FTKPatternBySectorIndexed::s_coverageBranchName="coverage";
char const *FTKPatternBySectorIndexed::s_coverageBranchID="coverage[ncov]/i";

vector<string> FTKPatternBySectorIndexed::s_cpatternBranch,
   FTKPatternBySectorIndexed::s_cpatternID,
   FTKPatternBySectorIndexed::s_ndecoderBranch,
   FTKPatternBySectorIndexed::s_decoderDataBranch,
   FTKPatternBySectorIndexed::s_ndecoderID,
   FTKPatternBySectorIndexed::s_decoderDataID;

char const *FTKPatternBySectorIndexed::GetNDecoderBranchName(uint32_t plane) {
   while(s_ndecoderBranch.size()<=plane) {
      int n=s_ndecoderBranch.size();
      s_ndecoderBranch.push_back(Form("ndecoder%d",n));
   }
   return s_ndecoderBranch[plane].c_str();
}

char const *FTKPatternBySectorIndexed::GetNDecoderBranchID(uint32_t plane) {
   while(s_ndecoderID.size()<=plane) {
      int n=s_ndecoderID.size();
      s_ndecoderID.push_back(Form("ndecoder%d/i",n));
   }
   return s_ndecoderID[plane].c_str();
}

char const *FTKPatternBySectorIndexed::GetDecoderDataBranchName(uint32_t plane) {
   while(s_decoderDataBranch.size()<=plane) {
      int n=s_decoderDataBranch.size();
      s_decoderDataBranch.push_back(Form("decoderData%d",n));
   }
   return s_decoderDataBranch[plane].c_str();
}

char const *FTKPatternBySectorIndexed::GetDecoderDataBranchID(uint32_t plane) {
   while(s_decoderDataID.size()<=plane) {
      int n=s_decoderDataID.size();
      s_decoderDataID.push_back(Form("decoderData%d[ndecoder%d]/i",n,n));
   }
   return s_decoderDataID[plane].c_str();
}

//================== class FTKPatternBySectorIndexedWriter ==================

FTKPatternBySectorIndexedWriter::FTKPatternBySectorIndexedWriter
(TDirectory &dir)
   : FTKPatternBySectorWriter("FTKPatternBySectorIndexedWriter",dir) {
   m_sourceName=m_dir.GetName();
   Info("FTKPatternBySectorIndexedWriter")<<"dir="<<m_sourceName<<"\n";
m_cpatternDataTree=0;
   m_cpatternIndexTree=0;
   m_patternStorage=0;
}

FTKPatternBySectorIndexedWriter::~FTKPatternBySectorIndexedWriter() {
   Flush();
   m_dir.cd();
   m_cpatternDataTree->Write(0,TObject::kOverwrite);
   m_cpatternIndexTree->Write(0,TObject::kOverwrite);
   delete m_cpatternDataTree;
   delete m_cpatternIndexTree;
   //  if (m_patternData) delete m_patternData;
}

int FTKPatternBySectorIndexedWriter::AppendPattern
(int sector,int coverage,FTKHitPattern const &pattern) {
   // try two times
   // at the end of each try, flush the table, so there is enough space
   // for the second attempt
   int error;
   static int nPattern=0;
   static int flush1=0;
   static int flush2=0;
   nPattern++;
   for(int itry=0;itry<2;itry++) {
      if(!m_patternStorage)
         m_patternStorage=new CodedPatternStorage(GetNLayers());
      map<int,PatternData_t>::iterator iSector=m_patternData.find(sector);
      if(iSector==m_patternData.end()) {
         iSector=m_patternData.insert
            (make_pair(sector,PatternData(GetNLayers(),m_patternStorage)))
            .first;
      }
      PatternData_t &sectorData=(*iSector).second;
      error=0;
      uint32_t iCodedPattern=m_patternStorage->AllocatePattern();
      if(!iCodedPattern) {
         // out of memory
         error=2;
         flush2++;
      }
      if(!error) {
         // encode pattern for each layer and store as 64-bit word
         for(int plane=GetNLayers()-1;plane>=0;plane--) {
            // locate SSID in encoding table
            int ssid=pattern.GetHit(plane);
            map<int,int>::iterator iSSID=
               sectorData.m_encoder[plane].second.find(ssid);
            if(iSSID==sectorData.m_encoder[plane].second.end()) {
               // not found, have to enlarge encoding table
               // determine new coded ID
               size_t id=sectorData.m_encoder[plane].second.size();
               if(id<sectorData.m_encoder[plane].first.size()) {
                  sectorData.m_encoder[plane].first[id]=ssid;
                  sectorData.m_encoder[plane].second[ssid]=id;
               } else {
                  // encoding table full, have to flush
                  error=1;
                  flush1++;
                  break;
               }
               iSSID=sectorData.m_encoder[plane].second.find(ssid);
            }
            m_patternStorage->Store(iCodedPattern,plane,(*iSSID).second);
         }
      }
      if(!error) {
         CPatternWithCoverage_t::iterator iData=
            sectorData.m_data.find(iCodedPattern);
         if(iData==sectorData.m_data.end()) {
            // insert new pattern
            sectorData.m_data[iCodedPattern]+=coverage;
         } else {
            // increase coverage
            (*iData).second+=coverage;
            m_patternStorage->Delete(iCodedPattern);
         }
      }
      if(error) {
         // flush buffers
         Flush();
      } else {
         break;
      }
   }
   /* static int loop=100;
   if(loop) {
      loop--;
      if(!loop) exit(0);
      } */
   return error;
}

void FTKPatternBySectorIndexedWriter::Flush(void) {
   Info("Flush")<<"Number of sectors: "<<m_patternData.size()<<"\n";
   m_dir.cd();
   if(!m_cpatternDataTree) {

      m_cpatternDataTree=new TTree(s_cdataTreeName,s_cdataTreeName);
      m_cnpattBranch=m_cpatternDataTree->Branch
         (s_cnpattBranchName,0,s_cnpattBranchID);
      for(int plane=0;plane<GetNLayers();plane++) {
         m_cpatternBranch.push_back(m_cpatternDataTree->Branch
                                    (GetCPatternBranchName(plane),0,GetCPatternBranchID(plane)));
      }

      m_cpatternIndexTree=new TTree(s_indexTreeName,s_indexTreeName);
      m_cpatternIndexTree->Branch(s_sectorBranchName,&m_treeSector,
                                  s_sectorBranchID);
      m_cpatternIndexTree->Branch(s_ncovBranchName,&m_treeNCov,
                                  s_ncovBranchID);
      m_npatternBranch=
         m_cpatternIndexTree->Branch(s_npatternBranchName,0,
                                    s_npatternBranchID);
      m_coverageBranch=
         m_cpatternIndexTree->Branch(s_coverageBranchName,0,
                                     s_coverageBranchID);
      m_decoderDataBranch.resize(GetNLayers());
      m_decoderSize.resize(GetNLayers());
      for(int i=0;i<GetNLayers();i++) {
         m_cpatternIndexTree->Branch
            (GetNDecoderBranchName(i),&m_decoderSize[i],
             GetNDecoderBranchID(i));
         m_decoderDataBranch[i]=m_cpatternIndexTree->Branch
            (GetDecoderDataBranchName(i),0,GetDecoderDataBranchID(i));
      }
   }

   // dump complete table from memory to disk

   for(map<int,PatternData_t>::iterator iSector=
          m_patternData.begin();iSector!=m_patternData.end();iSector++) {
      // dump table for one sector to disk
      PatternData_t const & patternData=(*iSector).second;
      m_treeSector=(*iSector).first;

      // order pattern data by coverage
      map<uint32_t,list<int32_t> > ordered;
      // ordered.first : coverage
      // ordered.second : set(patterns)
      for(CPatternWithCoverage_t::const_iterator i=patternData.m_data.begin();
          i!=patternData.m_data.end();i++) {
         ordered[(*i).second].push_back((*i).first);
      }
      //
      m_treeNCov=0;
      int n=ordered.size();
      m_treeCoverage.resize(n);
      m_treeNPattern.resize(n);
      // reverse loop over coverage
      for(map<uint32_t,list<int32_t> >::reverse_iterator iCov=ordered.rbegin();
          iCov!=ordered.rend();++iCov) {
         // save data
         m_treeCoverage[m_treeNCov]=(*iCov).first;
         uint32_t &npatt=m_treeNPattern[m_treeNCov];
         npatt=(*iCov).second.size();
         m_cnpattBranch->SetAddress(&npatt);
         vector<vector<uint8_t> > cpattern(GetNLayers());
         for(size_t plane=0;plane<cpattern.size();plane++) {
            cpattern[plane].resize(npatt);
            m_cpatternBranch[plane]->SetAddress(cpattern[plane].data());
         }
         npatt=0;
         for(list<int32_t>::const_iterator iPat=(*iCov).second.begin();
             iPat!=(*iCov).second.end();++iPat) {
            for(size_t plane=0;plane<cpattern.size();plane++) {
               cpattern[plane][npatt]=m_patternStorage->Get((*iPat),plane);
            }
            ++npatt;
         }
         m_cpatternDataTree->Fill();
         ++m_treeNCov;
      }
      // set branch addresses
      m_npatternBranch->SetAddress(m_treeNPattern.data());
      m_coverageBranch->SetAddress(m_treeCoverage.data());
      // save encoder
      for(int i=0;i<GetNLayers();i++) {
         m_decoderDataBranch[i]->SetAddress
            ((*iSector).second.m_encoder[i].first.data());
         m_decoderSize[i]=(*iSector).second.m_encoder[i].second.size();
      }
      m_cpatternIndexTree->Fill();

   }
   // clear memory
   m_patternData.clear();
   delete m_patternStorage;
   m_patternStorage=0;
}



//================== class FTKPatternBySectorIndexedReader ==================

int FTKPatternBySectorIndexedReader::InitializeIndex
(TTree *indexTree,set<int> const *sectorList) {
   int error=0;
   // number of decoders
   int nIndex=indexTree->GetEntries();
   Info("InitializeIndex")<<"Loading index, nIndex="<<nIndex<<"\n";
   // determine number of planes
   uint32_t nPlane=0;
   for(nPlane=0;indexTree->GetBranch(GetNDecoderBranchName(nPlane));++nPlane);
   SetNLayers(nPlane);
   m_patternData=new FTKPatternWithCoverage(nPlane);

   // one entry
   typedef struct DecoderWithIndex {
      // size of decoders per plane
      uint32_t *m_decoderSize;
      // number of Indexes
      uint32_t m_ncov;
      // sector number
      uint32_t m_sectorID;
   } DecoderWithIndex_t;
   // full table, read from TTree
   vector<DecoderWithIndex_t> decoderWithIndexTable(nIndex);
   // set up pointers to decoder per plane
   vector<uint32_t> decoderSizeData(nIndex*nPlane);
   uint32_t *decoderSizePtr=decoderSizeData.data();
   for(int iIndex=0;iIndex<nIndex;++iIndex) {
      decoderWithIndexTable[iIndex].m_decoderSize=decoderSizePtr;
      decoderSizePtr += nPlane;
   }
   // read sector structure, decoder sizes and ncov
   vector<uint32_t> ndecoder(nPlane);
   uint32_t sector=666,ncov=666666;
   for(uint32_t plane=0;plane<nPlane;++plane) {
      indexTree->SetBranchAddress(GetNDecoderBranchName(plane),
                                  &ndecoder[plane]);
   }
   indexTree->SetBranchAddress(s_sectorBranchName,&sector);
   indexTree->SetBranchAddress(s_ncovBranchName,&ncov);

   // loop over the tree to find ncov,sector,ndecoder%d
   // store result in local variables 
   for(int iIndex=0;iIndex<nIndex;iIndex++) {
      indexTree->GetEntry(iIndex);
      DecoderWithIndex_t &decoderWithIndex=decoderWithIndexTable[iIndex];
      decoderWithIndex.m_sectorID=sector;
      decoderWithIndex.m_ncov=ncov;
      for(uint32_t plane=0;plane<nPlane;++plane) {
         decoderWithIndex.m_decoderSize[plane]=ndecoder[plane];
      }
   }
   uint32_t maxNCov=0;
   uint32_t decoderSize=0;
   // determine total size of decoder table
   for(int iIndex=0;iIndex<nIndex;iIndex++) {
      DecoderWithIndex_t const &decoderWithIndex=
         decoderWithIndexTable[iIndex];
      for(uint32_t plane=0;plane<nPlane;++plane) {
         decoderSize += decoderWithIndex.m_decoderSize[plane];
      }
      if(decoderWithIndex.m_ncov>maxNCov) maxNCov=decoderWithIndex.m_ncov;
   }
   Info("InitializeIndex")
      <<"decoder size="<<decoderSize<<" maxNCov="<<maxNCov<<" \n";
   // allocate decoder tables
   m_decoderData.resize(decoderSize);
   m_decoderDataPtr.resize(nIndex*nPlane+1);

   vector<uint32_t> bufferCoverage(maxNCov);
   vector<uint32_t> bufferNPattern(maxNCov);
   // read index and decoder data and build up sector map
   indexTree->SetBranchAddress(s_npatternBranchName,bufferNPattern.data());
   indexTree->SetBranchAddress(s_coverageBranchName,bufferCoverage.data());

   uint32_t lastSector=0;
   SetContentType(CONTENT_MERGED);
   uint32_t cpatternEntry=0;
   Info("InitializeIndex")<<"read decoder data and data pointers\n";
   decoderSize=0;
   // loop over all entries in the index ttree 
   for(int iIndex=0;iIndex<nIndex;iIndex++) {
      DecoderWithIndex_t const &decoderWithIndex=
         decoderWithIndexTable[iIndex];
      for(uint32_t plane=0;plane<nPlane;++plane) {
         m_decoderDataPtr[iIndex*nPlane+plane]=&m_decoderData[decoderSize];
         indexTree->SetBranchAddress(GetDecoderDataBranchName(plane),
                                     m_decoderDataPtr[iIndex*nPlane+plane]);
         decoderSize+= decoderWithIndex.m_decoderSize[plane];
      }
      m_decoderDataPtr[(iIndex+1)*nPlane]=&m_decoderData[decoderSize];
      indexTree->GetEntry(iIndex);

      uint32_t sector=decoderWithIndex.m_sectorID;
      SectorInfo_t &sectorData=m_sectorTable[sector];
      if(!sectorData.m_coverageTable.size()) {
         sectorData.m_nPattern=0;
      } else if(lastSector!=sector) {
         SetContentType(CONTENT_NOTMERGED);
      }
      // loop over coverage,npattern
      for(uint32_t icov=0;icov<decoderWithIndex.m_ncov;++icov) {
         DataBlock_t indexData;
         indexData.m_decoderDataPtr=&m_decoderDataPtr[iIndex*nPlane];
         indexData.m_cpatternEntry=cpatternEntry;
         indexData.m_nPattern=bufferNPattern[icov];
         ++cpatternEntry;
         // skip sectors which are not in the sector list
         if((!sectorList)||(sectorList->find(sector)!=sectorList->end())) {
            sectorData.m_coverageTable.insert
               (make_pair(bufferCoverage[icov],indexData));            
            sectorData.m_coveragePtr=sectorData.m_coverageTable.rbegin();
            sectorData.m_nPattern+=indexData.m_nPattern;
         }
      }
      lastSector=sector;
   }
   if(m_sectorTable.empty()) {
      SetContentType(CONTENT_EMPTY);
      error=1;
   }
   // if merged, set up coverage table
   uint64_t nPattern=0;
   for(SectorTable_t::const_iterator
          iSector=m_sectorTable.begin();
       iSector!=m_sectorTable.end();iSector++) {
      CoverageTable_t const &ctable=(*iSector).second.m_coverageTable;
      for(CoverageTable_t::const_iterator iCov=ctable.begin();
          iCov!=ctable.end();iCov++) {
         m_SectorByCoverage[(*iCov).first][(*iSector).first]+=
            (*iCov).second.m_nPattern;
         nPattern+=(*iCov).second.m_nPattern;
      }
   }
   Info("InitializeIndex")
      <<"nSector="<<m_sectorTable.size()
      <<" nPattern="<<nPattern
      <<" nCoverage="<<m_SectorByCoverage.size()<<"\n";
   return error;
}

bool FTKPatternBySectorIndexedReader::CheckConsistency
(FTKSSMap *ssMap,int tower,int hwmodeid) 
   const {
   // loop over all sectors
   int nPlane=GetNLayers();
   int error=0;
   std::set<int> badSector;
   for(SectorTable_t::const_iterator iSector=m_sectorTable.begin();
       iSector!=m_sectorTable.end();iSector++) {
      uint32_t sectorID=(*iSector).first;
      SectorInfo const &sectorInfo((*iSector).second);
      std::vector<std::set<int> > moduleSet(nPlane);
      for(CoverageTable_t::const_iterator
             iCov=sectorInfo.m_coverageTable.begin();
          iCov!=sectorInfo.m_coverageTable.end();iCov++) {
         DataBlock_t const &dataBlock((*iCov).second);
         for(int plane=0;plane<nPlane;plane++) {
            uint32_t const *decoderData=dataBlock.m_decoderDataPtr[plane];
            int decoderSize=dataBlock.m_decoderDataPtr[plane+1]-decoderData;
            bool isPixel=ssMap->getPlaneMap()->isPixel(plane);
            for(int iSSID=0;iSSID<decoderSize;iSSID++) {
               int ssid=decoderData[iSSID];
               int phimod=-1,etacode=-1,section=0,moduleID=-1;
               int localX=0,localY=0;
               if(hwmodeid==0) {
                  if(isPixel) {
                     ssMap->decodeSSxy
                        (ssid,plane,section,phimod,localX,etacode,localY);
                     moduleID=ssMap->getSSxy
                        (plane,section,phimod,etacode,0,0);
                  } else {
                     ssMap->decodeSSx
                        (ssid,plane,section,phimod,localX,etacode);
                     moduleID=ssMap->getSSx(plane,section,phimod,etacode,0);
                  }
               } else if(hwmodeid==2) {
                  if (isPixel) {
                     ssMap->decodeSSTowerXY(ssid,tower,plane,section,
                                            moduleID,localX,localY);
                  } else {
                     ssMap->decodeSSTowerX(ssid,tower,plane,section,
                                           moduleID,localX);
                  }
               }
               moduleSet[plane].insert(moduleID);
            }
         }
      }
      bool isBad=false;
      for(int plane=0;plane<nPlane;plane++) {
         if(moduleSet[plane].size()>1) {
            isBad=true;
            if(error<100) {
               std::cout<<" sector="<<sectorID<<" plane="<<plane<<" [";
               for(std::set<int>::const_iterator id=moduleSet[plane].begin();
                   id!=moduleSet[plane].end();id++) {
                  std::cout<<" "<< *id;
               }
               std::cout<<"]\n";
            }
            error++;
         }
      }
      if(isBad) badSector.insert(sectorID);
   }
   if(error) {
      std::cout<<"List of bad sectors:\n";
      std::map<int,int> sectorList;
      int sequenceStart=-2;
      int previous=-2;
      for(std::set<int>::const_iterator i=badSector.begin();
          i!=badSector.end();i++) {
         int sector=(*i);
         if(sector==previous+1) {
            previous++;
         } else {
            sequenceStart=(*i);
         }
         previous=(*i);
         sectorList[sequenceStart]++;
      }
      for(std::map<int,int>::const_iterator i=sectorList.begin();
          i!=sectorList.end();i++) {
         std::cout<<" "<<(*i).first;
         if((*i).second>1) {
            std::cout<<"-"<<(*i).first+(*i).second-1;
         }
      }
      std::cout<<"\n";
   }
   if(error) {
      Fatal("CheckConsistency")
         <<"input patterns corrupted: several modules are assigned to a given (sector,plane) pair\n";
   } else {
      Info("CheckConsistency")<<"input patterns seem consistent\n";
   }
   return error==0;
}

FTKPatternBySectorIndexedReader::FTKPatternBySectorIndexedReader
(TDirectory &dir,set<int> const *sectorList,int *error)
   : FTKPatternBySectorReader("FTKPatternBySectorIndexedReader") {
   TTree *indexTree;
   dir.GetObject(s_indexTreeName,indexTree);
   m_dataTree=0;
   m_patternData=0;
   if(indexTree && !InitializeIndex(indexTree,sectorList)) {
      dir.GetObject(s_cdataTreeName,m_dataTree);
   }
   if(indexTree) {
      delete indexTree;
   }
   if(!m_dataTree) {
      if(error) {
         *error=1;
      } else {
         Error("FTKPatternBySectorIndexedReader")<<"not implemented\n";
      }
   }
}

FTKPatternBySectorIndexedReader::FTKPatternBySectorIndexedReader
(FTKRootFileChain &chain,int *error)
   : FTKPatternBySectorReader("FTKPatternBySectorIndexedReader") {
   TChain indexChain(s_indexTreeName);
   int errorCount=0;
   for(int i=0;i<chain.GetLength();i++) {
      if(!indexChain.Add(chain[i].c_str(),0)) ++errorCount;
   }
   m_dataTree=0;
   m_patternData=0;
   if((!errorCount) && !InitializeIndex(&indexChain,0)) {
      TChain *dataChain=new TChain(s_cdataTreeName);
      for(int i=0;i<chain.GetLength();i++) {
         if(!dataChain->Add(chain[i].c_str(),0)) ++errorCount;
      }
      if(!errorCount) {
         m_dataTree=dataChain;
      } else {
         delete dataChain;
      }
   }
   if(!m_dataTree) {
      if(error) {
         *error=1;
      } else {
         Error("FTKPatternBySectorIndexedReader")<<"pattern data not found\n";
      }
   }
}

FTKPatternBySectorIndexedReader::~FTKPatternBySectorIndexedReader() {
   if(m_dataTree) delete m_dataTree;
   if (m_patternData) delete m_patternData;
}

Long64_t FTKPatternBySectorIndexedReader::GetNPatterns(int sector) const {
   Long64_t n=0;
   SectorTable_t::const_iterator i=
      m_sectorTable.find(sector);
   if(i!=m_sectorTable.end()) {
      n=(*i).second.m_nPattern;
   }
   return n;
}

int FTKPatternBySectorIndexedReader::GetFirstSector(void) const {
   int sector=-1;
   if(m_sectorTable.begin()!=m_sectorTable.end()) {
      sector=(*m_sectorTable.begin()).first;
   }
   return sector;
}

int FTKPatternBySectorIndexedReader::GetNextSector(int sector) const {
   SectorTable_t::const_iterator i=
      m_sectorTable.upper_bound(sector);
   int r=-1;
   if(i!=m_sectorTable.end()) {
      r=(*i).first;
   }
   return r;
}

void FTKPatternBySectorIndexedReader::RewindSector(int sector) {
   SectorTable_t::iterator iSector=
      m_sectorTable.find(sector);
   if(iSector!=m_sectorTable.end()) {
      SectorInfo_t &info=(*iSector).second;
      info.m_coveragePtr=info.m_coverageTable.rbegin();
   }
}

FTKPatternOneSector *FTKPatternBySectorIndexedReader::Read
(int sector,int minCoverage) {
   FTKPatternOneSector *r=new FTKPatternOneSector(sector);
   SectorTable_t::iterator iSector=
      m_sectorTable.find(sector);
   int nread=0;
   if(iSector!=m_sectorTable.end()) {
      SectorInfo_t &info=(*iSector).second;
      uint32_t cnpatt,cnpattMax=0;
      m_dataTree->SetBranchAddress(s_cnpattBranchName,&cnpatt);
      for(CoverageTable_t::const_reverse_iterator icov=info.m_coveragePtr;
          icov!=info.m_coverageTable.rend();++icov) {
         cnpatt=(*icov).second.m_nPattern;
         if(cnpattMax<cnpatt) cnpattMax=cnpatt;
      }
      vector<vector<uint8_t> > patternData(GetNLayers());
      for(size_t plane=0;plane<patternData.size();plane++) {
         patternData[plane].resize(cnpattMax);
         m_dataTree->SetBranchAddress(GetCPatternBranchName(plane),
                                      patternData[plane].data());
      }
      while(info.m_coveragePtr !=info.m_coverageTable.rend()) {
         int coverage=(*info.m_coveragePtr).first;
         if(coverage<minCoverage) break;
         // read all patterns
         DataBlock_t const &block=(*info.m_coveragePtr).second;
         uint32_t entry=block.m_cpatternEntry;
         m_dataTree->GetEntry(entry);
         // loop over patterns
         for(uint32_t ipattern=0;ipattern<cnpatt;++ipattern) {
            // decode SSIDs
            for(int plane=0;plane<GetNLayers();plane++) {
               m_patternData->SetHit
                  (plane,block.m_decoderDataPtr[plane]
                   [patternData[plane][ipattern]]); 
            }
            m_patternData->SetCoverage(coverage);
            r->AddPattern(*m_patternData);
            nread++;
         }
         info.m_coveragePtr++;
      }
   }
   return r;
}

int FTKPatternBySectorIndexedReader::ReadRaw
(int firstSector,std::list<FTKPatternOneSector *> &sectorList,
 uint64_t maxPattern) {
   // define a work-plan
   uint64_t patternEstimate=0;
   int readSector;
   int nSector=0;
   uint32_t cnpattMax=0;
   map<uint32_t,pair<int,CoverageTable_t::const_reverse_iterator> > IOordered;
   for(readSector=firstSector;readSector>=0;
       readSector=GetNextSector(readSector)) {
      SectorTable_t::iterator iSector=m_sectorTable.find(readSector);
      if(iSector!=m_sectorTable.end()) {
         SectorInfo_t &info=(*iSector).second;
         while(info.m_coveragePtr !=info.m_coverageTable.rend()) {
            DataBlock_t const &block=(*info.m_coveragePtr).second;
            IOordered[block.m_cpatternEntry]=
               make_pair(readSector,info.m_coveragePtr);
            info.m_coveragePtr++;
            patternEstimate += block.m_nPattern;
            if(cnpattMax<block.m_nPattern) cnpattMax=block.m_nPattern;
         }
         nSector++;
         if(patternEstimate>maxPattern) {
            readSector=GetNextSector(readSector);
            break;
         }
      }
   }
   Info("ReadRaw")
      <<"reading "<<IOordered.size()<<" chunks "<<nSector<<" sectors\n";

   uint32_t cnpatt;
   m_dataTree->SetBranchAddress(s_cnpattBranchName,&cnpatt);
   vector<vector<uint8_t> > patternData(GetNLayers());
   for(size_t plane=0;plane<patternData.size();plane++) {
      patternData[plane].resize(cnpattMax);
      m_dataTree->SetBranchAddress(GetCPatternBranchName(plane),
                                   patternData[plane].data());
   }
   map<int, FTKPatternOneSector *> sectorMap;
   for(map<uint32_t,pair<int,CoverageTable_t::const_reverse_iterator> >
          ::const_iterator iChunk=IOordered.begin();iChunk!=IOordered.end();
       iChunk++) {
      int sector=(*iChunk).second.first;
      FTKPatternOneSector *&sectorData=sectorMap[sector];
      if(!sectorData) sectorData=new FTKPatternOneSector(sector);
      CoverageTable_t::const_reverse_iterator iCT=(*iChunk).second.second;
      DataBlock_t const &block=(*iCT).second;
      uint32_t entry=block.m_cpatternEntry;
      m_dataTree->GetEntry(entry);
      // loop over patterns
      for(uint32_t ipattern=0;ipattern<cnpatt;++ipattern) {
         // decode SSIDs
         for(int plane=0;plane<GetNLayers();plane++) {
            m_patternData->SetHit
               (plane,block.m_decoderDataPtr[plane]
                [patternData[plane][ipattern]]); 
         }
         m_patternData->SetCoverage((*iCT).first);
         sectorData->AddPattern(*m_patternData);
      }
   }
   for(map<int,FTKPatternOneSector *>::const_iterator i=sectorMap.begin();
       i!=sectorMap.end();i++) {
      sectorList.push_back((*i).second);
   }
   return readSector;
}

