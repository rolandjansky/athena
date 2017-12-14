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
 * Two basic formats are supported:
 *   "Forest"  there is one TTree per sector -> O(10000) TTrees
 *               -> excessive buffer memory consumption in root
 *   "Indexed" there is one TTree with data and two auxillary TTrees
 *
 * Abstract interface classes:
 *   FTKPatternBySectorReaderBase  basic methods for reading patterns
 *   FTKPatternBySectorWriterBase  basic methods for writing patterns
 *     use the Factory() methods to create the appropriate
 *      reader/writer implementation
 *
 * Implementation:
 *   FTKPatternBySectorForestReader   for reading the "Forest" format
 *   FTKPatternBySectorIndexedReader  for reading the "Indexed" format
 *
 *   FTKPatternBySectorForestWriter   for writing the "Forest" format
 *   FTKPatternBySectorIndexedWriter  for writing the "Indexed" format
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 * Major modification: 2016/11/18
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
class FTKSSMap;

class FTKPatternBySectorBase : public FTKLogging {
public:
   FTKPatternBySectorBase(char const *name="FTKPatternBySectorBase")
      : FTKLogging(name),m_NLayers(0),m_contentType(CONTENT_EMPTY) { }
   virtual ~FTKPatternBySectorBase();
   inline int GetNLayers(void) const { return m_NLayers; } // get number of layers
   int SetNLayers(int nLayer); // set number of layers
   enum CONTENT_TYPE {
      CONTENT_EMPTY=0, // no data inside
      CONTENT_MERGED=1, // data is merged
      CONTENT_NOTMERGED=2 // data is not merged
   };
   inline CONTENT_TYPE GetContentType(void) const { return m_contentType; }
   inline void SetContentType(CONTENT_TYPE type) { m_contentType=type; }
   inline TString const &GetSourceName(void) const { return m_sourceName; }
protected:
   int m_NLayers;
   TString m_sourceName;
   CONTENT_TYPE m_contentType;
};

class FTKPatternBySectorReader : public virtual FTKPatternBySectorBase {
 public:
   static FTKPatternBySectorReader *Factory
      (TDirectory &dir,std::set<int> const *sectorList=0);
   static FTKPatternBySectorReader *Factory(FTKRootFileChain &chain);

   virtual ~FTKPatternBySectorReader();

   virtual Long64_t GetNPatternsTotal(void) const;

   // read patterns (optimized IO), given a minimum number of patterns
   // repeated calls to this function will result in all sectors to be read
   // for sectors appended to the list, all patterns have been read
   // (use Rewind() to start all over)
   //   returns: next sector to read (or -1)
   virtual int ReadRaw(int firstSector,
                       std::list<FTKPatternOneSector *> &sectorList,
                       uint64_t maxPattern);

   // for the given sector, read all patterns down to minCoverage
   // minCoverage<=1: read all data (also works for non-merged banks)
   virtual FTKPatternOneSector *Read(int sector,int minCoverage)=0; 

   // read all patterns of a sector, also works for non-merged banks
   FTKPatternOneSector *MergePatterns(int sector) {
      RewindSector(sector); return Read(sector,0);
   }

   // rewind one/all sectors
   void Rewind(void);
   virtual void RewindSector(int sector)=0;
   // determine multiplicity of each coverage
   void GetNPatternsByCoverage(std::map<int,int> &coverageMap) const;
   // for each coverage, list sectors and number of patterns
   typedef std::map<int,int> SectorMultiplicity_t;
   typedef std::map<int,SectorMultiplicity_t> SectorByCoverage_t;
   virtual SectorByCoverage_t const &GetSectorByCoverage(void) const {
      return m_SectorByCoverage;
   }
   // export patterns as ASCII
   int WritePatternsToASCIIstream(std::ostream &out,
                                  int iSub,int nSub); // export as ASCII

   // total number of patterns in a sector
   virtual Long64_t GetNPatterns(int sector) const=0;
   // for loop over all sectors
   virtual int GetFirstSector(void) const=0;
   virtual int GetNextSector(int sector) const=0;

   // for loops over coverage, within coverage over sector
   int GetMaxCoverage(void) const;
   int GetNextCoverage(int coverage) const;
   int GetCoverageFirstSector(int coverage) const;
   int GetCoverageNextSector(int coverage,int sector) const;

   // consistency check of the bank data
   // this checks whether all SSIDs of a sector/plane 
   // are from the same module
   virtual bool CheckConsistency(FTKSSMap *ssMap,int tower,int hwmodeid) const=0;
 protected:
   FTKPatternBySectorReader(char const *name);
   SectorByCoverage_t m_SectorByCoverage;
};

