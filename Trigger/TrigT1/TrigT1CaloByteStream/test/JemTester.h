/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEMTESTER_H
#define TRIGT1CALOBYTESTREAM_JEMTESTER_H

#include <map>
#include <string>
#include <stdint.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class CMMEtSums;
  class CMMJetHits;
  class CMMRoI;
  class JEMEtSums;
  class JEMHits;
  class JEMRoI;
  class JetElement;
  class JetElementKey;
}

namespace LVL1BS {

/** Algorithm to test JEM component bytestream conversions.
 *
 *  Just prints out the contents of the JetElement objects,
 *  JEMHits objects and JEMEtSums objects.
 *  Now also includes CMMJetHits, CMMEtSums, JEMRoI and CMMRoI.
 *
 *  Run before writing bytestream and after reading it and compare.
 *
 *  @author Peter Faulkner
 */

class JemTester : public AthAlgorithm {

 public:
   JemTester(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~JemTester();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   typedef DataVector<LVL1::JetElement>              JetElementCollection;
   typedef DataVector<LVL1::JEMHits>                 JetHitsCollection;
   typedef DataVector<LVL1::JEMEtSums>               EnergySumsCollection;
   typedef DataVector<LVL1::CMMJetHits>              CmmJetCollection;
   typedef DataVector<LVL1::CMMEtSums>               CmmEnergyCollection;
   typedef DataVector<LVL1::JEMRoI>                  JemRoiCollection;
   typedef std::map<unsigned int, const LVL1::JetElement*> JetElementMap;
   typedef std::map<int, const LVL1::JEMHits*>       JetHitsMap;
   typedef std::map<int, const LVL1::JEMEtSums*>     EnergySumsMap;
   typedef std::map<int, const LVL1::CMMJetHits*>    CmmHitsMap;
   typedef std::map<int, const LVL1::CMMEtSums*>     CmmSumsMap;
   typedef std::map<uint32_t, const LVL1::JEMRoI*>   JemRoiMap;

   /// Print the jet elements
   void printJetElements(const std::string& source) const;
   /// Print the jet hits
   void printJetHits()    const;
   /// Print the energy sums
   void printEnergySums() const;
   /// Print the CMM hits
   void printCmmHits()    const;
   /// Print the CMM energy sums
   void printCmmSums()    const;
   /// Print the JEM RoIs
   void printJemRois(const std::string& source) const;
   /// Print the CMM RoIs
   void printCmmRois(const std::string& source,
                     const LVL1::CMMRoI* roi)   const;

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;
   /// Print a vector (unsigned)
   void printVecU(const std::vector<unsigned int>& vec) const;

   /// Set up jet element map
   void setupJeMap(const JetElementCollection* jeCollection);
   /// Set up jet hits map
   void setupHitsMap(const JetHitsCollection* hitCollection);
   /// Set up energy sums map
   void setupEtMap(const EnergySumsCollection* etCollection);
   /// Set up CMM hits map
   void setupCmmHitsMap(const CmmJetCollection* hitCollection);
   /// Set up CMM energy sums map
   void setupCmmEtMap(const CmmEnergyCollection* etCollection);
   /// Set up JEM RoI map
   void setupJemRoiMap(const JemRoiCollection* jrCollection);

   /// Jet element key provider
   LVL1::JetElementKey* m_elementKey;
   /// Jet element container StoreGate key
   std::string m_jetElementLocation;
   /// Jet hits container StoreGate key
   std::string m_jemHitsLocation;
   /// Energy sums container StoreGate key
   std::string m_jemEtSumsLocation;
   /// CMM hits container StoreGate key
   std::string m_cmmJetLocation;
   /// CMM energy sums container StoreGate key
   std::string m_cmmEnergyLocation;
   /// JEM RoI container StoreGate key
   std::string m_jemRoiLocation;
   /// CMM RoI container StoreGate key
   std::string m_cmmRoiLocation;
   /// JEM RoI from RoIB container StoreGate key
   std::string m_jemRoiLocationRoib;
   /// CMM RoI from RoIB container StoreGate key
   std::string m_cmmRoiLocationRoib;
   /// Jet element overlap container StoreGate key
   std::string m_jetElementLocationOverlap;
   /// Force number of JEM slices
   int m_forceSlicesJem;
   /// Force number of CMM slices
   int m_forceSlicesCmm;
   /// Jet element print flag
   int m_jetElementPrint;
   /// Jet hits print flag
   int m_jemHitsPrint;
   /// Energy sums print flag
   int m_jemEtSumsPrint;
   /// CMM hits print flag
   int m_cmmHitsPrint;
   /// CMM energy sums print flag
   int m_cmmEtSumsPrint;
   /// JEM RoI print flag
   int m_jemRoiPrint;
   /// CMM RoI print flag
   int m_cmmRoiPrint;
   /// JEM RoI from RoIB print flag
   int m_jemRoiPrintRoib;
   /// CMM RoI from RoIB print flag
   int m_cmmRoiPrintRoib;
   /// Jet element overlap print flag
   int m_jetElementPrintOverlap;

   /// Jet element map
   JetElementMap m_jeMap;
   /// Jet hits map
   JetHitsMap    m_hitsMap;
   /// Energy sums map
   EnergySumsMap m_etMap;
   /// CMM hits map
   CmmHitsMap    m_cmmHitsMap;
   /// CMM energy sums map
   CmmSumsMap    m_cmmEtMap;
   /// JEM RoI map
   JemRoiMap     m_roiMap;

};

} // end namespace

#endif
