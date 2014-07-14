/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMSUBSETTESTER_H
#define TRIGT1CALOBYTESTREAM_PPMSUBSETTESTER_H

#include <map>
#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

namespace LVL1 {
  class TriggerTower;
  class TriggerTowerKey;
}

namespace LVL1BS {

class ITrigT1CaloDataAccess;

/** Algorithm to test Trigger tower bytestream conversions.
 *
 *  Just prints out the contents of the TriggerTower objects.
 *  Run before writing bytestream and after reading it and compare.
 *
 *  @author Peter Faulkner
 */

class PpmSubsetTester : public AthAlgorithm {

 public:
   PpmSubsetTester(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~PpmSubsetTester();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:
   /// Trigger tower container
   typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;
   /// Trigger tower map
   typedef std::map<unsigned int, const LVL1::TriggerTower*> TriggerTowerMap;

   /// Print the trigger towers
   void printTriggerTowers() const;

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;

   /// Set up trigger tower map
   void setupTTMap(const TriggerTowerCollection* ttCollection,
                   double etaMin, double etaMax, double phiMin, double phiMax);
   /// Set up trigger tower map
   void setupTTMap(TriggerTowerCollection::const_iterator& beg,
                   TriggerTowerCollection::const_iterator& end);

   /// TT subset access tool
   ToolHandle<LVL1BS::ITrigT1CaloDataAccess> m_dataAccess;
   /// Trigger tower key provider
   LVL1::TriggerTowerKey* m_towerKey;
   /// Trigger tower container StoreGate key
   std::string m_triggerTowerLocation;
   /// Force number of LUT slices
   int m_forceSlicesLut;
   /// Force number of FADC slices
   int m_forceSlicesFadc;

   /// Trigger tower map
   TriggerTowerMap m_ttMap;

};

} // end namespace

#endif
