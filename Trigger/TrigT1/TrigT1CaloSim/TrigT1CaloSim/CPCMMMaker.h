/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALO_CPCMMMAKER_H
#define TRIGT1CALO_CPCMMMAKER_H

#include <string>
#include <vector>

#include "DataModel/DataVector.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloToolInterfaces/IL1CPHitsTools.h"

class StoreGateSvc;

namespace LVL1 {

class CMMCPHits;
class CPMHits;
class CPMTower;
class CPMRoI;
class EmTauROI;


/** Algorithm to make CMM-CP hits container.
 *
 *  Also makes CPM RoI container and CP collection to write bytestream.
 *
 *  @author Peter Faulkner
 */

class CPCMMMaker : public Algorithm {

 public:
   CPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~CPCMMMaker();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:

   typedef DataVector<CPMTower>        CPMTowerCollection;
   typedef DataVector<CPMHits>         CPMHitsCollection;
   typedef DataVector<CMMCPHits>       CMMHitsCollection;
   typedef DataVector<CPMRoI>          CPMRoICollection;
   typedef DataVector<EmTauROI>        EmTauROICollection;

   /// Make CMM-CP
   StatusCode makeCmmCp();

   /// Make CPM RoI container
   StatusCode makeCpmRoi();

   /// Make CP bytestream container
   StatusCode makeCpBsCollection();

   /// StoreGate service
   ServiceHandle<StoreGateSvc> m_storeGate;
   /// Tool for Hits
   ToolHandle<LVL1::IL1CPHitsTools>   m_cpHitsTool;
   /// CPM towers container StoreGate key
   std::string m_cpmTowerLocation;
   /// CPM hits container StoreGate key
   std::string m_cpmHitsLocation;
   /// CMM-CP hits container StoreGate key
   std::string m_cmmHitsLocation;
   /// CPM RoI container StoreGate key
   std::string m_cpmRoiLocation;
   /// EmTauROI container StoreGate key
   std::string m_emTauRoiLocation;
   /// CP bytestream container StoreGate key
   std::string m_cpBsCollectionLocation;
   /// Simulation flag
   bool m_runSimulation;

};

} // end namespace

#endif
