/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1CPMTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1CPMTOOLS_H
#define LVL1L1CPMTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1CPMTools.h"
#include "TrigT1CaloUtils/CPMTobAlgorithm.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1
{

  /** @class L1CPMTools

      This is a tool to reconstruct the L1 EM/tau trigger sums
      for a particula]
      r RoI location from the stored TriggerTowers.
      Used for offline monitoring and trigger reconstruction.

      Usage: L1CPMTools->mapTowers() // set up event for analysis
             L1CPMTools->formSums(eta, phi) // for each RoI
             L1CPMTools->EmClus() // etc. Return values
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1CPMTools : virtual public IL1CPMTools, public AthAlgTool
    {
    public:
      typedef DataVector<xAOD::CPMTower>         CPMTCollection ;
      typedef std::map<int, xAOD::CPMTower*>*    CPMTContainer;
      
      L1CPMTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1CPMTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize() override;
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  () override;
      
      /** Tool-based EM/tau algorithm implementation */
      
      /** Map CPMTowers from user-supplied DataVector of inputs */
      virtual void mapTowers(const DataVector<xAOD::CPMTower>* cpmts, xAOD::CPMTowerMap_t* towers) const override;
       
      /** Find RoIs in user-supplied map of input towers */
      virtual void findRoIs(const xAOD::CPMTowerMap_t* towers, DataVector<CPMTobAlgorithm>* tobs, int slice = -1) const override;
      
      /** Find RoIs in user-supplied DataVector of input towers */
      virtual void findRoIs(const DataVector<xAOD::CPMTower>* cpmts, DataVector<CPMTobAlgorithm>* tobs, int slice = -1) const override;
     
      /** Form CPM results for specified crate/module using user-supplied map of input towers
          This version fills a vector of CPMTobRoI for the whole detector */
      virtual void findCPMTobRoIs(const xAOD::CPMTowerMap_t* towers, xAOD::CPMTobRoIContainer* rois, int slice = -1) const override;
     
      /** Form CPM results for specified crate/module using user-supplied vector of input towers
          This version fills a vector of CPMTobRoI for the whole detector */
      virtual void findCPMTobRoIs(const DataVector<xAOD::CPMTower>* cpmts, xAOD::CPMTobRoIContainer* rois, int slice = -1) const override;
      
      /** Form CPM results for specified crate/module using user-supplied map of input towers
          This version adds to DataVector of CPMTobRoI and returns backplane data words*/
      virtual void findCPMResults(const xAOD::CPMTowerMap_t* towers, int crate, int module,
                                  DataVector<CPMTobRoI>* rois, std::vector<unsigned int>& emCMXData,
                                  std::vector<unsigned int>& tauCMXData, int slice = -1) const override;
      
      /** Return RoI object for a specified location */
      virtual CPMTobAlgorithm findRoI(double RoIeta, double RoIphi, const xAOD::CPMTowerMap_t* towers, int slice = -1)  const override;
      
      /** Form cluster/isolation sums for a given RoI location */
      virtual void formSums(double RoIeta, double RoIphi, const xAOD::CPMTowerMap_t* towers, int slice = -1)  override;
      virtual void formSums(uint32_t roiWord, const xAOD::CPMTowerMap_t* towers, int slice = -1) override;
      /** Methods to return the cluster/isolation ET values */
      virtual int Core() const override;
      virtual int EMCore() const override;
      virtual int HadCore() const override;
      virtual int EMClus() const override;
      virtual int TauClus() const override;
      virtual int EMIsol() const override;
      virtual int HadIsol() const override;
      virtual bool isEtMax() const override;
      virtual bool isEMRoI() const override;
      virtual bool isTauRoI() const override; 
      virtual unsigned int EMIsolWord() const override;
      virtual unsigned int TauIsolWord() const override;
            
    private:
      
      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;


     /** member variables for algorithm properties: */

      /** Utility for decoding RoI words */
      CPRoIDecoder m_conv;

      /** This object contains all information on specified trigger window */
      CPMTobAlgorithm* m_RoI;

    };
} // end of namespace

#endif 
