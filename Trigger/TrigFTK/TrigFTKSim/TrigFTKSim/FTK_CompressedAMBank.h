/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __H_FTK_COMPRESSEDAMBANKTSP
#define __H_FTK_COMPRESSEDAMBANKTSP

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKLogging.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTK_AMsimulation_base.h"
#include <inttypes.h>
#include <vector>
#include <map>
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

//#include <boost/container/map.hpp>
//#include <boost/container/vector.hpp>
//#include <boost/container/list.hpp>
//#define MAP boost::container::map
//#define VECTOR boost::container::vector
//#define LIST boost::container::list
#define MAP std::map
#define VECTOR std::vector
#define LIST std::list

//#define CANDIDATES_WITHOUT_SECTORID
#define OPTIMIZED_VECTORMAP

#ifdef OPTIMIZED_VECTORMAP
// this map has a parallel linear stucture for fast loops
//   using "const_ptr" rather than const_iterator
template<class T> class VECTORMAP : public MAP<int,T> {
 public:
   VECTORMAP() : fData(0),fSize(0) { }
   virtual ~VECTORMAP() { if(fData) delete [] fData; }
   class const_ptr {
   public:
      inline const_ptr(typename std::pair<const int,T> const **p=0) : ptr(p) {
      }
      inline typename std::pair<const int,T> const &operator*(void) {
         return **ptr;
      }
      inline bool operator!=(const_ptr const &cp) { return ptr!=cp.ptr; }
      inline const_ptr &operator++(void) { ++ptr; return *this; }
   protected:
      typename std::pair<const int,T> const **ptr;
   };
   int getMemoryEstimate(void) const {
      return ((uint8_t const *)(this+1)-(uint8_t const *)this)+
         (sizeof(typename std::pair<int, T>)+4*sizeof(void *))*
         MAP<int,T>::size()+sizeof(void *)*fSize;
   }
   inline const_ptr beginPtr(void) const { return fData; }
   inline const_ptr endPtr(void) const { return fData+fSize; }
   inline void pack() {
      if(fData) { delete [] fData; fData=0; }
      fSize=MAP<int,T>::size();
      if(fSize) {
         fData=new typename std::pair<const int,T> const * [fSize];
         unsigned k=0;
         for(typename MAP<int,T>::const_iterator i=MAP<int,T>::begin();
             i!=MAP<int,T>::end();i++) {
            fData[k++]=&(*i);
         }
      }
   }
 protected:
   typename std::pair<const int,T> const **fData;
   unsigned fSize;
};
#else
template<class T> class VECTORMAP : public MAP<int,T> {
public:
   typedef typename MAP<int,T>::const_iterator const_ptr;
   inline const_ptr beginPtr(void) const { return MAP<int,T>::begin(); }
   inline const_ptr endPtr(void) const {return MAP<int,T>::end(); }
   inline void pack() { }
};
#endif

class FTK_CompressedAMBank : public FTKLogging , public FTK_AMsimulation_base {
public:
   
   virtual ~FTK_CompressedAMBank();
   // set up pattern bank
   FTK_CompressedAMBank(int bankID,int subID,FTKSSMap *ssMap=0,
                        FTKSSMap *ssMapTSP=0,
                        char const *name="FTK_CompressedAMBank");
   // set SS map (TSP)
   void setSSMapTSP(FTKSSMap *m_ssmap_tsp) { m_SSmapTSP=m_ssmap_tsp; }
   FTKSSMap *getSSMapTSP() const { return m_SSmapTSP; }

   // convert cached file to sector ordered file (slow!)
   int convertCachedBankFiles(std::vector<char const *> &cachedBanks,
                              char const *sectorOrderedFile);

   // read sector-ordered file (slow!)
   int readSectorOrderedFile(char const *sectorOrderedFile);

   // write cached-bank file
   int writeCachedBankFile(char const *cachedBankFile) const;
  
   // write binary file (fast but hardware-dependent!)
   int writeBinaryFile(char const *binaryLookup) const;

   // write root file (reasonaby fast)
   int writeRootFile(char const *rootLoopup) const;

   // read binary file (fast but hardware dependent)
   int readBinaryFile(char const *rootLoopup);

   // read root file (reasonably fast)
   int readRootFile(char const *binaryLookup);

   // full comparison of two pattern banks
   int compare(FTK_CompressedAMBank const *bank) const;


   // purely virtual methods from FTK_AMsimulation_base, to be implemented
   virtual const std::unordered_map<int,FTKSS>& getStrips(int plane);
   virtual const std::list<FTKRoad>& getRoads();
   virtual int informationMatch(FTKRoad *r1,FTKRoad *r2);

   // set number of planes - also initializes various tables
   virtual void setNPlanes(int nPlanes);

   // FTK_AMsimulation_base interface to load bank data
   virtual int readROOTBank(const char*, int maxpatts=-1);
   // set up wildcards here
   virtual void init();

   // methods requuired by passHits in the simulation
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
   //   TSPMap???
   std::vector<int> const &getTSPssidVector(int layer,int dcSSID);
   inline int getTSPssid(int layer,int dcSSID,int tspXY) {
      return getTSPssidVector(layer,dcSSID)[tspXY];
   }
   std::pair<int,int> const &getDCssid(int layer,int tspSSID);

protected:
   // finalize memory structures after readin bank data from file
   void readBankPostprocessing(const char *where);

