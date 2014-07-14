/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEMTESTERV2_H
#define TRIGT1CALOBYTESTREAM_JEMTESTERV2_H

#include <map>
#include <string>
#include <stdint.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class CMXEtSums;
  class CMXJetTob;
  class CMXJetHits;
  class CMXRoI;
  class JEMEtSums;
  class JEMTobRoI;
  class JetElement;
  class JetElementKey;
}

namespace LVL1BS {

/** Algorithm to test JEM component bytestream conversions.
 *
 *  Just prints out the contents of the JetElement objects,
 *  JEMEtSums, CMXJetTob, CMXJetHits, CMXEtSums, JEMTobRoI and CMXRoI.
 *
 *  Run before writing bytestream and after reading it and compare.
 *
 *  @author Peter Faulkner
 */

class JemTesterV2 : public AthAlgorithm {

 public:
   JemTesterV2(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~JemTesterV2();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   typedef DataVector<LVL1::JetElement>              JetElementCollection;
   typedef DataVector<LVL1::JEMEtSums>               EnergySumsCollection;
   typedef DataVector<LVL1::CMXJetTob>               CmxTobCollection;
   typedef DataVector<LVL1::CMXJetHits>              CmxJetCollection;
   typedef DataVector<LVL1::CMXEtSums>               CmxEnergyCollection;
   typedef DataVector<LVL1::JEMTobRoI>               JemRoiCollection;
   typedef std::map<unsigned int, const LVL1::JetElement*> JetElementMap;
   typedef std::map<int, const LVL1::JEMEtSums*>     EnergySumsMap;
   typedef std::map<int, const LVL1::CMXJetTob*>     CmxTobMap;
   typedef std::map<int, const LVL1::CMXJetHits*>    CmxHitsMap;
   typedef std::map<int, const LVL1::CMXEtSums*>     CmxSumsMap;
   typedef std::map<uint32_t, const LVL1::JEMTobRoI*>   JemRoiMap;

   /// Print the jet elements
   void printJetElements(const std::string& source) const;
   /// Print the energy sums
   void printEnergySums() const;
   /// Print the CMX TOBs
   void printCmxTobs()    const;
   /// Print the CMX hits
   void printCmxHits()    const;
   /// Print the CMX energy sums
   void printCmxSums()    const;
   /// Print the JEM RoIs
   void printJemRois(const std::string& source) const;
   /// Print the CMX RoIs
   void printCmxRois(const std::string& source,
                     const LVL1::CMXRoI* roi)   const;

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;
   /// Print a vector (unsigned)
   void printVecU(const std::vector<unsigned int>& vec) const;
   /// Print a vector of hits
   void printVecH(const std::vector<unsigned int>& vec, int bits,
                                                        int words) const;

   /// Set up jet element map
   void setupJeMap(const JetElementCollection* jeCollection);
   /// Set up energy sums map
   void setupEtMap(const EnergySumsCollection* etCollection);
   /// Set up CMX TOB map
   void setupCmxTobMap(const CmxTobCollection* tobCollection);
   /// Set up CMX hits map
   void setupCmxHitsMap(const CmxJetCollection* hitCollection);
   /// Set up CMX energy sums map
   void setupCmxEtMap(const CmxEnergyCollection* etCollection);
   /// Set up JEM RoI map
   void setupJemRoiMap(const JemRoiCollection* jrCollection);

   /// Jet element key provider
   LVL1::JetElementKey* m_elementKey;
   /// Jet element container StoreGate key
   std::string m_jetElementLocation;
   /// Energy sums container StoreGate key
   std::string m_jemEtSumsLocation;
   /// CMX TOB container StoreGate key
   std::string m_cmxTobLocation;
   /// CMX hits container StoreGate key
   std::string m_cmxJetLocation;
   /// CMX energy sums container StoreGate key
   std::string m_cmxEnergyLocation;
   /// JEM RoI container StoreGate key
   std::string m_jemRoiLocation;
   /// CMX RoI container StoreGate key
   std::string m_cmxRoiLocation;
   /// JEM RoI from RoIB container StoreGate key
   std::string m_jemRoiLocationRoib;
   /// CMX RoI from RoIB container StoreGate key
   std::string m_cmxRoiLocationRoib;
   /// Jet element overlap container StoreGate key
   std::string m_jetElementLocationOverlap;
   /// Force number of JEM slices
   int m_forceSlicesJem;
   /// Force number of CMX slices
   int m_forceSlicesCmx;
   /// Jet element print flag
   int m_jetElementPrint;
   /// Energy sums print flag
   int m_jemEtSumsPrint;
   /// CMX TOB print flag
   int m_cmxTobPrint;
   /// CMX hits print flag
   int m_cmxHitsPrint;
   /// CMX energy sums print flag
   int m_cmxEtSumsPrint;
   /// JEM RoI print flag
   int m_jemRoiPrint;
   /// CMX RoI print flag
   int m_cmxRoiPrint;
   /// JEM RoI from RoIB print flag
   int m_jemRoiPrintRoib;
   /// CMX RoI from RoIB print flag
   int m_cmxRoiPrintRoib;
   /// Jet element overlap print flag
   int m_jetElementPrintOverlap;

   /// Jet element map
   JetElementMap m_jeMap;
   /// Energy sums map
   EnergySumsMap m_etMap;
   /// CMX TOB map
   CmxTobMap     m_cmxTobMap;
   /// CMX hits map
   CmxHitsMap    m_cmxHitsMap;
   /// CMX energy sums map
   CmxSumsMap    m_cmxEtMap;
   /// JEM RoI map
   JemRoiMap     m_roiMap;

};

} // end namespace

#endif
