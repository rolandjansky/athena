/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JEPEtSumsTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JEPETSUMSTOOLS_H
#define LVL1L1JEPETSUMSTOOLS_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1CaloToolInterfaces/IL1EtTools.h"
#include "TrigT1CaloToolInterfaces/IL1JEPEtSumsTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"


namespace LVL1 
{
  //using namespace xAOD;

class CMMEtSums;
class JEMEtSums;
class JetElement;
class ModuleEnergy;
class CrateEnergy;
class SystemEnergy;

  /** @class L1JEPEtSumsTools

      This is a tool to reconstruct the L1 JEM and CMM-Energy Et sums
      from Jet Elements.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1JEPEtSumsTools->formJEMEtSums() // Form JEMEtSums
                                                //   from JetElements
              L1JEPEtSumsTools->formCMMEtSums() // Form CMMEtSums
	                                        //   from JEMEtSums
      
      @author  Peter Faulkner
  */  

class L1JEPEtSumsTools : virtual public IL1JEPEtSumsTools, public AthAlgTool
{
  public:
     
    L1JEPEtSumsTools(const std::string& type, const std::string& name,
                                            const IInterface* parent);

     /** default destructor */
    virtual ~L1JEPEtSumsTools ();
      
     /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
     /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
      
     /** form JEMEtSums from JetElements */
    virtual void formJEMEtSums(const DataVector<xAOD::JetElement>* jetElementVec,
                               DataVector<JEMEtSums>* jemEtSumsVec) const;
     /** form complete CMMEtSums from JEMEtSums */
    virtual void formCMMEtSums(const DataVector<JEMEtSums>* jemEtSumsVec,
                               DataVector<CMMEtSums>* cmmEtSumsVec) const;
     /** form partial CMMEtSums (module) from JEMEtSums */
    virtual void formCMMEtSumsModule(const DataVector<JEMEtSums>* jemEtSumsVec,
                               DataVector<CMMEtSums>* cmmEtSumsMod) const;
     /** form partial CMMEtSums (crate) from module CMMEtSums */
    virtual void formCMMEtSumsCrate(const DataVector<CMMEtSums>* cmmEtSumsMod,
                               DataVector<CMMEtSums>* cmmEtSumsCrate) const;
     /** form partial CMMEtSums (system) from crate CMMEtSums */
    virtual void formCMMEtSumsSystem(
                               const DataVector<CMMEtSums>* cmmEtSumsCrate,
                               DataVector<CMMEtSums>* cmmEtSumsSys) const;
     /** form partial CMMEtSums (sumEt/missingEt maps) from system CMMEtSums */
    virtual void formCMMEtSumsEtMaps(const DataVector<CMMEtSums>* cmmEtSumsSys,
                               DataVector<CMMEtSums>* cmmEtSumsMap) const;
      
  private:

    typedef std::vector<unsigned int>              EnergyVector;
    typedef std::vector<int>                       ErrorVector;
    typedef std::vector<DataVector<ModuleEnergy>*> MultiSliceModuleEnergy;
    typedef std::vector<DataVector<CrateEnergy>*>  MultiSliceCrateEnergy;
    typedef std::vector<SystemEnergy*>             MultiSliceSystemEnergy;

    /** Convert CMMEtSums container to internal ModuleEnergy containers */
    void etSumsToModuleEnergy(const DataVector<CMMEtSums>* etSums,
                       MultiSliceModuleEnergy& modulesVec, int& peak) const;
    /** Convert CMMEtSums container to internal CrateEnergy containers */
    void etSumsToCrateEnergy(const DataVector<CMMEtSums>* etSums,
                       MultiSliceCrateEnergy& crateVec, int& peak) const;
    /** Convert CMMEtSums container to internal SystemEnergy objects */
    void etSumsToSystemEnergy(const DataVector<CMMEtSums>* etSums,
                       MultiSliceSystemEnergy& systemVec, int& peak) const;
    /** Convert internal ModuleEnergy containers to JEMEtSums container */
    void moduleEnergyToEtSums(const MultiSliceModuleEnergy& modulesVec,
                       DataVector<JEMEtSums>* jemEtSumsVec, int peak) const;
    /** Convert internal CrateEnergy containers to CMMEtSums container */
    void crateEnergyToEtSums(const MultiSliceCrateEnergy& cratesVec,
                       DataVector<CMMEtSums>* cmmEtSumsVec, int peak) const;
    /** Convert internal SystemEnergy objects to CMMEtSums object */
    void systemEnergyToEtSums(const MultiSliceSystemEnergy& systemVec,
                       DataVector<CMMEtSums>* cmmEtSumsVec, int peak) const;
    /** Convert maps from internal SystemEnergy objects to CMMEtSums objects */
    void etMapsToEtSums(const MultiSliceSystemEnergy& systemVec,
                       DataVector<CMMEtSums>* cmmEtSumsVec, int peak) const;

    /** trigger configuration service */
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
    /** Tool for JetElement map */
    ToolHandle<LVL1::IL1JetElementTools> m_jeTool;
    /** Tool for energy sums */
    ToolHandle<LVL1::IL1EtTools> m_etTool;
     
};

} // end of namespace

#endif 
