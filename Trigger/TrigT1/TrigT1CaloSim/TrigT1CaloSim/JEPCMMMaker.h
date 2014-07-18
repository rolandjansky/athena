/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALO_JEPCMMMAKER_H
#define TRIGT1CALO_JEPCMMMAKER_H

#include <string>
#include <vector>

#include "DataModel/DataVector.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloToolInterfaces/IL1JEPEtSumsTools.h"
#include "TrigT1CaloToolInterfaces/IL1JEPHitsTools.h"

class StoreGateSvc;

namespace LVL1 {

class CMMEtSums;
class CMMJetHits;
class CMMRoI;
class JEMEtSums;
class JEMHits;
class JEMRoI;
class JetElement;
class JetROI;


/** Algorithm to make CMM-Jet hits and CMM-Energy sums containers.
 *
 *  Also makes JEM/CMM RoI containers and JEP collections for writing
 *  bytestream.
 *
 *  @author Peter Faulkner
 */

class JEPCMMMaker : public Algorithm {

 public:
   JEPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~JEPCMMMaker();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:

   typedef DataVector<JetElement>    JetElementCollection;
   typedef DataVector<JetROI>        JetRoiCollection;
   typedef DataVector<JEMHits>       JetHitsCollection;
   typedef DataVector<JEMEtSums>     EnergySumsCollection;
   typedef DataVector<JEMRoI>        JemRoiCollection;
   typedef DataVector<CMMJetHits>    CmmJetCollection;
   typedef DataVector<CMMEtSums>     CmmEnergyCollection;

   /// Make CMM-Jet
   StatusCode makeCmmJet();

   /// Make CMM-Energy
   StatusCode makeCmmEnergy();

   /// Make JEM RoI container
   StatusCode makeJemRoi();

   /// Make CMM RoI container
   StatusCode makeCmmRoi();

   /// Make JEP bytestream container
   StatusCode makeJepBsCollection();

   /// Make JEP RoI bytestream container
   StatusCode makeJepRoiBsCollection();

   /// StoreGate service
   ServiceHandle<StoreGateSvc> m_storeGate;
   /// Tool for EtSums
   ToolHandle<LVL1::IL1JEPEtSumsTools> m_jepEtSumsTool;
   /// Tool for Hits
   ToolHandle<LVL1::IL1JEPHitsTools>   m_jepHitsTool;
   /// Jet elements container StoreGate key
   std::string m_jetElementLocation;
   /// Jet hits container StoreGate key
   std::string m_jemHitsLocation;
   /// Energy sums container StoreGate key
   std::string m_jemEtSumsLocation;
   /// CMM-Jet hits container StoreGate key
   std::string m_cmmJetLocation;
   /// CMM-Energy sums container StoreGate key
   std::string m_cmmEnergyLocation;
   /// Jet RoI StoreGate key
   std::string m_jetRoiLocation;
   /// JEM RoI StoreGate key
   std::string m_jemRoiLocation;
   /// CMM RoI StoreGate key
   std::string m_cmmRoiLocation;
   /// JEP bytestream container StoreGate key
   std::string m_jepBsCollectionLocation;
   /// JEP RoI bytestream container StoreGate key
   std::string m_jepRoiBsCollectionLocation;
   /// Simulation flag
   bool m_runSimulation;

};

} // end namespace

#endif
