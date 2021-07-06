/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JEMJetTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JEMJETTOOLS_H
#define LVL1L1JEMJETTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1JEMJetTools.h"
#include "TrigT1CaloEvent/JetInput.h"
#include "TrigT1CaloUtils/JetInputKey.h"
#include "TrigT1CaloUtils/JEMJetAlgorithm.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "xAODTrigL1Calo/JetElementContainer.h"

class AtlasDetectorID;
class Identifier;

namespace TrigConf {
  class L1Menu;
}

namespace LVL1 
{

  /** @class L1JEMJetTools

      This is a tool to reconstruct the L1 Jet trigger sums
      for a particular RoI location from the stored JetInputs.
      Used for offline monitoring and trigger reconstruction.
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1JEMJetTools : virtual public extends<AthAlgTool, IL1JEMJetTools>
    {
    public:
      
      L1JEMJetTools(const std::string&,const std::string&,const IInterface*);
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize() override;
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize() override;
      
      /** Convert user-supplied JetElements to map of JetInputs. Can specify which time slice of data to use */
      virtual void mapJetInputs(const xAOD::JetElementContainer* jes, std::map<int, JetInput*>* elements, int slice = -1) const override;

      /** Return vector of TOB RoI objects derived from user-specified inputs. JetElements can be multi-slice */
      virtual void findRoIs(const std::map<int, JetInput*>* elements, xAOD::JEMTobRoIContainer* rois) const override;
      virtual void findRoIs(const xAOD::JetElementContainer* jes, xAOD::JEMTobRoIContainer* rois, int slice = -1) const override;

      /** Return vector of RoI objects derived from user-specified inputs. JetElements can be multi-slice */
      virtual void findRoIs(const std::map<int, JetInput*>* elements, DataVector<JEMJetAlgorithm>* rois) const override;
      virtual void findRoIs(const xAOD::JetElementContainer* jes, DataVector<JEMJetAlgorithm>* rois, int slice = -1) const override;
      
      /** Form JEM results for specified crate/module using user-supplied map of input towers
          Adds to DataVector of JEMTobRoI and returns backplane data words*/
      virtual void findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                  xAOD::JEMTobRoIContainer* rois, std::vector<unsigned int>& jetCMXData) const override;
      /** Form JEM results for specified crate/module using user-supplied map of input towers
          Adds to DataVector of JEMTobRoI and returns backplane data words*/
      virtual void findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                DataVector<JEMTobRoI>* rois, std::vector<unsigned int>& jetCMXData) const override;

      /** Return RoI object for specified location */
      virtual JEMJetAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) const override;
                  
      /** Form jet cluster sums for a given RoI location */
      virtual JEMJetAlgorithm formSums(double RoIeta, double RoIphi,
                            const std::map<int, JetInput*>* elements) const override;
      virtual JEMJetAlgorithm formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) const override;
            
    private:
      
      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc{this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc"};
      Gaudi::Property<bool> m_useNewConfig { this, "UseNewConfig", true, "When true, read the menu from detector store, when false use the L1ConfigSvc" };
      const TrigConf::L1Menu * m_l1menu{nullptr};

      /** Utility for decoding RoI words */
      JEPRoIDecoder m_conv;

    }; 
} // end of namespace

#endif 
