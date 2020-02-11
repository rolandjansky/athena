/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __H_FTK_COMPRESSEDAMBANK
#define __H_FTK_COMPRESSEDAMBANK

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKLogging.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTK_AMsimulation_base.h"
#include "TrigFTKSim/FTK_CompressedSectorPatternList.h"
#include "TrigFTKSim/FTK_HitMask.h"
#include <inttypes.h>
#include <vector>
#include <map>
#include <set>
#include <list>

/* class FTK_CompressedAMBank
 *
 * perform an emulation of the FTK pattern recognition
 *   patterns are stored in compressed form, both on disk and in memory
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

class FTKSSMap;
class TSPMap;
class FTKHitPattern;
class FTKHitPatternCompare;
class FTKPatternOneSector;
class FTKPatternBySectorReader;

#define ROAD_CAND_AS_VECTOR

//#include <boost/container/map.hpp>
//#include <boost/container/vector.hpp>
//#include <boost/container/list.hpp>
//#define MAP boost::container::map
//#define VECTOR boost::container::vector
//#define LIST boost::container::list
#define MAP std::map
#define VECTOR std::vector
#define LIST std::list

template<class T> class VECTORMAP : public MAP<int,T> {
public:
   typedef typename MAP<int,T>::const_iterator const_ptr;
   inline const_ptr beginPtr(void) const { return MAP<int,T>::begin(); }
   inline const_ptr endPtr(void) const {return MAP<int,T>::end(); }
   inline void pack() { }
   int getMemoryEstimate(void) const {
      // size of the class
      // data plus four pointers (parent,prev,next,child) per node
      return
         ((uint8_t const *)(this+1)-(uint8_t const *)this)+
         (sizeof(typename std::pair<int, T>)+
          +4*sizeof(void *))*MAP<int,T>::size();
   }
};

class FTK_CompressedAMBank : public FTKLogging , public FTK_AMsimulation_base {
public:
   
   virtual ~FTK_CompressedAMBank();
   // set up pattern bank
   FTK_CompressedAMBank(int bankID,int subID,FTKSSMap *ssMap=0,
                        FTKSSMap *ssMapTSP=0,
                        int hwmodeid_TSP=-1,int hwmodeid_DC=-1,
                        char const *name="FTK_CompressedAMBank");

   // set compression scheme
  //
  // compression scheme
  enum COMPRESSION {
     COMPRESSION_SKIP=-1,   // skip "finalize step"
     COMPRESSION_DEFAULT=0, // use default compression scheme

     COMPRESSION_U7=FTK_CompressedPatternList::ENCODING_U7,
     COMPRESSION_U32=FTK_CompressedPatternList::ENCODING_U32,
     COMPRESSION_DELTA=FTK_CompressedPatternList::ENCODING_DELTA,
     COMPRESSION_BIT4=FTK_CompressedPatternList::ENCODING_BIT4
  };
   void setCompressionScheme(int scheme);
   inline int getCompressionScheme(void) const { return m_compressionScheme; };

   // set parameters for TSP import
   void setNDCmax(int ndc);
   void setNDCmaxPlane(size_t plane,int ndc);
   inline int getNDCmax(void) const { return m_nDCmax; }
   inline int getNDCmaxPlane(size_t plane) const { return m_nDCmaxPlane[plane]; }
   void setWildcardPenalty(int penalty);
   void setWildcardPenaltyPlane(size_t plane,int penalty);
   inline int getWildcardPenalty(void) const { return m_wildcardPenalty; }
   inline int getWildcardPenaltyPlane(size_t plane) const {
      return m_wildcardPenaltyPlane[plane]; }

   // check hwmodeid settings
   int getHWModeSS_dc(void) const;
   int getHWModeSS_tsp(void) const;

   // set SS map (TSP)
   void setSSMapTSP(FTKSSMap *m_ssmap_tsp) { m_SSmapTSP=m_ssmap_tsp; }
   FTKSSMap *getSSMapTSP() const { return m_SSmapTSP; }

   void readSectorDefinition(const char *sectorFileHWMODEID0,
                             const char *sectorFileHWMODEID2);

   // write cached-bank file
   //   flat format=0: pcache format
   //   flat format=1: pbank format
   //   flat format=2: pbank+ccache format
   int writePCachedBankFile(char const *cachedBankFile,
                            int flatFormat=0,int nsub=0,int nlamb=0) const;

   // write root file (reasonaby fast)
   int writeCCachedBankFile(char const *cachedBankFile,int flatFormat=0) const;
   int writeCCachedBankFile(TDirectory *out) const;

   // full comparison of two pattern banks
   int compare(FTK_CompressedAMBank const *bank) const;

   // print head and tail of one sector of the pattern bank
   void printSector(int sector,int npattern=10,int ipattern=-1);

   // purely virtual methods from FTK_AMsimulation_base, to be implemented
   virtual const std::unordered_map<int,FTKSS>& getStrips(int plane);
   virtual const std::list<FTKRoad>& getRoads();
   virtual int informationMatch(FTKRoad *r1,FTKRoad *r2);

   // set number of planes - also initializes various tables
   virtual void setNPlanes(int nPlanes);

   // FTK_AMsimulation_base interface to load bank data
   virtual int readROOTBank(const char*, int maxpatts);
   virtual int readROOTBankCache(const char*);
   
   virtual void init();

   // methods required by passHits in the simulation
   virtual void clear(void);
   virtual void sort_hits(const std::vector<FTKHit> &);
   virtual void data_organizer(void);
   virtual void am_in(void);
   virtual void am_output(void);

   // for pattern finding in absence of FTKHit structures
   void data_organizer_r(std::vector<std::list<int> > const &tspSSIDfired);
   void am_in_r(std::vector<std::list<int> > const &tspSSIDfired);

   // translate SSID from DC to TSP and back
   // these functions could possibly be moved elsewhere
   // argument is SSID w/o dc bits
   // returns vector with dc-bit as index, contains TSP-SSID
   // (note, the dc-bits possibly are gray-coded)
   std::vector<int> const &getTSPssidVector(int layer,int dcSSID);
   inline int getTSPssid(int layer,int dcSSID,int tspXY) {
      return getTSPssidVector(layer,dcSSID)[tspXY];
   }
   // returns pair(SSID w/o dc bits , dc bits)
   std::pair<int,int> const &getDCssid(int layer,int tspSSID);
   int getDCssidConst(int layer,int tspSSID) const;

   // read root file (sector-ordered), one subregion
   int readSectorOrderedBank(const char *name, int maxpatts,int nSub,int numSectorMax,int nlamb);
   // read root file (sector-ordered)
   //   the sectors are divided into partitions
   //   each partition comes with a maximum number of patterns
   int readSectorOrderedBank(const char *name,const char *partitionListName,
                             int nSub,int numSectorMax,int nlamb);
   // read root file (sector ordered)
   // for each sector,determine the maximum number of DC patterns
   // (when packing according to the DC packing algorithm)
   /// returns: list of sectors with maximum number of patterns
   MAP<int,int> countSectorOrderedPatterns(const char *name);

   // this method reads patterns as defined in the partition list
   // a given partition implements two limits
   //   maximum number of sectors
   //   maximum number of patterns
   // and it comes with alist of valid sector IDs
   struct Partition {
     Partition(int patternMax,int sectorMax,std::set<int> const &sectorSet)
     : fNumPatternMax(patternMax),fNumSectorMax(sectorMax),fSectorSet(sectorSet) { }
      int fNumPatternMax;
      int fNumSectorMax; // if this is negative, no limit shall be applied
      std::set<int> fSectorSet;
   };
   int readPartitionedSectorOrderedBank(const char *name,
                                        std::list<Partition> const &partitionList,int nlamb);

   bool isReservedPatternId(int patternID) const;

   void printStrips(int plane=-1) const;

   // read root file (pcache)
   int readPCachedBank(TDirectory *pcache,int nsub=0);

   // read root file (Compressed cache)
   int readCCachedBank(TDirectory *ccache);

protected:

   // convert sector-ordered DC bank (in memory) to TSP-bank
   void importDCpatterns
      (int nLayer,int offsetSSID,int32_t *ssidData,
       VECTOR<std::pair<int,int> > const &sectorPointer,int nlamb);

   // finalize memory structures after reading bank data from file
   void readBankPostprocessing(const char *where);

   // read wildcards from default files
   void readWildcards(void);

   // read bad module lists
   void importBadModuleASCII(std::string const &name,int type);
   void importBadModuleTTree(TDirectory *dir,char const *name="BadModules");
   void exportBadModuleTTree(TDirectory *dir,char const *name="BadModules") const;
   // set up wildcards for bad modules
   void setupSectorWildcards(void);

   VECTOR<MAP<int,int> > m_badModules;

   //
   // holds pattern data for a given layer (all SSIDs and all sectors)
   //    the index is the SSID number (in TSP space)
   typedef VECTORMAP<FTK_CompressedSectorPatternList> PatternBySectorSSidMap_t;

   //
   // store SSID and ternary bits by pattern ID for one layer
   struct SSIDternaryByPattern {
      //
      // dcSSID IDs by sector (for unpacking pattern data)
      //    m_dcSSIDtable[sector][i]  is an SSID (in dc-space)
      VECTOR<VECTOR<int> > m_dcSSIDbySectorIndex;
      //
      // the data
      std::vector<uint8_t> m_pattern8DataPacked;
      std::vector<uint16_t> m_pattern16DataPacked;
      //
      // offset for unpacking ternary bits
      uint32_t m_offsetTernary;
      //
      // method to extract packed data
      inline uint16_t getPackedData(uint32_t patternID) const {
         uint16_t data;
         if(patternID<m_pattern8DataPacked.size())
            data=m_pattern8DataPacked[patternID];
         else data=m_pattern16DataPacked[patternID];
         return data;
      }
      //
      // method to unpack SSID
      inline int getSSID(uint32_t sector,uint32_t patternID) const {
         uint32_t data=getPackedData(patternID);
         uint32_t indexSSID=(data-1)/m_offsetTernary;
         return m_dcSSIDbySectorIndex[sector][indexSSID];
      }
      //
      // method to unpack ternary bits
      //   return.first : DC bits
      //   return .second : HB bits
      inline std::pair<uint16_t,uint16_t> getDCHB(int patternID)
         const {
         std::pair<uint16_t,uint16_t> r(std::make_pair(0,0));
         uint32_t data=(getPackedData(patternID)-1)%m_offsetTernary;
         for(int bits3=m_offsetTernary/3;bits3;bits3/=3) {
            int t=(data/bits3)%3;
            r.second = (r.second<<1)|(t &1);
            r.first = (r.first<<1)|(t>>1);
         }
         return r;
      }
   };
   //
   // holds patterndata and auxillary data for a given layer
   struct LayerData {
      //
      // pattern data for lookup by SSID
      //    m_SSidData[SSID][sector] gives acces to the encoded
      //          list of patterns for this SSID,sector combination
      PatternBySectorSSidMap_t m_SSidData;
      //
      // this holds the Pattern data, ordered by patternID 
      //  each word is packed as:
      //     DC*m_offsetTernary+TERNARY
      // TERNARY is packed using {0,1,X} <-> {0,1,2}
      struct SSIDternaryByPattern m_SSIDternaryByPattern;
  };
   class Range {
   public:
      inline Range(void) : m_head(0),m_tail(0) { }
      inline int First(void) const {return m_head; }
      inline int Last(void) const {return m_tail-1; }
      inline int Size(void) const { return m_tail-m_head; }
      inline void SetRange(int first,int last) {
         m_head=first; m_tail=last+1; }
      inline void UpdateRange(int first,int last) {
         if(Size()<=0) SetRange(first,last);
         else {
            if(first<m_head) m_head=first;
            if(last>=m_tail) m_tail=last+1; } }
   protected:
      int m_head,m_tail;
   };
   //
  // hold a full pattern bank
  struct PatternBank {
     //
     // this holds all layer-ordered information of the bank
     //   for example, to locate the patterns given a layer,tspSSID,sector:
     //     m_Bank.m_PatternByLayer[layer].m_SSidData[tspSSID][sector]
     //       returns the coresponding struct SectorData
     VECTOR<LayerData> m_PatternByLayer;
     //
     //
     // this holds auxillary information (if available)
     //  number of TSP patterns per pattern
     //  integrated coverage per pattern
     VECTOR<uint32_t> m_numTSP,m_coverage;
     //
     // for each sector, give the pattern range
     VECTOR<Range> m_PatternRangeBySector;
  };
  void erase();
  //
  // holds all pattern data
  PatternBank m_bank;
  MAP<int,bool> m_tooFew;
  MAP<int,int> m_maxPatterns;

  //
  // types to hold hit-mask for a pattern
  //
  // hold wildcards (layer mask) per sector
  VECTOR<FTK_HitMask> m_SectorWC;
  //
  // TSP-SSIDs 
  std::vector<std::list<int> > m_tspSSID;
  //
  // used sectors
  VECTOR<FTK_HitMask> m_sectorUsage;
  //
  // hit patterns 
  FTK_HitMaskVector m_hitMask;
  //
  // road candidates
#ifdef ROAD_CAND_AS_VECTOR
  static int const MAX_ROADCAND;
  std::vector<std::pair<uint32_t,uint32_t> > m_roadCand;
#else
  VECTORMAP<uint32_t> m_roadCandMap;
#endif
  //
  // minimum number of hits
  uint8_t m_nhWCmin;
  int m_MAX_MISSING_PLANES;

 private:
  //
  // method to insert TSP patterns from one sector
  typedef std::multimap<FTKHitPattern,std::pair<uint64_t,uint64_t>,
     FTKHitPatternCompare> HitPatternMap_t;
  void insertPatterns(int sector,FTKPatternOneSector const *patterns,
                      int maxpatts,VECTOR< HitPatternMap_t > &dcPatterns,
                      int &nDC,int &nTSP);
  //
  // these methods are used to populate tables to translate
  //   tspSSID to dcSSID and back
  //   (this should be moved to another class?)
  int getTSPssidSlow(int layer,int ssid,int tspXY);
  int getDCssidSlow(int layer,int tspSSID,int *moduleID);
  void insertSSID(int layer,int tspSSID,int dcSSID);
  void invalidateSSIDtables(void);

  //
  // table to convert SSID w/o DC bits and DC bits to a TSP SSID
  VECTOR<MAP<int,std::vector<int> > > m_DCtoTSP;
  //
  // table to convert TSP SSID to SSID w/o DC bits and dc bits
  VECTOR<MAP<int,std::pair<int,int> > > m_TSPtoDC;
  //
  // lookup-tables to convert compressed DC bits to subSSmask,DC,HB
  //    m_dcMaskLookup[layer][dcHBbits]  returns the DC bits for this layer
  //    m_hbMaskLookup[layer][dcHBbits]  returns the HB bits for this layer
  // here, dcHBbits is extracted from bits inside m_Bank.m_PatternBitData[]
  VECTOR<VECTOR<int> > m_dcMaskLookup;
  VECTOR<VECTOR<int> > m_hbMaskLookup;
  // lookup-tables to get encoded (DC,HB) bits given the subindex of TSP wrt DC
  VECTOR<VECTOR<uint64_t> > m_dcBitsLookup1;
  // lookup-table to get number of active DC bits given the
  //   encoded (DC,HB) bits
  VECTOR<VECTOR<uint8_t> > m_ndcBitsLookup;
  // lookup table to calculate the encoded merged (DC,HB) bits
  //   given two pairs of encoded (DC,HB) bits
  VECTOR<VECTOR<uint64_t> > m_dcBitsLookup2;
  //
  // lookup-table to get the number of bits in a 16-bit word
  //    for example, m_nHit16[0x38]=3 because three bits ars set
  VECTOR<uint8_t> m_nHit16;

  //
  // data members required for the getStrips method
  //   m_FiredSSmap[layer] : list of all superstrips
  //   m_UsedSSmap[layer] : list of those superstrips which belong to a road
  std::vector<std::unordered_map<int,FTKSS> > m_FiredSSmap,m_UsedSSmap;

  // FTK geometry maps
  //   TSP geometry (fine segmentation)
  FTKSSMap *m_SSmapTSP;

  // steering to interpret SSID
  int m_hwmodeIDtsp,m_hwmodeIDdc;

  //   map TSP to DC geometry (fine superstrip <-> coarse superstrip)
  TSPMap *m_TSPmap;

  // convert HWMODEID=0 to HWMODEID=2 and back
  //   m_moduleIdHW0[plane][sector] : module identifier for HWMODEID=0
  //   m_moduleIdHW2[plane][sector] : module identifier for HWMODEID=2
  VECTOR<VECTOR<int> > m_moduleIdHW0,m_moduleIdHW2;

  // steering for importing patterns
  VECTOR<int32_t> m_nDCmaxPlane;
  VECTOR<int32_t> m_wildcardPenaltyPlane;
  int32_t m_nDCmax;
  int32_t m_wildcardPenalty;

  //
  // identifier for wildcard SS
  static int const s_WILDCARDid;
  static int const s_INVALIDid;
  int m_compressionScheme;
};

#endif
