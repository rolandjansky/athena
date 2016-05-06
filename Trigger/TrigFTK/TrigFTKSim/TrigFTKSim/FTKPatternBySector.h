/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef H_FTKPatternBySector
#define H_FTKPatternBySector

/* package FTKPatternBySector
 *
 * IO interface for reading and writing patterns to files
 * For reading patterns, chains of root-files are supported
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 * classes:
 *   FTKPatternBySectorBase 
 *     base class for sector-and-coverage ordered patterns root IO
 *   FTKPatternBySectorReader
 *     class for reading
 *   FTKPatternBySectorWriter
 *     class for writing
 *
 */

#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <TTree.h>
#include "FTKLogging.h"
#include "FTKPatternOneSector.h"

class TDirectory;

class FTKPatternBySectorBase : public FTKLogging {
public:
   FTKPatternBySectorBase(char const *name="FTKPatternBySectorBase")
      : FTKLogging(name),fNLayers(0),fContentType(CONTENT_EMPTY) { }
   virtual ~FTKPatternBySectorBase();
   inline int GetNLayers(void) const { return fNLayers; } // get number of layers
   int SetNLayers(int nLayer); // set number of layers
   enum CONTENT_TYPE {
      CONTENT_EMPTY=0, // no data inside
      CONTENT_MERGED=1, // data is merged
      CONTENT_NOTMERGED=2 // data is not merged
   };
   inline CONTENT_TYPE GetContentType(void) const { return fContentType; }
   inline void SetContentType(CONTENT_TYPE type) { fContentType=type; }
   inline TString const &GetSourceName(void) const { return fSourceName; }
protected:
   int fNLayers;
   TString fSourceName;
   CONTENT_TYPE fContentType;
};

class FTKPatternBySectorReader : public FTKPatternBySectorBase {
public:
   // typedefs and comparison classes
   typedef std::map<int,FTKPatternRootTreeReader *> PatternTreeBySector_t;
   class FTKPatternTreeBySectorCompare {
   public:
      // helper class to compare patterns from different sectors
      // and order them by coverage then sector number
      explicit FTKPatternTreeBySectorCompare
      (FTKHitPatternCompare hitComp=FTKHitPatternCompare())
         : fHitCompare(hitComp) { }
      bool operator()
         (PatternTreeBySector_t::iterator const &a,
          PatternTreeBySector_t::iterator const &b) const {
         FTKPatternRootTreeReader const *ra=(*a).second;
         FTKPatternRootTreeReader const *rb=(*b).second;
         int ca=ra->GetPattern().GetCoverage();
         int cb=rb->GetPattern().GetCoverage();
         // compare coverages
         if(ca>cb) return true;
         if(ca<cb) return false;
         // compare hits
         //return fHitCompare(ra->GetPattern().GetHitPattern(),
         //  rb->GetPattern().GetHitPattern()); //) return true;
         // compare sector
         return (*a).first<(*b).first;
      }
   protected:
      FTKHitPatternCompare fHitCompare;
   };
   typedef std::set<PatternTreeBySector_t::iterator,FTKPatternTreeBySectorCompare> SectorSet_t;
   
public:
   //explicit FTKPatternBySectorReader(TDirectory &dir,
   //                                  char const *name="FTKPatternBySector",
   //                                 int nSub=1,int iSub=0); // read pattern bank
   explicit FTKPatternBySectorReader(TDirectory &dir,
                                     char const *name="FTKPatternBySector",
                                     std::set<int> const *sectorList=0); // read pattern bank
   explicit FTKPatternBySectorReader(FTKRootFileChain &chain); // read pattern bank
   ~FTKPatternBySectorReader();

   void SelectSubregion(int nSub,int iSub);

   Long64_t GetNPatterns(void) const;
   Long64_t GetNPatterns(int sector) const;
   void GetNPatternsByCoverage(std::map<int,int> &coverageMap) const;
   void GetNPatternsBySectorCoverage(std::map<int,std::map<int,int> >&sectorCoverageMap) const;

   int WritePatternsToASCIIstream(std::ostream &out,
                                  int iSub,int nSub); // export as ASCII
   // save data from one sector, ordered by coverage
   FTKPatternOneSector *MergePatterns(int sector) const; // get data from one sector   

   // the following methods are for looping over the data of one sector
   int GetFirstSector(void) const;
   int GetNextSector(int sector) const;
   Long64_t GetPatternByCoverage(SectorSet_t& sectorByCoverage, int iSub, int nSub) ;

protected:
   PatternTreeBySector_t fPatterns;

};

class FTKPatternBySectorBlockReader : public FTKPatternBySectorReader {
 public:
   FTKPatternBySectorBlockReader
      (TDirectory &dir,std::set<int> const *sectorList);
   void Rewind(void);
   FTKPatternOneSector *Read(int sector,int minCoverage); 
};

class FTKPatternBySectorWriter : public FTKPatternBySectorBase {
public:
   explicit FTKPatternBySectorWriter(TDirectory &dir); // read or create pattern bank
   virtual ~FTKPatternBySectorWriter(); // save patterns to TDirectory
   int AppendMergedPatterns(FTKPatternBySectorReader &in,int minCoverage); // append patterns
   int AppendPatternsFromASCIIstream(std::istream &in); // append
   int AppendMergedPatterns(int sector,
                            FTKPatternOneSectorOrdered const *ordered);
   int AppendPattern(int sector,FTKPatternWithCoverage const &pattern);
   int AppendPattern(FTKPatternWithSector const &pattern) { return AppendPattern(pattern.GetSectorID(),pattern);} 
   TDirectory& GetTDirectoryAddress() { return fDir;}
   
protected:
   TDirectory &fDir;
   typedef std::map<int,FTKPatternRootTree *> PatternTreeBySector_t;
   PatternTreeBySector_t fPatterns;
};

#endif
