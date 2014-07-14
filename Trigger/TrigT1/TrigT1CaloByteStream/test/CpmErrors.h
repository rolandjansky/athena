/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMERRORS_H
#define TRIGT1CALOBYTESTREAM_CPMERRORS_H

#include <string>
#include <stdint.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class CPMTower;
  class CPMTobRoI;
  class CMXCPTob;
  class CMXCPHits;
}

namespace LVL1BS {

/** Algorithm to add errors to CPM data for testing bytestream converters
 *  and monitoring.
 *
 *  NB. Doesn't pretent to be rigorous so read back data may not be identical
 *  to written (eg. SubStatus errors in single CPMTower on output will be in
 *  multiple CPMTowers on subsequent input, ie. all from the relevant sub-block).
 *
 *
 *  @author Peter Faulkner
 */

class CpmErrors : public AthAlgorithm {

 public:
   CpmErrors(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~CpmErrors();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   enum ErrorBits { Error1 = 0x1, Error2 = 0x2, Error3 = 0x4, Error4 = 0x8,
		    Error5 = 0x10, Error6 = 0x20, Error7 = 0x40, Error8 = 0x80,
		    Error9 = 0x100, Error10 = 0x200, Error11 = 0x400,
		    Error12 = 0x800, Error13 = 0x1000, Error14 = 0x2000,
		    Error15 = 0x4000, Error16 = 0x8000 };

   typedef DataVector<LVL1::CPMTower>  CpmTowerCollection;
   typedef DataVector<LVL1::CMXCPTob>  CmxCpTobCollection;
   typedef DataVector<LVL1::CMXCPHits> CmxCpHitsCollection;
   typedef DataVector<LVL1::CPMTobRoI> CpmTobRoiCollection;
   
   /// Generate CPMTowers errors
   void cpmTowerErrors(const CpmTowerCollection* cpCollection,
                             CpmTowerCollection* errCollection);
   /// Generate CMXCPTob errors
   void cmxTobErrors(const CmxCpTobCollection* tobCollection,
                           CmxCpTobCollection* errCollection);
   /// Generate CMXCPHits errors
   void cmxHitsErrors(const CmxCpHitsCollection* hitCollection,
                            CmxCpHitsCollection* errCollection);
   /// Generate CPM TOB RoI errors
   void cpmTobRoiErrors(const CpmTobRoiCollection* crCollection,
                              CpmTobRoiCollection* errCollection);

   /// Print a message when error generated
   void errorMessage(const std::string& msg);

   /// CP element container input StoreGate key
   std::string m_cpmTowerLocation;
   /// CMX TOB container input StoreGate key
   std::string m_cmxCpTobLocation;
   /// CMX hits container input StoreGate key
   std::string m_cmxCpHitsLocation;
   /// CPM TOB RoI container input StoreGate key
   std::string m_cpmTobRoiLocation;
   /// CP element container output StoreGate key
   std::string m_cpmTowerLocationOut;
   /// CMX TOB container output StoreGate key
   std::string m_cmxCpTobLocationOut;
   /// CMX hits container output StoreGate key
   std::string m_cmxCpHitsLocationOut;
   /// CPM TOB RoI container output StoreGate key
   std::string m_cpmTobRoiLocationOut;
   /// CPM towers errors flag
   int m_cpmTowerErrors;
   /// CMX TOB errors flag
   int m_cmxTobErrors;
   /// CMX hits errors flag
   int m_cmxHitsErrors;
   /// CPM TOB RoI errors flag
   int m_cpmTobRoiErrors;

   /// Individual error done flags
   bool m_doneThisEvent;
   /// CPMTowers
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
   bool m_doneCpmSubStatus;
   /// CMXCPTob
   bool m_doneCmxTobParityM;
   bool m_doneCmxTobParity0;
   bool m_doneCmxTobParity1;
   bool m_doneCmxTobParity2;
   bool m_doneCmxTobParity3;
   bool m_doneTobEnReadoutA;
   bool m_doneTobEnReadoutB;
   bool m_doneTobEnReadoutC;
   bool m_doneTobIsReadoutA;
   bool m_doneTobIsReadoutB;
   bool m_doneTobIsReadoutC;
   /// CMXCPHits
   bool m_doneCmxHitsParity0;
   bool m_doneCmxHitsParity1;
   /// CPMTobRoI

   static const int s_saturation = 255;

};

} // end namespace

#endif
