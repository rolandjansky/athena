/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 /***************************************************************************
                           JEMJetSim.h  -  description
                              -------------------
     begin                : Wed Mar 12 2014
     email                : Alan.Watson@CERN.CH
  ***************************************************************************/

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/
 #ifndef JEMJetSim_H
 #define JEMJetSim_H

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

 // LVL1 Calo Trigger
 #include "TrigT1CaloToolInterfaces/IL1JEMJetTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

 namespace LVL1 {

   //using namespace TrigConf;
 
 class JetCMXData;
 class JEMTobRoI;


   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Jet trigger.
   */
 class JEMJetSim : public AthAlgorithm
 {

  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   JEMJetSim( const std::string& name, ISvcLocator* pSvcLocator ) ;


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode execute() ;

 private: // Private methods
   /** Store TOB RoI objects in the TES. */
   void storeModuleRoIs();
   /** Store module outputs in TES as inputs to CMX simulation */
   void storeBackplaneTOBs();

  /** Debug routine: dump trigger menu at start of run */
  void printTriggerMenu();
  
 private: // Private attributes

   /** TOB RoIs for RoIB input and DAQ output simulation */
   DataVector<JEMTobRoI>* m_allTOBs;

   /** Backplane data objects: CPM outputs to CMX */
   DataVector<JetCMXData>* m_JetCMXData;

   /** Where to find the JetElements */
   SG::ReadHandleKey<xAOD::JetElementContainer> m_JetElementInputKey { this, "JetElementLocation", TrigT1CaloDefs::JetElementLocation, "Input JetElement container" };

   /** Locations of outputs in StoreGate */
   SG::WriteHandleKey<DataVector<JEMTobRoI> > m_JEMTobRoIOutputKey { this, "JEMTOBRoILocation", TrigT1CaloDefs::JEMTobRoILocation, "Output Jet ROI container" };
   SG::WriteHandleKey<DataVector<JetCMXData> > m_JetCMXDataOutputKey { this, "JetCMXDataLocation", TrigT1CaloDefs::JetCMXDataLocation, "Output Jet CMX data container" };
   
   /** The essentials - data access, configuration, tools */
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   ToolHandle<LVL1::IL1JEMJetTools> m_JetTool;

};

 } // end of namespace bracket


 #endif












