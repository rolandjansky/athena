/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1CPMTowerTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1CPMTOWERTOOLS_H
#define LVL1L1CPMTOWERTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1CPMTowerTools.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1 
{

  /** @class L1CPMTowerTools

      This is a tool to form CPMTowers from TriggerTowers or to
      copy CPMTowers into a standard map, using keys from
      TrigT1Utils::TriggerTowerKey
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1CPMTowerTools : virtual public IL1CPMTowerTools, public AthAlgTool
    {
    public:
      typedef DataVector<TriggerTower>          TTCollection ;
      typedef DataVector<xAOD::TriggerTower>    xAODTTCollection ;
      
      L1CPMTowerTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1CPMTowerTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /** Fill DataVector of CPMTowers from user-supplied TriggerTowers */
      virtual void makeCPMTowers(const DataVector<TriggerTower>* triggerTowers, DataVector<CPMTower>* cpmTowers, bool zeroSuppress = true);

      /** Fill DataVector of CPMTowers from user-supplied xAOD::TriggerTowers */
      virtual void makeCPMTowers(const DataVector<xAOD::TriggerTower>* triggerTowers, DataVector<CPMTower>* cpmTowers, bool zeroSuppress = true);

      /** Fill DataVector of CPMTowers from user-supplied xAOD::TriggerTowers */
      virtual void makeCPMTowers(const DataVector<xAOD::TriggerTower>* triggerTowers, DataVector<xAOD::CPMTower>* cpmTowers, bool zeroSuppress = true);
            
    private:

             
    }; 
} // end of namespace

#endif 
