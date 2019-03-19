/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**                                                                            
 * @file TruthD3PDMaker/src/PileUpInfoFillerTool.cxx
 * @author Jacob Searcy <jsearcy1@uoregon.edu>
 * @date Jul, 2010
 * @brief Block Associator tool to aid in looping over PileUpInfo Container for @c PileUpInfo information. 
 */


#ifndef TRUTHD3P3MAKER_PILEUPINFOASSOCIATORTOOL_H
#define TRUTHD3P3MAKER_PILEUPINFOASSOCIATORTOOL_H

#include "EventInfo/PileUpEventInfo.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"

namespace D3PD {

  class PileUpInfoAssociatorTool : public MultiAssociationTool<PileUpEventInfo,PileUpEventInfo::SubEvent>
    {
    public:
      typedef MultiAssociationTool<PileUpEventInfo,PileUpEventInfo::SubEvent> Base;

      PileUpInfoAssociatorTool(const std::string& type, const std::string& name, const IInterface* parent);

      virtual StatusCode reset(const PileUpEventInfo &p);
      virtual const PileUpEventInfo::SubEvent* next();


    private:
      PileUpEventInfo::SubEvent::const_iterator m_it;
      PileUpEventInfo::SubEvent::const_iterator m_end;
      //long int* m_pevt_time;
      //long unsigned  int* m_pevt_index;
    };
} 

#endif
