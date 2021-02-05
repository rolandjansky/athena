/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 /***************************************************************************
                           JEMEnergySim.h  -  description
                              -------------------
     begin                : Monday 12 May 2014
     email                : Alan.Watson@CERN.CH
  ***************************************************************************/

 #ifndef JEMENERGYSIM_H
 #define JEMENERGYSIM_H

 // STL
 #include <string>
 #include <vector>
 #include <map>

 // Athena/Gaudi
 #include "AthenaBaseComps/AthAlgorithm.h"
 #include "GaudiKernel/ServiceHandle.h"  
 #include "GaudiKernel/ToolHandle.h"
 
 #include "AthContainers/DataVector.h"

 // Athena/Gaudi includes
 #include "GaudiKernel/DataSvc.h"
 
 // LVL1 Calo Trigger
 #include "TrigT1CaloToolInterfaces/IL1EtTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"


 namespace LVL1 {
 
 class ModuleEnergy;
 class JEMEtSums;
 class EnergyCMXData;

   //Doxygen class description below:
   /**
 This class simulates the missing ET and total ET triggers.
 JEMEnergySim uses EnergyCrate and JetElement objects in order to closely follow
 the layout of the hardware.
   */
 class JEMEnergySim : public AthAlgorithm
 {
  typedef DataVector<JEMEtSums>     JEMEtSumsCollection;
  typedef DataVector<EnergyCMXData> EnergyCMXDataCollection;
  
  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   JEMEnergySim( const std::string& name, ISvcLocator* pSvcLocator ) ;


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode execute() ;

private: // Private attributes
  ToolHandle<LVL1::IL1EtTools> m_EtTool;

  /* StoreGate keys */
  SG::ReadHandleKey<xAOD::JetElementContainer> m_JetElementInputKey { this, "JetElementLocation", TrigT1CaloDefs::JetElementLocation, "Input JetElement container" };
    
  std::string   m_jemEtSumsLocation ;
  SG::WriteHandleKey<DataVector<JEMEtSums> > m_jemEtSumsOutputKey { this, "JEMEtSumsLocation", TrigT1CaloDefs::JEMEtSumsLocation, "Output Jet energy sums container" };

  std::string   m_energyCMXDataLocation ;
  SG::WriteHandleKey<DataVector<EnergyCMXData> > m_energyCMXDataOutputKey { this, "EnergyCMXDataLocation", TrigT1CaloDefs::EnergyCMXDataLocation, "Output Jet CMX energy sum container" };
  
  /** vector of ModuleEnergy objects */
  DataVector<ModuleEnergy>* m_jemContainer;

private: // Private methods
  
  /** put results into SG */
  void storeJEMEtSums();
  void storeBackplaneData();

  /** delete pointers etc. */
  void cleanup();

};

} // end of namespace bracket

#endif
 
