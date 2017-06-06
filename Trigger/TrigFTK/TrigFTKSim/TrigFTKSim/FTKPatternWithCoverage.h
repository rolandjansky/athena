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
   FTKHitPattern(int nLayer) : m_hits(nLayer) { }
   inline int GetHit(int i) const { return m_hits[i]; }
   inline void SetHit(int layer, int data) { m_hits[layer]=data; }
   inline int *GetAddress(void) { return &m_hits[0]; }
   inline unsigned GetSize(void) const { return m_hits.size(); }
   inline void SetNLayer(int i) {m_hits.resize(i);}
   inline unsigned int GetNLayer() {return m_hits.size();}
protected:
   std::vector<int> m_hits;
};

class FTKPatternWithCoverage {
public:
   // store hit patterns and coverage
   FTKPatternWithCoverage() { ;}
   FTKPatternWithCoverage(int nLayer) : m_pattern(nLayer) { }
   inline int GetCoverage(void) const { return m_coverage; }
   inline FTKHitPattern const &GetHitPattern(void) const { return m_pattern; }
   inline FTKHitPattern &GetHitPattern(void) { return m_pattern; }
   inline void AddToCoverage(int c) { m_coverage += c; }
   inline int* GetCoverageAddress(void) { return &m_coverage; }
   inline int* GetHitPatternAddress(void) { return m_pattern.GetAddress(); }
   inline void SetCoverage(int c) { m_coverage=c; }
   inline void SetHitPattern(FTKHitPattern const &p) { m_pattern=p; }
   inline void SetHit(int layer,int data) { m_pattern.SetHit(layer,data); }
   inline void SetNLayer(int i) {m_pattern.SetNLayer(i);}
   inline unsigned int GetNLayer() {return m_pattern.GetNLayer();}
protected:
   FTKHitPattern m_pattern;
   int m_coverage;
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
   inline Long64_t GetNumberOfPatternsRead(void) const { return m_numReads; }
   inline FTKPatternWithCoverage const &GetPattern(void) const {
      return *m_pattern; }
   static int ExtractSectorNumber(char const *treeName);
   static TString ConstructTreeName(int sector);
   TString const &GetTreeName(void) const { return m_treeName; }
   void ReadCoverageOnly(bool suppressPatternRead); 
   int GetSectorNumber(void) const { return m_sector; }
protected:
   bool LoadTree(TDirectory *dir);
   bool CreateTree(TDirectory *dir,int nLayer);
   void SetPattern(FTKPatternWithCoverage const &p) { *m_pattern=p; }
   void SetPattern(int coverage,FTKHitPattern const &hits) {
      m_pattern->SetCoverage(coverage);
      m_pattern->SetHitPattern(hits);
   }
   Long64_t m_patternNumber;
   Long64_t m_numReads;
   Long64_t m_lastNumPatterns;
   TTree *m_TTreePtr;
   TDirectory *m_lastReadDir;
private:
   bool m_doReadPattern;
   FTKPatternWithCoverage *m_pattern;
   int m_sector;
   TString m_treeName;
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
   inline Long64_t GetNumberOfPatternsWritten(void) const { return m_numWrites; }
   bool WritePattern(FTKPatternWithCoverage const &pattern);
   bool WritePattern(FTKHitPattern const &hits,int coverage);
protected:
   bool m_mustSave;
   Long64_t m_numWrites;
};

class FTKPatternRootChainReader : public FTKPatternRootTreeReader {
public:
   FTKPatternRootChainReader(FTKRootFileChain &chain,int sector)
      : FTKPatternRootTreeReader(sector,"FTKPatternRootChainReader"),
      m_chain(chain) { m_currentFile=0; }
   void Add(char const *name,Long64_t nPattern);
   virtual Long64_t GetNPatterns(void) const;
   virtual bool ReadNextPattern(void);
protected:
   FTKRootFileChain &m_chain;
   std::vector<std::string> m_fileNames;
   std::vector<Long64_t> m_numEntries;
   std::vector<Long64_t> m_numEntriesSummed;
   int m_currentFile;
};

#endif