class FTKPatternBySectorWriter : public virtual FTKPatternBySectorBase {
 public:
   enum WRITERTYPE_t {
      kWriterDefault=0,
      kWriterForest=1,
      kWriterIndexed=2
   };
   static FTKPatternBySectorWriter *Factory
      (TDirectory &dir,WRITERTYPE_t type=kWriterDefault);
   // append patterns from file (down to minCoverage)
   //  the default algorithm merges one sector a time, calling
   //    AppendMergedPatterns() fro each sector.
   //    This procedure is not optimal for all file formats
   //  For this reason the method is virtual, to allow for
   //    a more efficient implementation
   virtual int AppendMergedPatterns
      (FTKPatternBySectorReader &in,int minCoverage);
   // append patterns from ASCII file
   int AppendPatternsFromASCIIstream(std::istream &in);
   // append a single pattern
   int AppendPattern(FTKPatternWithSector const &pattern) {
      return AppendPattern(pattern.GetSectorID(),pattern);} 
   int AppendPattern(int sector,FTKPatternWithCoverage const &pattern) {
      return AppendPattern(sector,pattern.GetCoverage(),
                           pattern.GetHitPattern()); 
   }
   virtual int AppendPattern(int sector,int coverage,
                             FTKHitPattern const &pattern)=0;
 protected:
   FTKPatternBySectorWriter(const char *name,TDirectory &dir)
      : FTKPatternBySectorBase(name),m_dir(dir) { }
   virtual int AppendMergedPatternsSector
      (int sector,FTKPatternOneSectorOrdered const *ordered);
   TDirectory &m_dir;
   static const uint64_t PATTERN_CHUNK;
};

class FTKPatternBySectorForestWriter : public FTKPatternBySectorWriter {
public:
   explicit FTKPatternBySectorForestWriter(TDirectory &dir); // create pattern bank
   virtual ~FTKPatternBySectorForestWriter(); // save patterns to TDirectory
   virtual int AppendPattern(int sector,int coverage,
                             FTKHitPattern const &pattern);
 protected:
   virtual int AppendMergedPatternsSector(int sector,
                                    FTKPatternOneSectorOrdered const *ordered);
   typedef std::map<int,FTKPatternRootTree *> PatternTreeBySectorRW_t;
   PatternTreeBySectorRW_t m_patterns;
};

class FTKPatternBySectorForestReader : public FTKPatternBySectorReader {
 public:
   // for this reader, the sectors are organized in a forest of TTrees
   explicit FTKPatternBySectorForestReader
      (TDirectory &dir,std::set<int> const *sectorList=0,int *error=0); // read pattern bank
   explicit FTKPatternBySectorForestReader(FTKRootFileChain &chain,
                                           int *error=0); // read pattern bank
   virtual ~FTKPatternBySectorForestReader();
   virtual Long64_t GetNPatterns(int sector) const;
   virtual int GetFirstSector(void) const;
   virtual int GetNextSector(int sector) const;
   virtual void RewindSector(int sector=-1);
   virtual FTKPatternOneSector *Read(int sector,int minCoverage); 
   virtual bool CheckConsistency(FTKSSMap *ssMap,int tower,int hwmodeid) const;
 protected:
   typedef std::map<int,FTKPatternRootTreeReader *> PatternTreeBySectorRO_t;
   PatternTreeBySectorRO_t m_patterns;
   void InitializeSectorByCoverageTable(void);
};

class FTKPatternBySectorIndexed : public virtual FTKPatternBySectorBase {
 public:
   //int GetBitsPerPlane(void) const;
   static char const *s_cdataTreeName;
   static char const *s_indexTreeName;
   static char const *s_cnpattBranchName;
   static char const *s_cnpattBranchID;
   static char const *GetCPatternBranchName(uint32_t plane);
   static char const *GetCPatternBranchID(uint32_t plane);
   static char const *s_sectorBranchName;
   static char const *s_sectorBranchID;
   static char const *s_ncovBranchName;
   static char const *s_ncovBranchID;
   static char const *s_npatternBranchName;
   static char const *s_npatternBranchID;
   static char const *s_coverageBranchName;
   static char const *s_coverageBranchID;
   static char const *GetNDecoderBranchName(uint32_t plane);
   static char const *GetNDecoderBranchID(uint32_t plane);
   static char const *GetDecoderDataBranchName(uint32_t plane);
   static char const *GetDecoderDataBranchID(uint32_t plane);
 protected:
   static std::vector<std::string> s_cpatternBranch,s_cpatternID;
   static std::vector<std::string> s_ndecoderBranch,s_ndecoderID;
   static std::vector<std::string> s_decoderDataBranch,s_decoderDataID;
};

