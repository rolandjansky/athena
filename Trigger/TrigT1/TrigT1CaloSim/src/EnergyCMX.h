/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 /***************************************************************************
                           EnergyCMX.h  -  description
                              -------------------
      begin                : 04 04 2014
      email                : Alan.Watson@cern.ch
  ***************************************************************************/

 #ifndef ENERGYCMX_H
 #define ENERGYCMX_H

 // STL
 #include <string>
 #include <vector>
 #include <map>

 // Athena/Gaudi
 #include "AthenaBaseComps/AthReentrantAlgorithm.h"
 #include "GaudiKernel/ServiceHandle.h"  
 #include "GaudiKernel/ToolHandle.h"
 #include "AthContainers/DataVector.h"
 #include "StoreGate/ReadHandleKey.h"
 #include "StoreGate/WriteHandleKey.h"
 
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
 class EnergyCMX : public AthReentrantAlgorithm
 {
  typedef DataVector<EnergyCMXData> EnergyCMXDataCollection;
  typedef DataVector<CMXEtSums> CMXEtSumsCollection;
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

   virtual StatusCode initialize() override;
   virtual StatusCode start() override;
   virtual StatusCode execute(const EventContext& ctx) const override;

private: // Private attributes
  /* Service and tool handles */
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc {
    this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", "Service providing L1 menu thresholds"};
  ToolHandle<LVL1::IL1EtTools> m_EtTool {
    this, "L1EtTools", "LVL1::L1EtTools/L1EtTools", "Tool performing the simulation"};

  /* Input handles */
  SG::ReadHandleKey<EnergyCMXDataCollection> m_energyCMXDataLocation {
    this, "EnergyCMXDataLocation", TrigT1CaloDefs::EnergyCMXDataLocation,
    "Read handle key for EnergyCMXDataCollection"};

  /* Output handles */
  SG::WriteHandleKey<EnergyCTP> m_energyCTPLocation {
    this, "EnergyCTPLocation", TrigT1CaloDefs::EnergyCTPLocation,
    "Write handle key for EnergyCTP"};
  SG::WriteHandleKey<EnergyTopoData> m_energyTopoLocation {
    this, "EnergyTopoDataLocation", TrigT1CaloDefs::EnergyTopoDataLocation,
    "Write handle key for EnergyTopoData"};
  SG::WriteHandleKey<CMXEtSumsCollection> m_cmxEtsumsLocation {
    this, "CMXEtSumsLocation", TrigT1CaloDefs::CMXEtSumsLocation,
    "Write handle key for CMXEtSumsCollection"};
  SG::WriteHandleKey<CMXRoI> m_cmxRoILocation {
    this, "CMXRoILocation", TrigT1CaloDefs::CMXRoILocation,
    "Write handle key for CMXRoI"};

  TrigConf::L1DataDef m_def;
      
private: // Private methods
  
  /** find Trigger Menu set by CTP, and set internal TM values from it*/
  void setupTriggerMenuFromCTP();

  /** form CTP objects and store them in SG. */
  StatusCode saveCTPObjects(const SystemEnergy& resultsFull,
                            const SystemEnergy& resultsTrunc,
                            const EventContext& ctx) const;
  /** put EnergyRoIs into SG */
  StatusCode saveRoIs(const SystemEnergy& resultsFull,
                      const SystemEnergy& resultsTrunc,
                      const EventContext& ctx) const;
  
  /** returns the Energy CTP word */
  unsigned int ctpWord(unsigned int metSigPassed,
                       unsigned int etMissPassed,
                       unsigned int etSumPassed) const;
};

} // end of namespace bracket

#endif
 
