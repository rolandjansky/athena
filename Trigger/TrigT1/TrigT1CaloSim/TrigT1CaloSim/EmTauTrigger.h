/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


 /***************************************************************************
                           EmTauTrigger.h  -  description
                              -------------------
     begin                : Wed Dec 13 2000
     copyright            : (C) 2000 by moyse
     email                : moyse@heppch.ph.qmw.ac.uk
  ***************************************************************************/

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/
 #ifndef EmTauTrigger_H
 #define EmTauTrigger_H

 // STL
 #include <string>
 #include <vector>

 // Athena/Gaudi
 #include "GaudiKernel/Algorithm.h"
 #include "GaudiKernel/MsgStream.h"
 #include "GaudiKernel/ServiceHandle.h"
 #include "GaudiKernel/ToolHandle.h"
 
 #include "DataModel/DataVector.h"
 #include "GaudiKernel/DataSvc.h"
 #include "StoreGate/StoreGateSvc.h"

// Include for the configuration service:
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"

 // LVL1 Calo Trigger
 #include "TrigT1CaloEvent/EmTauROI.h"
 #include "TrigT1CaloEvent/CPMHits.h"
 #include "TrigT1CaloUtils/CPAlgorithm.h"
 #include "TrigT1CaloUtils/ClusterProcessorModuleKey.h"
 #include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"

 namespace LVL1 {

 using namespace TrigConf;

   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Em/tau calo trigger.
   */
 class EmTauTrigger : public Algorithm
 {

  public:
  typedef DataVector<CPMHits>                 CPMHitsCollection;

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   EmTauTrigger( const std::string& name, ISvcLocator* pSvcLocator ) ;

   virtual ~EmTauTrigger();


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode beginRun() ;
   StatusCode execute() ;
   StatusCode finalize() ;

 private: // Private methods
   /** create final ROI objects and place them in the TES. */
   void saveExternalROIs();
   /** Store CPM hit counts in CPMHits */
   void formCPMHits();
   /** put CPM summary objects into SG */
   void saveCPMHits();
  /** increment CPM hit word */
  unsigned int addHits(unsigned int hitMult, unsigned int hitVec);

  /** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
  void setupTriggerMenuFromCTP();
 private: // Private attributes

   /** These are our main output data */
   DataVector<EmTauROI>* m_vectorOfEmTauROIs;

   /** Where to find the TriggerTowers */
   std::string   m_TriggerTowerLocation ;
   /** Locations of outputs in StoreGate */
   std::string   m_emTauOutputLocation ;
   std::string    m_cpmHitsLocation ;

  /** These objects emulate the actual CP FPGA */
  DataVector<CPAlgorithm>* m_intROIContainer;
   
  /** This map holds the CPMHits objects - because a map
  provides a convenient way of finding a CPMHits if we need to update it */
  std::map<int, CPMHits *> * m_cpmHitsContainer;
  
   /** The essentials - data access, configuration, tools */
   ServiceHandle<StoreGateSvc> m_storeGate;
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   ToolHandle<LVL1::IL1EmTauTools> m_EmTauTool;

};

 } // end of namespace bracket


 #endif












