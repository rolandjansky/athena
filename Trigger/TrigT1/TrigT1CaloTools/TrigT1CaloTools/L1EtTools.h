/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1EtTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1ETTOOLS_H
#define LVL1L1ETTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1EtTools.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1CaloUtils/SystemEnergy.h"
#include "TrigT1CaloEvent/EnergyCMXData.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

namespace LVL1 
{

  /** @class L1EtTools

      This is which allows the user to simulate the ETmiss and ETsum
      triggers outside the TrigT1Calo package.

      Individual stages of the ET trigger chain (module, crate, system)
      can be run separately and their results analysed. Interfaces for
      running the later stages without needing to know about the earlier
      ones are also provided.

      Useage:

      To look at the intermediate results, run 3 tools in sequence:
        L1EtTools->moduleSums(DataVector<ModuleEnergy>);
        L1EtTools->crateSums(DataVector<CrateEnergy>, DataVector<ModuleEnergy>);
        SystemEnergy L1EtTools->systemSums(DataVector<CrateEnergy>);

        Where the ModuleEnergy, CrateEnergy and SystemEnergy objects gives
        access to the results at each stage (including, for the SystemEnergy,
        ETmiss/ETsum hits and RoI words).
        
      To just access Crate or System results:
        L1EtTools->crateSums(DataVector<CrateEnergy>);
      or
        SystemEnergy L1EtTools->systemSum();
      
      Note that these still run the earlier stages internally
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1EtTools : virtual public IL1EtTools, public AthAlgTool
    {
    public:
      
      L1EtTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1EtTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize();
      /** Returns a vector of LVL1::ModuleEnergy objects */
      virtual void moduleSums(const xAOD::JetElementContainer* jetelements,
                              DataVector<ModuleEnergy>* modules, int slice = -1) ;
      virtual void moduleSums(const std::map<int, xAOD::JetElement*>* jemap,
                              DataVector<ModuleEnergy>* modules, int slice = -1) ;
      /** Returns a vector of LVL1::CrateEnergy objects */
      virtual void crateSums(const DataVector<ModuleEnergy>* modules,
                             DataVector<CrateEnergy>* crates, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) ;
      virtual void crateSums(const DataVector<EnergyCMXData>* modules,
                             DataVector<CrateEnergy>* crates, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) ;
      /** Directly obtain LVL1::CrateEnergy objects */
      virtual void crateSums(const xAOD::JetElementContainer* jetelements,
                             DataVector<CrateEnergy>* crates, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) ;
      virtual void crateSums(const std::map<int, xAOD::JetElement*>* jemap,
                             DataVector<CrateEnergy>* crates, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) ;
      /** Returns a LVL1::SystemEnergy object */
      virtual SystemEnergy systemSums(const DataVector<CrateEnergy>* crates) ;
      /** Directly obtain a LVL1::SystemEnergy object */
      virtual SystemEnergy systemSums(const xAOD::JetElementContainer* jetelements, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) ;
      virtual SystemEnergy systemSums(const std::map<int, xAOD::JetElement*>* jemap, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) ;
            
    private:

      /** Return a map of JetElements */
      void mapJetElements(const xAOD::JetElementContainer* jetelements,
                          std::map<int, xAOD::JetElement*>* m_jeContainer);
      /** Fill masked out JetElements map */
      void fillMaskedOutMap();
      
      /** Thresholds to apply to JetElements in EtSum and EtMiss triggers */
      int m_jetElementThresholdEtSum;
      int m_jetElementThresholdEtMiss;
      
      /** Map of JetElements masked out of TE trigger */
      std::map<int, int>* m_TEMasks;
      
      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

    };
} // end of namespace

#endif 
