/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEMERRORS_H
#define TRIGT1CALOBYTESTREAM_JEMERRORS_H

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
}

namespace LVL1BS {

/** Algorithm to add errors to JEM data for testing bytestream converters
 *  and monitoring.
 *
 *
 *  @author Peter Faulkner
 */

class JemErrors : public AthAlgorithm {

 public:
   JemErrors(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~JemErrors();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   enum ErrorBits { Error1 = 0x1, Error2 = 0x2, Error3 = 0x4, Error4 = 0x8,
                    Error5 = 0x10, Error6 = 0x20, Error7 = 0x40, Error8 = 0x80,
                    Error9 = 0x100, Error10 = 0x200, Error11 = 0x400,
                    Error12 = 0x800, Error13 = 0x1000, Error14 = 0x2000,
                    Error15 = 0x4000, Error16 = 0x8000 };

   typedef DataVector<LVL1::JetElement> JetElementCollection;
   typedef DataVector<LVL1::JEMEtSums>  EnergySumsCollection;
   typedef DataVector<LVL1::CMXJetTob>  CmxJetTobCollection;
   typedef DataVector<LVL1::CMXJetHits> CmxJetHitsCollection;
   typedef DataVector<LVL1::CMXEtSums>  CmxEnergyCollection;
   typedef DataVector<LVL1::JEMTobRoI>  JemRoiCollection;

   /// Generate JetElement errors
   void jetElementErrors(const JetElementCollection* jeCollection,
                               JetElementCollection* errCollection);
   /// Generate JEMEtSums errors
   void jemEtSumsErrors(const EnergySumsCollection* etCollection,
                              EnergySumsCollection* errCollection);
   /// Generate CMXJetTob errors
   void cmxTobErrors(const CmxJetTobCollection* tobCollection,
                           CmxJetTobCollection* errCollection);
   /// Generate CMXJetHits errors
   void cmxHitsErrors(const CmxJetHitsCollection* hitCollection,
                            CmxJetHitsCollection* errCollection);

   /// Print a message when error generated
   void errorMessage(const std::string& msg);

   /// Jet element container input StoreGate key
   std::string m_jetElementLocation;
   /// Energy sums container input StoreGate key
   std::string m_jemEtSumsLocation;
   /// CMX TOB container input StoreGate key
   std::string m_cmxJetTobLocation;
   /// CMX hits container input StoreGate key
   std::string m_cmxJetHitsLocation;
   /// CMX energy sums container input StoreGate key
   std::string m_cmxEnergyLocation;
   /// JEM RoI container input StoreGate key
   std::string m_jemRoiLocation;
   /// CMX RoI container input StoreGate key
   std::string m_cmxRoiLocation;
   /// Jet element container output StoreGate key
   std::string m_jetElementLocationOut;
   /// Energy sums container output StoreGate key
   std::string m_jemEtSumsLocationOut;
   /// CMX TOB container output StoreGate key
   std::string m_cmxJetTobLocationOut;
   /// CMX hits container output StoreGate key
   std::string m_cmxJetHitsLocationOut;
   /// CMX energy sums container output StoreGate key
   std::string m_cmxEnergyLocationOut;
   /// JEM RoI container output StoreGate key
   std::string m_jemRoiLocationOut;
   /// CMX RoI container output StoreGate key
   std::string m_cmxRoiLocationOut;
   /// Jet element errors flag
   int m_jetElementErrors;
   /// Energy sums errors flag
   int m_jemEtSumsErrors;
   /// CMX TOB errors flag
   int m_cmxTobErrors;
   /// CMX hits errors flag
   int m_cmxHitsErrors;
   /// CMX energy sums errors flag
   int m_cmxEtSumsErrors;
   /// JEM RoI errors flag
   int m_jemRoiErrors;
   /// CMX RoI errors flag
   int m_cmxRoiErrors;

   /// Individual error done flags
   bool m_doneThisEvent;
   /// JetElement
   bool m_doneEmParity;
   bool m_doneHadParity;
   bool m_doneEmLinkDown;
   bool m_doneHadLinkDown;
   bool m_doneEmReadoutA;
   bool m_doneEmReadoutB;
   bool m_doneEmReadoutC;
   bool m_doneHadReadoutA;
   bool m_doneHadReadoutB;
   bool m_doneHadReadoutC;
   bool m_doneJeSubStatus;
   /// JEMEtSums
   bool m_doneJemEtReadoutA;
   bool m_doneJemEtReadoutB;
   bool m_doneJemEtReadoutC;
   bool m_doneJemExReadoutA;
   bool m_doneJemExReadoutB;
   bool m_doneJemExReadoutC;
   bool m_doneJemEyReadoutA;
   bool m_doneJemEyReadoutB;
   bool m_doneJemEyReadoutC;
   /// CMXJetTob
   bool m_doneCmxTobParity;
   bool m_doneTobLgReadoutA;
   bool m_doneTobLgReadoutB;
   bool m_doneTobLgReadoutC;
   bool m_doneTobSmReadoutA;
   bool m_doneTobSmReadoutB;
   bool m_doneTobSmReadoutC;
   /// CMXJetHits
   bool m_doneCmxHitsParity0;
   bool m_doneCmxHitsParity1;
   bool m_doneCmxHitsParity2;
   bool m_doneCmxHitsParity3;

};

} // end namespace

#endif
