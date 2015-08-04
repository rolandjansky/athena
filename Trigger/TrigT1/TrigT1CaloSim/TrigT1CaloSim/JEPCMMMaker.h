/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALO_JEPCMMMAKER_H
#define TRIGT1CALO_JEPCMMMAKER_H

#include <string>
#include <vector>

#include "DataModel/DataVector.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"


class StoreGateSvc;

namespace LVL1 {

/** Algorithm to make CMM-Jet hits and CMM-Energy sums containers.
 *
 *  Also makes JEM/CMM RoI containers and JEP collections for writing
 *  bytestream.
 *
 *  @author Peter Faulkner
 */

class JEPCMMMaker : public AthAlgorithm {

 public:
   JEPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~JEPCMMMaker();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:

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
