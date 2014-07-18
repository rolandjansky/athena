/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


 /***************************************************************************
                           JetTrigger.h  -  description
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

 #ifndef JETTRIGGER_H
 #define JETTRIGGER_H

 // STL
 #include <string>
 #include <vector>
 
 // Athena/Gaudi
 #include "GaudiKernel/Algorithm.h"
 #include "GaudiKernel/ServiceHandle.h"    
 #include "GaudiKernel/ToolHandle.h"
 #include "GaudiKernel/MsgStream.h"
 
 #include "DataModel/DataVector.h"
 #include "StoreGate/StoreGateSvc.h"
 
// Include for the configuration service:
 #include "TrigConfInterfaces/ITrigConfigSvc.h"
 #include "TrigConfL1Data/CTPConfig.h"
 #include "TrigConfL1Data/Menu.h"
 #include "TrigConfL1Data/TriggerThreshold.h"
 #include "TrigConfL1Data/TriggerThresholdValue.h"
 #include "TrigConfL1Data/JetThresholdValue.h"

 // LVL1 Calo Trigger
 #include "TrigT1CaloUtils/JetAlgorithm.h"
 #include "TrigT1CaloEvent/JetROI.h"
 #include "TrigT1CaloEvent/JEMHits.h"
 #include "TrigT1CaloToolInterfaces/IL1JetTools.h"

namespace LVL1 {

using namespace TrigConf;

   //Doxygen class description below:
   /**
   JetTrigger Simulates the hardware jet trigger by reading in JetElements 
   and the TriggerMenu (produced by TrigT1Config) and forms a vector of JetInputs
  for use in the jet trigger. FCAL splitting, and optional JetElement ET
  thresholding, are performed when forming JetInputs. JetInputs are only
  used as local objects within JetTrigger, i.e. not placed in StoreGate.
   */
 class JetTrigger : public Algorithm
 {

  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   JetTrigger( const std::string& name, ISvcLocator* pSvcLocator ) ;

   virtual ~JetTrigger();


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
   /** Store JEM hit counts in JEMHits */
   void formJEMHits();
   /** put JEM summary objects into SG */
   void saveJEMHits();
   /** find Trigger Menu set by CTP, for debug purposes*/
   void setupTriggerMenuFromCTP();  
  /** increment JEM hit word */
  unsigned int addHits(unsigned int hitMult, unsigned int hitVec, bool forward);
  
 private: // Private attributes
  
  typedef DataVector<JEMHits> JEMHitsCollection;

  ServiceHandle<StoreGateSvc> m_storeGate;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ToolHandle<LVL1::IL1JetTools> m_JetTool;

  DataVector<JetROI>* m_vectorOfJetROIs;

  /* StoreGate keys */
  std::string   m_JetElementLocation ;
  std::string   m_jetROIOutputLocation ;
  std::string   m_jemHitsLocation ;

  /** a map object that holds pointers to the "internal" ROIs,
  that is ROI objects that are not put into TES.
  The ROIs each have a key, in this case an integer
  that defines them. */
  DataVector<JetAlgorithm>* m_intROIContainer;
  
  /** Finally this map holds the JEMHits objects - because a map
  provides a convenient way of finding a JEMHits if we need to update it */
  std::map<int, JEMHits *> * m_jemHitsContainer;
 };

 } // end of namespace bracket

 #endif
 