   // hold pattern data for a given layer,SSID,sector (all patterns)
   //
   // loop over packed pattern data
   //
   // packed pattern data - store delta to preceeding pattern
   //   0x00-0x7f : delta=1..128
   //   0x8x : store bit  7..10 if delta-1>0x0000007f
   //   0x9x : store bit 11..14 if delta-1>0x000007ff
   //    ...
   //   0xDx : store bit 27..30 if delta-1>0x07ffffff
   //   0xE1 : store bit 31     if delta-1>0x7fffffff
   //   0xE4..0xE7 : store length (1..4 byte, big-endian)
   //   0xE8..0xEB : store nPatterns (1..4 byte, big-endian)
   //   0xEC..0xEF : store firstPattern address relative to sector start
   //                  (1..4 byte, big-endian)
   //   0xFx : encode repeated occurance of delta=1 (repeat=2..17)
   template<class A>
      inline void patternLoop(A &a,uint8_t const __restrict *ptr,
                              uint32_t firstPattern,int nPattern) const;
   struct SectorData {
      //
      // first pattern ID
      uint32_t m_FirstPattern;
      //
      // number of patterns (unpacked)
      uint32_t m_NPattern;
      uint32_t m_offset;
      uint32_t m_length;
   };

   //
   // holds pattern data for a given layer (all SSIDs and all sectors)
   //    the index is the SSID number (in TSP space)
   typedef VECTORMAP<SectorData> PatternBySector_t;
   typedef VECTORMAP<PatternBySector_t> PatternBySectorSSidMap_t;
   //    the index is the sector number
   //
   // holds patterndata and auxillary data for a given layer
   struct LayerData {
      //
      // pattern data for lookup by SSID
      //    m_SSidData[SSID][sector] gives acces to the encoded
      //          list of patterns for this SSID,sector combination
      PatternBySectorSSidMap_t m_SSidData;
      //
      // dcSSID IDs by sector (for unpacking pattern data)
      //    m_dcSSIDtable[sector][i]  is an SSID (in dc-space)
      VECTOR<VECTOR<int> > m_dcSSIDbySectorIndex;
      //
      // this holds the byte-compressed pattern delta data by sector
      VECTOR<VECTOR<uint8_t> > m_CompressedDeltaBySector;
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
     // this holds the Pattern data, ordered by patternID (bit-packed)
     //   the bit number is given by iPattern*m_PatternBitsTotal
     VECTOR<std::vector<uint8_t> > m_pattern8Data;
     VECTOR<std::vector<uint16_t> > m_pattern16Data;
  };
  void erase();
  //
  // holds all pattern data
  PatternBank m_bank;
  MAP<int,std::pair<int,int> > m_SectorFirstLastPattern;
#ifdef CANDIDATES_WITHOUT_SECTORID
  MAP<int,int> m_patternToSector;
#endif
  //
  // hold wildcards (layer mask) per sector
  typedef uint8_t HitPattern_t;
  MAP<int,HitPattern_t> m_SectorWildcard;

  //
  // TSP-SSIDs 
  std::vector<std::list<int> > m_tspSSID;
  //
  // used sectors
  VECTOR<HitPattern_t> m_sectorUsage;
  //
  // hit patterns 
  VECTOR<HitPattern_t> m_hitPatterns;
  //
  // road candidates
  static int const MAX_NROAD;
  unsigned m_nRoadCand;
  VECTOR<
#ifdef CANDIDATES_WITHOUT_SECTORID
     uint32_t
#else
     std::pair<uint32_t,uint32_t>
#endif
     > m_roadCand;
  //
  // minimum number of hits
  uint8_t m_nhWCmin;
  int m_MAX_MISSING_PLANES;
  int m_MAX_MISSING_SCT_PAIRS;

 private:
  //
  // these methods are used to populate tables to translate
  //   tspSSID to dcSSID and back
  //   (this should be moved to another class?)
  int getTSPssidSlow(int layer,int ssid,int tspXY);
  int getDCssidSlow(int layer,int tspSSID);  
  void insertSSID(int layer,int tspSSID,int dcSSID);
  //
  // table to convert SSID and tspXY offsets to a TSP SSID
  VECTOR<MAP<int,std::vector<int> > > m_DCtoTSP;
  //
  // table to convert TSP SSID to SSID and index
  VECTOR<MAP<int,std::pair<int,int> > > m_TSPtoDC;
  //
  // lookup-tables to convert compressed DC bits to subSSmask,DC,HB
  //    m_subSSmask[layer][dcHBbits]  returns the subSSmask for this layer
  //    m_dcMaskLookup[layer][dcHBbits]  returns the DC bits for this layer
  //    m_hbMaskLookup[layer][dcHBbits]  returns the DC bits for this layer
  // here, dcHBbits is extracted from bits inside m_Bank.m_PatternBitData[]
  VECTOR<VECTOR<int> > m_subSSmask;
  VECTOR<VECTOR<int> > m_dcMaskLookup;
  VECTOR<VECTOR<int> > m_hbMaskLookup;

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
  //   map TSP to DC geometry (fine superstrip <-> coarse superstrip)
  TSPMap *m_TSPmap;
};

#endif
