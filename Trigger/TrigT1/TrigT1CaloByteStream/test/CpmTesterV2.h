/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMTESTERV2_H
#define TRIGT1CALOBYTESTREAM_CPMTESTERV2_H

#include <map>
#include <string>
#include <stdint.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class CMXCPHits;
  class CMXCPTob;
  class CPMTower;
  class CPMTobRoI;
  class TriggerTowerKey;
}

namespace LVL1BS {

/** Algorithm to test CPM component bytestream conversions.
 *
 *  Just prints out the contents of the CPMTower objects
 *  Also includes CMXCPTob, CMXCPHits and CPMTobRoIs.
 *
 *  Run before writing bytestream and after reading it and compare.
 *
 *  @author Peter Faulkner
 */

class CpmTesterV2 : public AthAlgorithm {

 public:
   CpmTesterV2(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~CpmTesterV2();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   typedef DataVector<LVL1::CPMTower>                     CpmTowerCollection;
   typedef DataVector<LVL1::CMXCPTob>                     CmxCpTobCollection;
   typedef DataVector<LVL1::CMXCPHits>                    CmxCpHitsCollection;
   typedef DataVector<LVL1::CPMTobRoI>                       CpmRoiCollection;
   typedef std::map<unsigned int, const LVL1::CPMTower*>  CpmTowerMap;
   typedef std::map<int,          const LVL1::CMXCPTob*>  CmxCpTobMap;
   typedef std::map<int,          const LVL1::CMXCPHits*> CmxCpHitsMap;
   typedef std::map<uint32_t,     const LVL1::CPMTobRoI*> CpmRoiMap;

   /// Print the CPM towers
   void printCpmTowers(const std::string& source) const;
   /// Print the CMX-CP TOBs
   void printCmxCpTobs()   const;
   /// Print the CMX-CP hits
   void printCmxCpHits() const;
   /// Print the CPM RoIs
   void printCpmRois(const std::string& source)   const;

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;
   /// Print a vector of hits
   void printVecH(const std::vector<unsigned int>& vec, int bits,
                                                   int words) const;

   /// Set up CPM tower map
   void setupCpmTowerMap(const CpmTowerCollection* ttCollection);
   /// Set up CMX-CP TOB map
   void setupCmxCpTobMap(const CmxCpTobCollection* tobCollection);
   /// Set up CMX-CP hits map
   void setupCmxCpHitsMap(const CmxCpHitsCollection* hitCollection);
   /// Set up CPM RoI map
   void setupCpmRoiMap(const CpmRoiCollection* roiCollection);

   /// CPM tower key provider
   LVL1::TriggerTowerKey* m_towerKey;
   /// CPM tower container StoreGate key
   std::string m_cpmTowerLocation;
   /// CMX-CP TOB container StoreGate key
   std::string m_cmxCpTobLocation;
   /// CMX-CP hits container StoreGate key
   std::string m_cmxCpHitsLocation;
   /// CPM RoI container StoreGate key
   std::string m_cpmRoiLocation;
   /// CPM RoI from RoIB container StoreGate key
   std::string m_cpmRoiLocationRoib;
   /// CPM tower overlap container StoreGate key
   std::string m_cpmTowerLocationOverlap;
   /// Force number of CPM slices
   int m_forceSlicesCpm;
   /// Force number of CMX slices
   int m_forceSlicesCmx;
   /// CPM tower print flag
   int m_cpmTowerPrint;
   /// CMX-CP TOB print flag
   int m_cmxCpTobPrint;
   /// CMX-CP hits print flag
   int m_cmxCpHitsPrint;
   /// CPM RoI print flag
   int m_cpmRoiPrint;
   /// CPM RoI from RoIB print flag
   int m_cpmRoiPrintRoib;
   /// CPM tower overlap print flag
   int m_cpmTowerPrintOverlap;

   /// CPM tower map
   CpmTowerMap  m_ttMap;
   /// CMX-CP TOB map
   CmxCpTobMap  m_tobMap;
   /// CMX-CP hits map
   CmxCpHitsMap m_hitsMap;
   /// CPM RoI map
   CpmRoiMap    m_roiMap;

};

} // end namespace

#endif
