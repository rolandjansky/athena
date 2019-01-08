/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1TopoDataMaker.h, 
///////////////////////////////////////////////////////////////////


#ifndef L1TOPODATAMAKER_H
#define L1TOPODATAMAKER_H

#include "AthContainers/DataVector.h"

namespace ROIB {
   class RoIBResult;
   class EMTauResult;
   class JetEnergyResult;
}

namespace LVL1 
{
   class CPCMXTopoData;
   class JetCMXTopoData;
   class EnergyTopoData;

   /** @class L1TopoDataMaker

   This is a tool to reconstruct the CMX -> Topo simulation objects
   from the RoIBResult. 
      
   @author  Alan Watson <Alan.Watson@cern.ch>
   */  

   class L1TopoDataMaker {
   public:
        
      L1TopoDataMaker();

      /** default destructor */
      virtual ~L1TopoDataMaker ();
      
      /** Fill DataVector of CPCMXTopoData from RoIBResult */
      virtual void makeCPCMXTopoData(const ROIB::RoIBResult* roibResult, DataVector<CPCMXTopoData>* topoData) const;
      /** Fill DataVector of CPCMXTopoData from RoIBResult */
      virtual void makeCPCMXTopoData(const std::vector<ROIB::EMTauResult> & roibData, DataVector<CPCMXTopoData>* topoData) const;
      
      /** Fill DataVector of JetCMXTopoData from RoIBResult */
      virtual void makeJetCMXTopoData(const ROIB::RoIBResult* roibResult, DataVector<JetCMXTopoData>* topoData) const;
      /** Fill DataVector of JetCMXTopoData from RoIBResult */
      virtual void makeJetCMXTopoData(const std::vector<ROIB::JetEnergyResult> & roibData, DataVector<JetCMXTopoData>* topoData) const;
       
      /** Fill EnergyTopoData from RoIBResult */
      virtual void makeEnergyTopoData(const ROIB::RoIBResult* roibResult, EnergyTopoData* topoData) const;

      /** Fill EnergyTopoData from RoIBResult */
      virtual void makeEnergyTopoData(const std::vector<ROIB::JetEnergyResult> & roibData, EnergyTopoData* topoData) const;
            
   private:
                   
   }; 
} // end of namespace

#endif 
