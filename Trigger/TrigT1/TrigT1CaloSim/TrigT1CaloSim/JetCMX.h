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
 #include "AthenaBaseComps/AthAlgorithm.h"
 #include "GaudiKernel/ServiceHandle.h"
 #include "GaudiKernel/ToolHandle.h"
 
 #include "AthContainers/DataVector.h"
 #include "GaudiKernel/DataSvc.h"

 // Include for the configuration service:
 #include "TrigConfInterfaces/ILVL1ConfigSvc.h"

 // Outputs to CTP
 #include "TrigT1Interfaces/JetCTP.h"

 namespace LVL1 {

   //using namespace TrigConf;

 class JetCMXData;
 class JetCTP;

   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Em/tau calo trigger.
   */
 class JetCMX : public AthAlgorithm
 {

  typedef DataVector<LVL1::JetCMXData> t_jemDataContainer;

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

   StatusCode initialize() ;
   StatusCode execute() ;

 private: // Private methods
   /** Store CTP SLink data objects in the TES. */
//   void storeCTPData();
   /** Store L1Topo inputs in the TES */
//   void storeTopoTOBs();
   /** Store DAQ objects in the TES */
//   void storeReadoutObjects();

  /** Debug routine: dump trigger menu at start of run */
  void printTriggerMenu();
  
 private: // Private attributes

   /** Where to store the CMXCPHits (for CMX readout simulation) */
   std::string   m_CMXJetHitLocation;
   /** Where to store the CMXCPTobs (for CMX readout simulation) */
   std::string   m_CMXJetTobLocation;
   /** Locations of real-time outputs in StoreGate */
   std::string m_TopoOutputLocation ;
   std::string m_CTPOutputLocation ;
   /** Location of input data in StoreGate */
   std::string   m_JetCMXDataLocation;
   
   /** CTP Results */
   JetCTP* m_jetCTP;
   
   /** The essentials - data access, configuration, tools */
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   
};

 } // end of namespace bracket


 #endif












