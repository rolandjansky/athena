/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef H_FTKPatternWithCoverageCompare
#define H_FTKPatternWithCoverageCompare

#include "FTKLogging.h"

/* package FTKPatternWithCoverage
 *
 * Store a hit pattern with coverage and
 * provide classes to be used for comparisons with STL containers
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 * classes:
 *     FTKHitPattern
 *        store hit pattern (a vector of ints)
 *     FTKPatternWithCoverage
 *        hit pattern with coverage
 *     FTKHitPatternCompare
 *        ordering rule for hit patterns
 *     FTKPatternRootTreeReader
 *        base blass for reading instances of FTKPatternWithCoverage
 *        from root files
 *     FTKPatternRootTree
 *        read from and write to a root file
 *     FTKRootFileChain
 *        sequence of several root files which are treated as a single
 *        source of patterns
 *     FTKPatternRootChainReader
 *        reader to obtain patterns rom a FTKRootFileChain for a single sector
 */

#include <string>
#include <vector>
#include <map>
#include <TString.h>

class TDirectory;
class TTree;

class FTKRootFileChain;

class FTKHitPattern {
public:
   // store hit patterns
   FTKHitPattern() { }
   FTKHitPattern(int nLayer) : fHits(nLayer) { }
   inline int GetHit(int i) const { return fHits[i]; }
   inline void SetHit(int layer, int data) { fHits[layer]=data; }
   inline int *GetAddress(void) { return &fHits[0]; }
   inline unsigned GetSize(void) const { return fHits.size(); }
   inline void SetNLayer(int i) {fHits.resize(i);}
   inline unsigned int GetNLayer() {return fHits.size();}
protected:
   std::vector<int> fHits;
};

class FTKPatternWithCoverage {
public:
   // store hit patterns and coverage
   FTKPatternWithCoverage() { ;}
   FTKPatternWithCoverage(int nLayer) : fPattern(nLayer) { }
   inline int GetCoverage(void) const { return fCoverage; }
   inline FTKHitPattern const &GetHitPattern(void) const { return fPattern; }
   inline FTKHitPattern &GetHitPattern(void) { return fPattern; }
   inline void AddToCoverage(int c) { fCoverage += c; }
   inline int* GetCoverageAddress(void) { return &fCoverage; }
   inline int* GetHitPatternAddress(void) { return fPattern.GetAddress(); }
   inline void SetCoverage(int c) { fCoverage=c; }
   inline void SetHitPattern(FTKHitPattern const &p) { fPattern=p; }
   inline void SetHit(int layer,int data) { fPattern.SetHit(layer,data); }
   inline void SetNLayer(int i) {fPattern.SetNLayer(i);}
   inline unsigned int GetNLayer() {return fPattern.GetNLayer();}
protected:
   FTKHitPattern fPattern;
   int fCoverage;
};

class FTKPatternWithSector : public FTKPatternWithCoverage {
public:
   FTKPatternWithSector(int nLayer) : FTKPatternWithCoverage(nLayer) {}
   inline int GetSectorID() const {return m_sectorID;}
   inline void SetSectorID(int sID) {m_sectorID=sID;}
protected:
   int m_sectorID;
};

#define INCREASING_ORDER

class FTKHitPatternCompare {
public:
   // compare two hit patterns, for storing in STL containers
   virtual bool operator()(FTKHitPattern const &a,FTKHitPattern const &b)
      const {
      unsigned n=a.GetSize();
      if(n>b.GetSize()) n=b.GetSize();
#ifdef INCREASING_ORDER
      for(unsigned i=0;i<n;i++) {
         if(a.GetHit(i)<b.GetHit(i)) return true;
         if(a.GetHit(i)>b.GetHit(i)) return false;
      }
      if(a.GetSize()<b.GetSize()) return true;
#else
      for(unsigned i=0;i<n;i++) {
         if(a.GetHit(i)>b.GetHit(i)) return true;
         if(a.GetHit(i)<b.GetHit(i)) return false;
      }
      if(a.GetSize()>b.GetSize()) return true;
#endif
      return false; 
   }
   virtual bool operator()(FTKPatternWithCoverage const &a,FTKPatternWithCoverage const &b) const {
      return operator()(a.GetHitPattern(),b.GetHitPattern());
   }
};

class FTKPatternRootTreeReader : public FTKLogging {
public:
   FTKPatternRootTreeReader(int sector,const char *name);
   virtual ~FTKPatternRootTreeReader();
   virtual Long64_t GetNPatterns(void) const =0;
   virtual Long64_t SeekBeg(Long64_t position);
   virtual bool ReadNextPattern(void)=0;
   virtual bool HasMorePatterns(void) const;
   virtual void Suspend(void);
   void Rewind(void); // can be used to reduce memory consuption (?)
   inline Long64_t GetNumberOfPatternsRead(void) const { return fNumReads; }
   inline FTKPatternWithCoverage const &GetPattern(void) const {
      return *fPattern; }
   static int ExtractSectorNumber(char const *treeName);
   static TString ConstructTreeName(int sector);
   TString const &GetTreeName(void) const { return fTreeName; }
   void ReadCoverageOnly(bool suppressPatternRead); 
   int GetSectorNumber(void) const { return fSector; }
protected:
   bool LoadTree(TDirectory *dir);
   bool CreateTree(TDirectory *dir,int nLayer);
   void SetPattern(FTKPatternWithCoverage const &p) { *fPattern=p; }
   void SetPattern(int coverage,FTKHitPattern const &hits) {
      fPattern->SetCoverage(coverage);
      fPattern->SetHitPattern(hits);
   }
   Long64_t fPatternNumber;
   Long64_t fNumReads;
   Long64_t fLastNumPatterns;
   TTree *fTTreePtr;
   TDirectory *fLastReadDir;
private:
   bool fDoReadPattern;
   FTKPatternWithCoverage *fPattern;
   int fSector;
   TString fTreeName;
};

class FTKPatternRootTree : public FTKPatternRootTreeReader {
   // helper class to store root tree and data structures for a single sector
public:
   // give nLayer=0 for loading an existing tree
   // give nLayer>0 for creating a new tree
   FTKPatternRootTree(TDirectory &dir,int sector,int nLayer);
   virtual ~FTKPatternRootTree();
   virtual Long64_t GetNPatterns(void) const;
   virtual bool ReadNextPattern(void);
   inline Long64_t GetNumberOfPatternsWritten(void) const { return fNumWrites; }
   bool WritePattern(FTKPatternWithCoverage const &pattern);
   bool WritePattern(FTKHitPattern const &hits,int coverage);
protected:
   bool fMustSave;
   Long64_t fNumWrites;
};

class FTKPatternRootChainReader : public FTKPatternRootTreeReader {
public:
   FTKPatternRootChainReader(FTKRootFileChain &chain,int sector)
      : FTKPatternRootTreeReader(sector,"FTKPatternRootChainReader"),
      fChain(chain) { fCurrentFile=0; }
   void Add(char const *name,Long64_t nPattern);
   virtual Long64_t GetNPatterns(void) const;
   virtual bool ReadNextPattern(void);
protected:
   FTKRootFileChain &fChain;
   std::vector<std::string> fFileNames;
   std::vector<Long64_t> fNumEntries;
   std::vector<Long64_t> fNumEntriesSummed;
   int fCurrentFile;
};

#endif
