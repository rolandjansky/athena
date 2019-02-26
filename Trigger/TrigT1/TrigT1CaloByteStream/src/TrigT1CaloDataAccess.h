/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_TRIGT1CALODATAACCESS_H
#define TRIGT1CALOBYTESTREAM_TRIGT1CALODATAACCESS_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloByteStream/ITrigT1CaloDataAccess.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"

class IInterface;
class IROBDataProviderSvc;
class StatusCode;

namespace LVL1 {
  class TriggerTower;
  class JetElement;
}

namespace LVL1BS {

class JepByteStreamV2Tool;
class IPpmByteStreamSubsetTool;
class ITriggerTowerSelectionTool;

/** Tool to retrieve TriggerTowers corresponding to a given eta/phi range
 *  from bytestream.
 *
 *  @author Peter Faulkner
 */

class TrigT1CaloDataAccess : virtual public ITrigT1CaloDataAccess,
              public AthAlgTool, virtual public IIncidentListener {

 public:
   TrigT1CaloDataAccess(const std::string& type, const std::string& name,
                        const IInterface* parent);
   virtual ~TrigT1CaloDataAccess();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Return iterators to required trigger towers
   virtual StatusCode loadCollection(
                      DataVector<LVL1::TriggerTower>::const_iterator& beg,
		      DataVector<LVL1::TriggerTower>::const_iterator& end,
		      double etaMin, double etaMax,
		      double phiMin, double phiMax,const bool full);

   virtual StatusCode loadCollection(
                      DataVector<LVL1::JetElement>::const_iterator& beg,
		      DataVector<LVL1::JetElement>::const_iterator& end);

   /** handle to init a new event */
   void handle (const Incident& );

 private:

   /// Data fetching service
   ServiceHandle<IROBDataProviderSvc>             m_robDataProvider;
   /// Tool for selections
   ToolHandle<LVL1BS::ITriggerTowerSelectionTool> m_selectionTool;
   /// Tool for bytestream conversion
   ToolHandle<LVL1BS::IPpmByteStreamSubsetTool>   m_ppmBSConverter;
   /// Tool for Jep bytestream conversion
   ToolHandle<LVL1BS::JepByteStreamV2Tool>   m_JetConverter;

   /// ROB fragment pointers
   std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> m_robFrags;
   /// Current TriggerTower sub-collection
   DataVector<LVL1::TriggerTower>* m_ttCol;
   /// Current JetElements sub-collection
   DataVector<LVL1::JetElement>* m_jetCol;
   // save loads of memory for re-use
   // Get PPM sub-block channel IDs for wanted TriggerTowers
   std::vector<unsigned int> m_chanIds;
   // Get ROB IDs for wanted TriggerTowers
   std::vector<uint32_t> m_robs;
   // Get PPM sub-block channel IDs for wanted TriggerTowers
   std::vector<unsigned int> m_chanIds_full;
   // Get ROB IDs for wanted TriggerTowers
   std::vector<uint32_t> m_robs_full;
   // Get ROB IDs for wanted JetElements
   std::vector<uint32_t> m_robs_full_je;
   // First time full runs, we should cache everything
   bool m_first;
   // Help caching
   unsigned int m_lC_Jet;
   // Present Event Number 
   unsigned int m_present_event;



};

} // end namespace

#endif
