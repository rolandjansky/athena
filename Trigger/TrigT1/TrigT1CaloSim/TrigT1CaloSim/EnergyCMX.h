
 /***************************************************************************
                           EnergyCMX.h  -  description
                              -------------------
      begin                : 04 04 2014
      copyright            : (C) 2014 by Alan Watson
      email                : Alan.Watson@cern.ch
  ***************************************************************************/

 #ifndef ENERGYCMX_H
 #define ENERGYCMX_H

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
 #include "TrigT1CaloUtils/CrateEnergy.h"
 #include "TrigT1CaloUtils/SystemEnergy.h"
 #include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
 #include "TrigT1CaloEvent/EnergyCMXData_ClassDEF.h"
 #include "TrigT1Interfaces/EnergyCTP.h"
 #include "TrigT1CaloEvent/CMXEtSums_ClassDEF.h"
 #include "TrigT1CaloEvent/EnergyTopoData_ClassDEF.h"
 #include "TrigT1CaloEvent/CMXRoI.h"
 #include "TrigT1CaloToolInterfaces/IL1EtTools.h"
 #include "TrigT1Interfaces/TrigT1CaloDefs.h"
 #include "TrigConfL1Data/L1DataDef.h"


 namespace LVL1 {
   //using namespace TrigConf;

   //Doxygen class description below:
   /**
 This class simulates the missing ET and total ET triggers.
 EnergyCMX uses EnergyCrate and JetElement objects in order to closely follow
 the layout of the hardware.
   */
 class EnergyCMX : public AthAlgorithm
 {
  typedef DataVector<EnergyCMXData> EnergyCMXDataCollection;
  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   EnergyCMX( const std::string& name, ISvcLocator* pSvcLocator ) ;


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode beginRun() ;
   StatusCode execute() ;
   StatusCode finalize() ;

private: // Private attributes
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ToolHandle<LVL1::IL1EtTools> m_EtTool;

  /* StoreGate keys */
  std::string   m_energyCMXDataLocation ;
  std::string   m_energyRoILocation ;
  std::string   m_energyCTPLocation ;
  std::string   m_energyTopoLocation ;
  std::string   m_cmxEtsumsLocation ;
  std::string   m_cmxRoILocation ;
  
  /** SystemEnergy object returns results of ET trigger algorithms */
  SystemEnergy* m_resultsFull;
  SystemEnergy* m_resultsTrunc;

  TrigConf::L1DataDef m_def;
      
private: // Private methods
  
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
 
