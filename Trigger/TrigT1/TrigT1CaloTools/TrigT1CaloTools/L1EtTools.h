///////////////////////////////////////////////////////////////////
// L1EtTools.h, (c) Alan Watson
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
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
                              DataVector<ModuleEnergy>* modules, int slice = -1) const;
      virtual void moduleSums(const xAOD::JetElementMap_t* jemap,
                              DataVector<ModuleEnergy>* modules, int slice = -1) const;
      /** Returns a vector of LVL1::CrateEnergy objects */
      virtual void crateSums(const DataVector<ModuleEnergy>* modules,
                             DataVector<CrateEnergy>* crates, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false) const;
      virtual void crateSums(const DataVector<EnergyCMXData>* modules,
                             DataVector<CrateEnergy>* crates, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false) const;
      /** Directly obtain LVL1::CrateEnergy objects */
      virtual void crateSums(const xAOD::JetElementContainer* jetelements,
                             DataVector<CrateEnergy>* crates, int slice = -1, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false) const;
      virtual void crateSums(const xAOD::JetElementMap_t* jemap,
                             DataVector<CrateEnergy>* crates, int slice = -1, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false) const;
      /** Returns a LVL1::SystemEnergy object */
      virtual SystemEnergy systemSums(const DataVector<CrateEnergy>* crates) const;
      /** Directly obtain a LVL1::SystemEnergy object */
      virtual SystemEnergy systemSums(const xAOD::JetElementContainer* jetelements, int slice = -1, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false) const;
      virtual SystemEnergy systemSums(const xAOD::JetElementMap_t* jemap, int slice = -1, float etaMaxXE = 5., float etaMaxTE = 5., bool restricted = false) const;
            
    private:

      /** Return a map of JetElements */
      void mapJetElements(const xAOD::JetElementContainer* jetelements,
                          xAOD::JetElementMap_t* m_jeContainer) const;
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
