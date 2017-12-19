///////////////////////////////////////////////////////////////////
// L1JetElementTools.h, (c) Alan Watson
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JETELEMENTTOOLS_H
#define LVL1L1JETELEMENTTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1 
{

  /** @class L1JetElementTools

      This is a tool to reconstruct the L1 Jet trigger sums
      for a particular RoI location from the stored JetInputs.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1JetElementTools->mapTowers() // set up event for analysis
              L1JetElementTools->formSums(eta, phi) // for each RoI
              L1JetElementTools->ET8x8() // etc. Return values
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1JetElementTools : virtual public IL1JetElementTools, public AthAlgTool
    {
    public:
      
      L1JetElementTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1JetElementTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /** Fill DataVector of JetElements from user-supplied TriggerTowers */
      virtual void makeJetElements(const xAOD::TriggerTowerContainer* tts, xAOD::JetElementContainer* jetElements) const;
      virtual void mapJetElements(const xAOD::JetElementContainer* jetElements, xAOD::JetElementMap_t* map) const;

      // @DEPRICATED(amazurov)
      virtual void makeJetElements(const DataVector<TriggerTower>* tts, DataVector<JetElement>* jetElements) const;
            
    private:

     /** Trigger Tower and Jet Element properties */
     static const int m_ttSat = 0xFF;
     static const int m_layerSat = 0x1FF;
             
    }; 
} // end of namespace

#endif 
