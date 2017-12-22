
 /***************************************************************************
                           CPCMX.h  -  description
                              -------------------
     begin                : Mon Jul 28 2014
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
 #ifndef CPCMX_H
 #define CPCMX_H

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

 // Outputs to CTP
 #include "TrigT1Interfaces/EmTauCTP.h"
 #include "TrigT1Interfaces/JetCTP.h"

 namespace LVL1 {

   //using namespace TrigConf;

 class CPMCMXData;

   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Em/tau calo trigger.
   */
 class CPCMX : public AthAlgorithm
 {

  typedef DataVector<LVL1::CPMCMXData> t_cpmDataContainer;

  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   CPCMX( const std::string& name, ISvcLocator* pSvcLocator ) ;

   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode beginRun() ;
   StatusCode execute() ;
   StatusCode finalize() ;

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
   std::string   m_CMXCPHitLocation;
   /** Where to store the CMXCPTobs (for CMX readout simulation) */
   std::string   m_CMXCPTobLocation;
   /** Locations of real-time outputs in StoreGate */
   std::string m_TopoOutputLocation ;
   std::string m_CTPOutputLocation ;
   /** Location of input data in StoreGate */
   std::string   m_CPMCMXDataLocation;
   
   /** The essentials - data access, configuration, tools */
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   
   /** CTP info*/
   EmTauCTP* m_emTauCTP;

   /** Topo format parameters */
   static const int s_SourceLocal = 3;
   static const int s_SourceTotal = 4;

};

 } // end of namespace bracket


 #endif












