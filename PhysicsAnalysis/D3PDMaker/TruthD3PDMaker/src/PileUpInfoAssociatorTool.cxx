/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/PileUpInfoFillerTool.cxx
 * @author Jacob Searcy <jsearcy1@uoregon.edu>
 * @date Jul, 2010
 * @brief Block Associator tool to aid in looping over PileUpInfo Container for @c PileUpInfo information.
*/



#include "PileUpInfoAssociatorTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

 
PileUpInfoAssociatorTool::PileUpInfoAssociatorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : Base(type,name,parent)
    //m_pevt_time(0),
    //m_pevt_index(0)
{}

StatusCode PileUpInfoAssociatorTool::reset(const PileUpEventInfo &p){
    m_it  = p.beginSubEvt();
    m_end = p.endSubEvt();
    return StatusCode::SUCCESS;
  }

  const PileUpEventInfo::SubEvent* PileUpInfoAssociatorTool::next()
  {    
    if(m_it == m_end) return 0;
    const PileUpEventInfo::SubEvent* out=&(*m_it);
    ++m_it;
    return out;
  }
    }
