/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


 /***************************************************************************
                           EnergyTrigger.h  -  description
                              -------------------
     begin                : Wed Dec 13 2000
     copyright            : (C) 2000 by moyse
     email                : moyse@heppch.ph.qmw.ac.uk
  ***************************************************************************/

 #ifndef ENERGYTRIGGER_H
 #define ENERGYTRIGGER_H

 // STL
 #include <string>
 #include <vector>
 #include <map>

 // Athena/Gaudi
 #include "GaudiKernel/Algorithm.h"
 #include "GaudiKernel/ServiceHandle.h"  
 #include "GaudiKernel/ToolHandle.h"
 
 #include "DataModel/DataVector.h"

 // Athena/Gaudi includes
 #include "GaudiKernel/DataSvc.h"
 #include "StoreGate/StoreGateSvc.h"
 
 // Include for the configuration service:
 #include "TrigConfInterfaces/ITrigConfigSvc.h"
 #include "TrigConfL1Data/CTPConfig.h"
 #include "TrigConfL1Data/Menu.h"
 #include "TrigConfL1Data/TriggerThreshold.h"
 #include "TrigConfL1Data/TriggerThresholdValue.h"
 #include "TrigConfL1Data/L1DataDef.h"

 // LVL1 Calo Trigger
 #include "TrigT1CaloUtils/ModuleEnergy.h"
 #include "TrigT1CaloUtils/CrateEnergy.h"
 #include "TrigT1CaloUtils/SystemEnergy.h"
 #include "TrigT1CaloEvent/EnergyRoI.h"
 #include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
 #include "TrigT1Interfaces/EnergyCTP.h"
 #include "TrigT1CaloEvent/JEMEtSums.h"
 #include "TrigT1CaloToolInterfaces/IL1EtTools.h"


 namespace LVL1 {
 using namespace TrigConf;

   //Doxygen class description below:
   /**
 This class simulates the missing ET and total ET triggers.
 EnergyTrigger uses EnergyCrate and JetElement objects in order to closely follow
 the layout of the hardware.
   */
 class EnergyTrigger : public Algorithm
 {
  typedef DataVector<JEMEtSums> JEMEtSumsCollection;
  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   EnergyTrigger( const std::string& name, ISvcLocator* pSvcLocator ) ;

   virtual ~EnergyTrigger();


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode beginRun() ;
   StatusCode execute() ;
   StatusCode finalize() ;

private: // Private attributes
  /** Pointer to SG service*/
  ServiceHandle<StoreGateSvc> m_storeGate;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ToolHandle<LVL1::IL1EtTools> m_EtTool;

  /* StoreGate keys */
  std::string   m_JetElementLocation ;
  std::string   m_jemEtSumsLocation ;
  std::string   m_energyRoILocation ;
  std::string   m_energyCTPLocation ;
  
  /** vector of ModuleEnergy objects */
  DataVector<ModuleEnergy>* m_jemContainer;

  /** SystemEnergy object returns results of ET trigger algorithms */
  SystemEnergy* m_results;
  
  /**  this map holds the JEMEtSums objects, allowing easy
       location of any particular JEMEtSums  */
  std::map<int, JEMEtSums *> * m_jemEtSumsContainer;

  /**pointer to energy RoI object which will eventually be passed to SG*/
  EnergyRoI* m_energyRoI;
  /**pointer to energy CTP object which will eventually be passed to SG*/
  EnergyCTP* m_energyCTP;
  
  /** Trigger configuration parameters */
  L1DataDef m_def;
    
private: // Private methods
  
  /** create JEM summary objects for bytestream encoding */
  void formJEMEtSums();

  /** delete pointers etc. */
  void cleanup();
  
  /** find Trigger Menu set by CTP, and set internal TM values from it*/
  void setupTriggerMenuFromCTP();

  /** form CTP objects and store them in SG. */
  void saveCTPObjects();
  /** put EnergyRoIs into SG */
  void saveRoIs();
  /** put EnergyRoIs into SG */
  void saveJEMEtSums();
  
  /** returns the Energy CTP word */
  unsigned int ctpWord(unsigned int metSigPassed, unsigned int etMissPassed, unsigned int etSumPassed);
};

} // end of namespace bracket

#endif
 
