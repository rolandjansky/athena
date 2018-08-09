/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

      Useage: L1CPMTools->mapTowers() // set up event for analysis
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
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Tool-based EM/tau algorithm implementation */
      
      /** Map CPMTowers from user-supplied DataVector of inputs */
      virtual void mapTowers(const DataVector<xAOD::CPMTower>* cpmts, std::map<int, xAOD::CPMTower*>* towers);
       
      /** Find RoIs in user-supplied map of input towers */
      virtual void findRoIs(const std::map<int, xAOD::CPMTower*>* towers, DataVector<CPMTobAlgorithm>* tobs, int slice = -1);
      
      /** Find RoIs in user-supplied DataVector of input towers */
      virtual void findRoIs(const DataVector<xAOD::CPMTower>* cpmts, DataVector<CPMTobAlgorithm>* tobs, int slice = -1);
     
      /** Form CPM results for specified crate/module using user-supplied map of input towers
          This version fills a vector of CPMTobRoI for the whole detector */
      virtual void findCPMTobRoIs(const std::map<int, xAOD::CPMTower*>* towers, xAOD::CPMTobRoIContainer* rois, int slice = -1);
     
      /** Form CPM results for specified crate/module using user-supplied vector of input towers
          This version fills a vector of CPMTobRoI for the whole detector */
      virtual void findCPMTobRoIs(const DataVector<xAOD::CPMTower>* cpmts, xAOD::CPMTobRoIContainer* rois, int slice = -1);
      
      /** Form CPM results for specified crate/module using user-supplied map of input towers
          This version adds to DataVector of CPMTobRoI and returns backplane data words*/
      virtual void findCPMResults(const std::map<int, xAOD::CPMTower*>* towers, int crate, int module,
                                  DataVector<CPMTobRoI>* rois, std::vector<unsigned int>& emCMXData,
                                  std::vector<unsigned int>& tauCMXData, int slice = -1);
      
      /** Return RoI object for a specified location */
      virtual CPMTobAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, xAOD::CPMTower*>* towers, int slice = -1) ;
      
      /** Form cluster/isolation sums for a given RoI location */
      virtual void formSums(double RoIeta, double RoIphi, const std::map<int, xAOD::CPMTower*>* towers, int slice = -1) ;
      virtual void formSums(uint32_t roiWord, const std::map<int, xAOD::CPMTower*>* towers, int slice = -1) ;
      /** Methods to return the cluster/isolation ET values */
      virtual int Core() const;
      virtual int EMCore() const;
      virtual int HadCore() const;
      virtual int EMClus() const;
      virtual int TauClus() const;
      virtual int EMIsol() const;
      virtual int HadIsol() const;
      virtual bool isEtMax() const;  
      virtual bool isEMRoI() const;  
      virtual bool isTauRoI() const;  
      virtual unsigned int EMIsolWord() const;
      virtual unsigned int TauIsolWord() const;
            
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
