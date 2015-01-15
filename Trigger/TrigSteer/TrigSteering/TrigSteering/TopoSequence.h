/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : TopoSequence
 *
 * @brief  one sequence node, i.e.
 *         input TE type(s) -> PESA Algorithm(s) -> output TE type
 *
 * @author Ingrid Deigaard 
 *
 * File and Version Information:
 * $Id: TopoSequence.h 639471 2015-01-14 15:15:28Z stelzer $
 **********************************************************************************/

#ifndef TRIGSTEERING_TOPOSEQUENCE_H
#define TRIGSTEERING_TOPOSEQUENCE_H

#include <vector>
#include <map>
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/Sequence.h"
#include "L1TopoCoreSim/TopoSteering.h"

class TrigTimer;

namespace Trig {
  class TrigDecisionTool;
}

namespace HLT {
   class Algo;
   class AlgoConfig;
   class TriggerElement;

   class TopoSequence: public Sequence
   {

   public:

      TopoSequence(HLT::AlgoConfig* config, unsigned int outputType,TCS::TopoSteering *ts);

      ~TopoSequence(){};


      HLT::ErrorCode execute(); //!< execute this sequece if not done before
    
      void setRoi2TEMap( std::map<uint32_t, TriggerElement*> const * const mapping) { m_roiWord2TEMapping = mapping; }

   private:
    
      TriggerElement * getTEFromRoiWord(uint32_t roiWord) const;

      TCS::TopoSteering *m_topoSteer{ nullptr };
      std::string m_name { "" };

      std::map<uint32_t, TriggerElement*> const *    m_roiWord2TEMapping { nullptr };

   };
}

#endif
