
 /***************************************************************************
                           CPMSim.h  -  description
                              -------------------
     begin                : Wed Mar 05 2014
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
 #ifndef CPMSim_H
 #define CPMSim_H

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
 
 #include "TrigT1Interfaces/TrigT1CaloDefs.h"

// Include for the configuration service:
 #include "TrigConfInterfaces/ILVL1ConfigSvc.h"

 // LVL1 Calo Trigger
 #include "TrigT1CaloToolInterfaces/IL1CPMTools.h"

 // For RoI output 
 #include "TrigT1Interfaces/SlinkWord.h"


 namespace LVL1 {

   //using namespace TrigConf;

 class CPMTower;
 class CPMTobRoI;
 class CPMCMXData;

   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Em/tau calo trigger.
   */
 class CPMSim : public AthAlgorithm
 {

  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   CPMSim( const std::string& name, ISvcLocator* pSvcLocator ) ;

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
   /** Simulate Slink data for RoIB input */
  void storeSlinkObjects();
  
  /** adds slink header */
  void addHeader( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int subDetID, unsigned int moduleId);
  /** add Slink tail */
  void addTail( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int numberOfDataWords);
  /** creates a new SlinkWord object with the passed word, and returns a pointer.*/
  LVL1CTP::SlinkWord* getWord(unsigned int tword);

  /** Debug routine: dump trigger menu at start of run */
  void printTriggerMenu();
  
 private: // Private attributes

   /** CPM Towers (input to algorithm, output to BS simulation) */
   //DataVector<xAOD::CPMTower>* m_cpmTowers;
   
   /** TOB RoIs for RoIB input and DAQ output simulation */
   DataVector<CPMTobRoI>* m_allTOBs;

   /** Backplane data objects: CPM outputs to CMX */
   DataVector<CPMCMXData>* m_CMXData;
   
   /** there are 4 CP RoI RODs which have a Slink cable connected to the RoIB.
    This array holds pointers to 4 DataVectors containing the Slink words  */
   DataVector<LVL1CTP::SlinkWord>* m_CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs];

   /** Where to store the CPMTowers */
   std::string   m_CPMTowerLocation;
   /** Locations of outputs in StoreGate */
   std::string   m_CPMTobRoILocation;
   std::string   m_CPMCMXDataLocation;
   std::string   m_emTauSlinkLocation ;
   
   /** The essentials - data access, configuration, tools */
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   ToolHandle<LVL1::IL1CPMTools> m_CPMTool;
   
   unsigned int m_eventNumber;


};

 } // end of namespace bracket


 #endif












