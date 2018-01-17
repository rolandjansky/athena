
 /***************************************************************************
                           JEMJetSim.h  -  description
                              -------------------
     begin                : Wed Mar 12 2014
     copyright            : (C) 2014 Alan Watson
     email                : Alan.Watson@CERN.CH
  ***************************************************************************/

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
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
 #include "StoreGate/StoreGateSvc.h"

// Include for the configuration service:
 #include "TrigConfInterfaces/ILVL1ConfigSvc.h"

 // LVL1 Calo Trigger
 #include "TrigT1CaloToolInterfaces/IL1JEMJetTools.h"

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
   StatusCode beginRun() ;
   StatusCode execute() ;
   StatusCode finalize() ;

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
   std::string   m_JetElementLocation;
   /** Locations of outputs in StoreGate */
   std::string   m_JEMTobRoILocation;
   std::string   m_JetCMXDataLocation;
   
   /** The essentials - data access, configuration, tools */
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   ToolHandle<LVL1::IL1JEMJetTools> m_JetTool;

};

 } // end of namespace bracket


 #endif












