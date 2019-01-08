/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1ENERGYCMXTOOLS_H
#define LVL1L1ENERGYCMXTOOLS_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
//#include "TrigConfigSvc/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigT1CaloToolInterfaces/IL1EtTools.h"
#include "TrigT1CaloToolInterfaces/IL1EnergyCMXTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"

#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"

namespace LVL1
{
class ModuleEnergy;
class CrateEnergy;
class SystemEnergy;

/** @class L1EnergyCMXTools

      This is a tool to reconstruct the L1 JEM and CMX-Energy Et sums
      from Jet Elements.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1EnergyCMXTools->formJEMEtSums() // Form JEMEtSums
                                                //   from JetElements
              L1EnergyCMXTools->formCMXEtSums() // Form CMXEtSums
	                                        //   from JEMEtSums
      
      @author  Peter Faulkner
  */

class L1EnergyCMXTools : virtual public IL1EnergyCMXTools, public AthAlgTool
{
  public:
    L1EnergyCMXTools(const std::string &type, const std::string &name,
                     const IInterface *parent);

    /** default destructor */
    virtual ~L1EnergyCMXTools();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize();

    /** form JEMEtSums from JetElements */
    virtual void formJEMEtSums(const xAOD::JetElementContainer *jetElementVec,
                               xAOD::JEMEtSumsContainer *jemEtSumsVec) const;
    /** form complete CMXEtSums from JEMEtSums */
    virtual void formCMXEtSums(const xAOD::JEMEtSumsContainer *jemEtSumsVec,
                               xAOD::CMXEtSumsContainer *cmxEtSumsVec) const;
    /** form partial CMXEtSums (module) from JEMEtSums */
    virtual void formCMXEtSumsModule(const xAOD::JEMEtSumsContainer *jemEtSumsVec,
                                     xAOD::CMXEtSumsContainer *cmxEtSumsMod) const;
    /** form partial CMXEtSums (crate) from module CMXEtSums */
    virtual void formCMXEtSumsCrate(const xAOD::CMXEtSumsContainer *cmxEtSumsMod,
                                    xAOD::CMXEtSumsContainer *cmxEtSumsCrate) const;

    /** form partial CMXEtSums (system) from crate CMXEtSums */
    virtual void formCMXEtSumsSystem(
        const xAOD::CMXEtSumsContainer *cmxEtSumsCrate,
        xAOD::CMXEtSumsContainer *cmxEtSumsSys) const;
    /** form partial CMXEtSums (sumEt/missingEt maps) from system CMXEtSums */
    virtual void formCMXEtSumsEtMaps(const xAOD::CMXEtSumsContainer *cmxEtSumsSys,
                                     xAOD::CMXEtSumsContainer *cmxEtSumsMap) const;

  private:
    typedef std::vector<uint16_t> EnergyVector;
    typedef std::vector<uint32_t> ErrorVector;
    typedef std::vector<DataVector<ModuleEnergy> *> MultiSliceModuleEnergy;
    typedef std::vector<DataVector<CrateEnergy> *> MultiSliceCrateEnergy;
    typedef std::vector<SystemEnergy *> MultiSliceSystemEnergy;

    /** Convert CMXEtSums container to internal ModuleEnergy containers */
    void etSumsToModuleEnergy(const xAOD::CMXEtSumsContainer *etSums,
                              MultiSliceModuleEnergy &modulesVec, int &peak) const;
    /** Convert CMXEtSums container to internal CrateEnergy containers */
    void etSumsToCrateEnergy(const xAOD::CMXEtSumsContainer *etSums,
                             MultiSliceCrateEnergy &crateVecFull, MultiSliceCrateEnergy &crateVecRestricted,
                             int &peak) const;
    /** Convert CMXEtSums container to internal SystemEnergy objects */
    void etSumsToSystemEnergy(const xAOD::CMXEtSumsContainer *etSums,
                              MultiSliceSystemEnergy &systemVec, int &peak) const;
    /** Convert internal ModuleEnergy containers to JEMEtSums container */
    void moduleEnergyToEtSums(const MultiSliceModuleEnergy &modulesVec,
                              xAOD::JEMEtSumsContainer *jemEtSumsVec, int peak) const;
    /** Convert internal CrateEnergy containers to CMXEtSums container */
    void crateEnergyToEtSums(const MultiSliceCrateEnergy &cratesVec,
                             xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak) const;
    void crateEnergyToEtSums(
        const MultiSliceCrateEnergy &cratesVecFull, const MultiSliceCrateEnergy &cratesVecRestricted,
        xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak) const;

    /** Convert internal SystemEnergy objects to CMXEtSums object */
    void systemEnergyToEtSums(const MultiSliceSystemEnergy &systemVec,
                              xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak, xAOD::CMXEtSums_v1::Sources source = xAOD::CMXEtSums::TOTAL_STANDARD) const;
    /** Convert maps from internal SystemEnergy objects to CMXEtSums objects */
    void etMapsToEtSums(const MultiSliceSystemEnergy &systemVec,
                        xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak) const;
    void findRestrictedEta(uint32_t &maskXE, uint32_t &maskTE) const;

    void dumpCrateEnergies(const std::string& msg, const MultiSliceCrateEnergy& crates) const;
    /** trigger configuration service */
    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
    /** Tool for JetElement map */
    ToolHandle<LVL1::IL1JetElementTools> m_jeTool;
    /** Tool for energy sums */
    ToolHandle<LVL1::IL1EtTools> m_etTool;
    /** Debug flag */
    bool m_debug;
    /** Find restructed eta range.
     *  This will use the min/max values for the first valid threshold in the range 9-16 to define the ranges
     */
    uint32_t m_maskXE;
    uint32_t m_maskTE;
};

} // end of namespace

#endif
