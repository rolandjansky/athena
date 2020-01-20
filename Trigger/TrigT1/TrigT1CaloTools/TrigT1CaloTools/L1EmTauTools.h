/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1EmTauTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1EMTAUTOOLS_H
#define LVL1L1EMTAUTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1 
{

  /** @class L1EmTauTools

      This is a tool to reconstruct the L1 EM/tau trigger sums
      for a particula]
      r RoI location from the stored TriggerTowers.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1EmTauTools->mapTowers() // set up event for analysis
              L1EmTauTools->formSums(eta, phi) // for each RoI
              L1EmTauTools->EmClus() // etc. Return values
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1EmTauTools : virtual public IL1EmTauTools, public AthAlgTool
    {
    public:
      typedef DataVector<TriggerTower>         TTCollection ;
      typedef std::map<int, CPMTower*>*        CPMTContainer;
      
      L1EmTauTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1EmTauTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Tool-based EM/tau algorithm implementation */
      /** Find RoIs in user-supplied map of input towers */
      virtual void findRoIs(const CPMTowerMap_t* towers, DataVector<CPAlgorithm>* rois, int slice = -1);
      virtual void findRoIs(const TriggerTowerMap_t* tts, DataVector<CPAlgorithm>* rois, int slice = -1);
      /** Find RoIs in user-supplied DataVector of input towers */
      virtual void findRoIs(const DataVector<CPMTower>* cpmts, DataVector<CPAlgorithm>* rois, int slice = -1);
      virtual void findRoIs(const DataVector<TriggerTower>* tts, DataVector<CPAlgorithm>* rois, int slice = -1);
      
      /** Map CPMTowers from user-supplied DataVector of inputs */
      virtual void mapTowers(const DataVector<TriggerTower>* tts, CPMTowerMap_t* towers);
      virtual void mapTowers(const DataVector<CPMTower>* cpmts, CPMTowerMap_t* towers);
      /** Map of CPMTowers from user-supplied map of TriggerTowers */
      virtual void mapTowers(const TriggerTowerMap_t* tts, CPMTowerMap_t* towers);
      
      /** Return RoI object for a specified location */
      virtual CPAlgorithm findRoI(double RoIeta, double RoIphi, const CPMTowerMap_t* towers, int slice = -1) ;
      
      /** Form cluster/isolation sums for a given RoI location */
      virtual void formSums(double RoIeta, double RoIphi, const CPMTowerMap_t* towers, int slice = -1) ;
      virtual void formSums(uint32_t roiWord, const CPMTowerMap_t* towers, int slice = -1) ;
      /** Methods to return the cluster/isolation ET values */
      virtual int Core() const;
      virtual int EMClus() const;
      virtual int TauClus() const;
      virtual int EMIsol() const;
      virtual int HadIsol() const;
      virtual int HadCore() const;
      virtual bool isEtMax() const;  
      virtual unsigned int Hits() const;
      virtual unsigned int RoIWord() const;
            
    private:
      
      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;


     /** member variables for algorithm properties: */

      /** Utility for decoding RoI words */
      CPRoIDecoder m_conv;

     /** This object contains all information on specified trigger window */
     CPAlgorithm* m_RoI;

    };
} // end of namespace

#endif 
