/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 /***************************************************************************
                           JetCMX.h  -  description
                              -------------------
     begin                : Mon Jul 28 2014
     email                : Alan.Watson@CERN.CH
  ***************************************************************************/

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/
 #ifndef JetCMX_H
 #define JetCMX_H

 // STL
 #include <string>
 #include <vector>

 // Athena/Gaudi
 #include "AthenaBaseComps/AthReentrantAlgorithm.h"
 #include "GaudiKernel/ServiceHandle.h"
 #include "GaudiKernel/ToolHandle.h"
 #include "AthContainers/DataVector.h"
 #include "StoreGate/ReadHandleKey.h"
 #include "StoreGate/WriteHandleKey.h"

 // Include for the configuration service:
 #include "TrigConfInterfaces/ILVL1ConfigSvc.h"

 // Input/output types
 #include "TrigT1Interfaces/JetCTP.h"
 #include "TrigT1Interfaces/TrigT1CaloDefs.h"
 #include "TrigT1CaloEvent/CMXJetHits_ClassDEF.h"
 #include "TrigT1CaloEvent/CMXJetTob_ClassDEF.h"
 #include "TrigT1CaloEvent/JetCMXData_ClassDEF.h"
 #include "TrigT1CaloEvent/JetCMXTopoData_ClassDEF.h"

 namespace LVL1 {

   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Em/tau calo trigger.
   */
 class JetCMX : public AthReentrantAlgorithm
 {

  typedef DataVector<CMXJetHits> CMXJetHitsCollection;
  typedef DataVector<CMXJetTob> CMXJetTobCollection;
  typedef DataVector<JetCMXTopoData> JetCMXTopoDataCollection;
  typedef DataVector<LVL1::JetCMXData> JetCMXDataCollection;

  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   JetCMX( const std::string& name, ISvcLocator* pSvcLocator ) ;

   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   virtual StatusCode initialize() override;
   virtual StatusCode execute(const EventContext& ctx) const override;

 private: // Private methods

  /** Debug routine: dump trigger menu at start of run */
  void printTriggerMenu() const;
  
 private: // Private attributes
   /* Service and tool handles */
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc {
     this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", "Service providing L1 menu thresholds"};

   /* Input handles */
   SG::ReadHandleKey<JetCMXDataCollection> m_JetCMXDataLocation {
     this, "JetCMXDataLocation", TrigT1CaloDefs::JetCMXDataLocation,
     "Read handle key for JetCMXData collection"};

   /* Output handles */
   SG::WriteHandleKey<CMXJetHitsCollection> m_CMXJetHitsLocation {
     this, "CMXJetHitsLocation", TrigT1CaloDefs::CMXJetHitsLocation,
     "Write handle key for CMXJetHits collection"};
   SG::WriteHandleKey<CMXJetTobCollection> m_CMXJetTobLocation {
     this, "CMXJetTobLocation", TrigT1CaloDefs::CMXJetTobLocation,
     "Write handle key for CMXJetTob collection"};
   SG::WriteHandleKey<JetCMXTopoDataCollection> m_TopoOutputLocation {
     this, "TopoOutputLocation", TrigT1CaloDefs::JetTopoTobLocation,
     "Write handle key for JetCMXTopoData collection"};
   SG::WriteHandleKey<JetCTP> m_CTPOutputLocation {
     this, "CTPOutputLocation", TrigT1CaloDefs::JetCTPLocation,
     "Write handle key for JetCTP object"};
};

 } // end of namespace bracket


 #endif












