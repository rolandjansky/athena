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
#include <boost/container/map.hpp>
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

#define MAP boost::container::map

class FTK_CompressedAMBank : public FTKLogging , public FTK_AMsimulation_base {
public:
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

   // erase all pattern data
   void erase(void);

   // full comparison of two pattern banks
   int compare(FTK_CompressedAMBank const *bank) const;


   // purely virtual methods from FTK_AMsimulation_base, to be implemented
   virtual const std::unordered_map<int,FTKSS>& getStrips(int plane);
   virtual const std::list<FTKRoad>& getRoads();
   virtual int passHits(const std::vector<FTKHit> &);
   virtual int informationMatch(FTKRoad *r1,FTKRoad *r2);

   // set number of planes - also initializes various tables
   virtual void setNPlanes(int nPlanes);

   // FTK_AMsimulation_base interface to load bank data
   virtual int readROOTBank(const char*, int maxpatts=-1);
   // set up wildcards here
   virtual void init();

   // methods used in the simulation
   virtual void clearDOandAM(void);
   virtual void simulateDOinput(const std::vector<FTKHit> &hits,
                                std::vector<std::list<int> > &tspSSIDfired);
   virtual void simulateAM(std::vector<std::list<int> > const &tspSSIDfired);
   virtual void simulateDOoutput(void);

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
   struct SectorData {
      //
      // first pattern ID
      uint32_t m_FirstPattern;
      //
      // number of patterns (unpacked)
      uint32_t m_NPattern;
      //
      // packed pattern data - store delta to preceeding pattern
      //   0x00-0x7f : delta=1..128
      //   0x8x : store bit  7..10 if delta-1>0x0000007f
      //   0x9x : store bit 11..14 if delta-1>0x000007ff
      //    ...
      //   0xDx : store bit 27..30 if delta-1>0x07ffffff
      //   0xE1 : store bit 31     if delta-1>0x7fffffff
      //   0xFx : encode repeated occurance of (delta-1) = 0..3
      //      binary=1111mmdd
      //         dd ={00,01,10,11} corresponds to delta-1={1,2,3,4}
      //         mm ={00,01,10,11} corresponds to repeat={4,16,64,256}
      std::vector<uint8_t> m_Data;
   };

   // holds pattern data for a given layer,SSID (all sectors)
   //    the index is the sector number
   typedef MAP<int,SectorData> PatternBySectorMap_t;
   //
   // holds pattern data for a given layer (all SSIDs and all sectors)
   //    the index is the SSID number (in TSP space)
   typedef MAP<int,PatternBySectorMap_t> PatternBySectorSSidMap_t;
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
      MAP<int,std::vector<int> > m_dcSSIDtable;
#ifdef BIT_PACKED_BANK
      //
      // number of bits per pattern for this layer
      //    this is the number of bits
      //    needed to encode SSID+DCbits for this layer
      int m_PatternBits;
#endif
  };
   //
  // hold a full pattern bank
  struct PatternBank {
#ifdef BIT_PACKED_BANK
     //
     // the total number of bits to encode the (dcSSID,DCbit) of all layers
     //  for one pattern
     int m_PatternBitsTotal;
#endif
     //
     // this holds all layer-ordered information of the bank
     //   for example, to locate the patterns given a layer,tspSSID,sector:
     //     m_Bank.m_PatternByLayer[layer].m_SSidData[tspSSID][sector]
     //       returns the coresponding struct SectorData
     std::vector<LayerData> m_PatternByLayer;
     //
     // this holds the Pattern data, ordered by patternID (bit-packed)
     //   the bit number is given by iPattern*m_PatternBitsTotal
#ifdef BIT_PACKED_BANK
     std::vector<uint64_t> m_PatternBitData;
#else
     std::vector<std::vector<uint8_t> > m_pattern8Data;
     std::vector<std::vector<uint16_t> > m_pattern16Data;
#endif
  };
  //
  // holds all pattern data
  PatternBank m_Bank;
  MAP<int,std::pair<int,int> > m_SectorFirstLastPattern;

  //
  // hold wildcards (layer mask) per sector
  MAP<int,uint32_t> m_SectorWildcard;

   // split up AM simulation into two pieces
   //  list of all SSIDs
   //  ordered by sector, layer, ssid
  typedef std::list<SectorData const *> PatternDataBySSID_t;
  typedef std::vector<PatternDataBySSID_t> PatternDataBySectorSSID_t;
  typedef std::vector<PatternDataBySectorSSID_t> FiredSectorData_t;

  // distribute SSIDs to sectors
  virtual void simulateAMdistributeSSID
     (std::vector<std::list<int> > const &tspSSIDfired,
      FiredSectorData_t &firedSectorData) const;
  // loop over sectors and simulate AM
  virtual void simulateAMsectorloop(FiredSectorData_t
                                    const &firedSectorData);
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
  std::vector<MAP<int,std::vector<int> > > m_DCtoTSP;
  //
  // table to convert TSP SSID to SSID and index
  std::vector<MAP<int,std::pair<int,int> > > m_TSPtoDC;
  //
  // lookup-tables to convert compressed DC bits to subSSmask,DC,HB
  //    m_subSSmask[layer][dcHBbits]  returns the subSSmask for this layer
  //    m_dcMaskLookup[layer][dcHBbits]  returns the DC bits for this layer
  //    m_hbMaskLookup[layer][dcHBbits]  returns the DC bits for this layer
  // here, dcHBbits is extracted from bits inside m_Bank.m_PatternBitData[]
  std::vector<std::vector<int> > m_subSSmask;
  std::vector<std::vector<int> > m_dcMaskLookup;
  std::vector<std::vector<int> > m_hbMaskLookup;
  //
  // lookup-table to get the number of bits in a 16-bit word
  //    for example, m_nHit16[0x38]=3 because three bits ars set
  std::vector<uint8_t> m_nHit16;
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
