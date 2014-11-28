///////////////////////////////////////////////////////////////////
// L1CPMTowerTools.h, (c) Alan Watson
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1CPMTOWERTOOLS_H
#define LVL1L1CPMTOWERTOOLS_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
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

  class L1CPMTowerTools : virtual public IL1CPMTowerTools, public AlgTool
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
            
    private:
      
      /** class member version of retrieving MsgStream */
      mutable MsgStream   m_log;
      int                 m_outputlevel;
             
    }; 
} // end of namespace

#endif 