class FTKPatternBySectorIndexedWriter : public FTKPatternBySectorWriter,
   FTKPatternBySectorIndexed {
public:
   explicit FTKPatternBySectorIndexedWriter(TDirectory &dir); // create pattern bank
   virtual ~FTKPatternBySectorIndexedWriter(); // save patterns to TDirectory
   virtual int AppendPattern(int sector,int coverage,
                             FTKHitPattern const &pattern);
 protected:
   TTree *m_cpatternDataTree;
   std::vector<TBranch *> m_cpatternBranch;
   TBranch *m_cnpattBranch;

   TTree *m_cpatternIndexTree;
   TBranch *m_npatternBranch;
   TBranch *m_coverageBranch;
   std::vector<TBranch*> m_decoderDataBranch;
   // total number of patterns in memory
   class CodedPatternStorage {
   public:
      explicit CodedPatternStorage(uint32_t nPlane) : m_data(nPlane) {
         for(size_t plane=0;plane<m_data.size();plane++)
            m_data[plane].resize(15000000);
         m_next=0;
      }
      // returns size of storage
      uint32_t GetSize(void) const {
         return m_data[0].size();
      }

      // returns 0 is storage is full
      // index in stoage otherwise
      inline uint32_t AllocatePattern(void) {
         uint32_t r=(m_next+1)%(m_data[0].size()+1);
         if(r) m_next=r;
         return r;
      }
      inline void Store(uint32_t ipatt,uint32_t iplane,uint8_t data) {
         m_data[iplane][ipatt-1]=data;
      }
      inline uint8_t Get(uint32_t ipatt,uint32_t iplane) const {
         return m_data[iplane][ipatt-1];
      }
      inline void Delete(uint32_t ipatt) {
         if(ipatt && (ipatt==m_next)) --m_next;
      }
      inline void Clear(void) { m_next=0; }
      bool operator()(uint32_t a,uint32_t b) const {
         for(size_t plane=0;plane<m_data.size();plane++) {
            if(Get(a,plane)<Get(b,plane)) return true;
            if(Get(a,plane)>Get(b,plane)) return false;
         }
         return false;
      }
   protected:
      uint32_t m_next;
      std::vector<std::vector<uint8_t> > m_data;
   };
   CodedPatternStorage *m_patternStorage;
   typedef std::map<uint32_t,uint32_t,CodedPatternStorage&>
      CPatternWithCoverage_t;
   // indexing
   typedef struct PatternData {
     PatternData(int nLayer,CodedPatternStorage *storage)
     : m_encoder(nLayer),m_data(*storage) {
         for(size_t i=0;i<m_encoder.size();i++) {
            m_encoder[i].first.resize(1<<8);
         }
      }
      // encoder, translates [SSID] -> uint8_t
      std::vector< std::pair<std::vector<int>,std::map<int,int> > > m_encoder;
      // patterns with coverage
      CPatternWithCoverage_t m_data;
   } PatternData_t;
   // PatternData indexed by sector
   std::map<int,PatternData_t> m_patternData;
   uint32_t m_treeSector;
   uint32_t m_treeNCov;
   std::vector<uint32_t> m_treeCoverage;
   std::vector<uint32_t> m_treeNPattern;
   std::vector<uint32_t> m_decoderSize;
   void Flush(void);
};

class FTKPatternBySectorIndexedReader : public FTKPatternBySectorReader,
   FTKPatternBySectorIndexed {
 public:
   // for this reader, the sectors are organized in two TTrees
   explicit FTKPatternBySectorIndexedReader
      (TDirectory &dir,std::set<int> const *sectorList=0,int *error=0); // read pattern bank
   explicit FTKPatternBySectorIndexedReader(FTKRootFileChain &chain,
                                           int *error=0); // read pattern bank
   virtual ~FTKPatternBySectorIndexedReader();
   virtual Long64_t GetNPatterns(int sector) const;
   virtual int GetFirstSector(void) const;
   virtual int GetNextSector(int sector) const;
   virtual void RewindSector(int sector=-1);
   virtual FTKPatternOneSector *Read(int sector,int minCoverage); 
   // optimized reader of multiple sectors
   virtual int ReadRaw(int firstSector,
                       std::list<FTKPatternOneSector *> &sectorList,
                       uint64_t maxPattern);
   virtual bool CheckConsistency(FTKSSMap *ssMap,int tower,int hwmodeid) const;
 protected:
   TTree *m_dataTree;
   int InitializeIndex(TTree *indexTree,std::set<int> const *sectorList);

   //
   std::vector<uint32_t> m_decoderData;
   std::vector<uint32_t *> m_decoderDataPtr;
   struct DataBlock_t {
      // pointer to decoder table
      //   decoded_SSID = m_decoderDataPtr[plane][coded_SSID]
      uint32_t **m_decoderDataPtr;
      // pointer to offset in data TTree
      uint32_t m_cpatternEntry;
      // number of patterns in this Block
      uint32_t m_nPattern;
   };

   // indexed by coverage (multiple entries are possible - decoder change)
   typedef std::multimap<uint32_t,DataBlock_t> CoverageTable_t;
   typedef struct SectorInfo {
      // table ordered by coverage
      CoverageTable_t m_coverageTable;
      // pointer to one entry of the table above (for reading)
      CoverageTable_t::const_reverse_iterator m_coveragePtr;
      uint64_t m_nPattern;
   } SectorInfo_t;
   
   // indexed by Sector
   typedef std::map<uint32_t,SectorInfo_t> SectorTable_t;
   SectorTable_t m_sectorTable;
   FTKPatternWithCoverage *m_patternData;
};

#endif
