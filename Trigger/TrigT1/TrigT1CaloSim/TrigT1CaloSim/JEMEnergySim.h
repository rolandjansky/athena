
 /***************************************************************************
                           JEMEnergySim.h  -  description
                              -------------------
     begin                : Monday 12 May 2014
     copyright            : (C) 2014 by Alan Watson
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
 #include "StoreGate/StoreGateSvc.h"
 
 // LVL1 Calo Trigger
 #include "TrigT1CaloToolInterfaces/IL1EtTools.h"


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
   StatusCode beginRun() ;
   StatusCode execute() ;
   StatusCode finalize() ;

private: // Private attributes
  ToolHandle<LVL1::IL1EtTools> m_EtTool;

  /* StoreGate keys */
  std::string   m_JetElementLocation ;
  std::string   m_jemEtSumsLocation ;
  std::string   m_energyCMXDataLocation ;
  
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
 
