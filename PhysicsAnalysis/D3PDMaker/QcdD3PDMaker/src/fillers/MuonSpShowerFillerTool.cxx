/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file QcdD3PDMaker/src/fillers/MuonSpShowerFillerTool.cxx
 * @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
 * @date Jan., 2011
 * @brief Block filler tool for MuonSpShower
 */

#include "QcdD3PDMaker/fillers/MuonSpShowerFillerTool.h"
#include "muonEvent/MuonSpShower.h"
#include "muonEvent/MuonSpShowerContainer.h"

#include "AthenaKernel/errorcheck.h"


namespace QcdD3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonSpShowerFillerTool::MuonSpShowerFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : D3PD::BlockFillerTool<Rec::MuonSpShower> (type, name, parent),
      m_storeGate("StoreGateSvc", name),
      m_doPosition(false),
      m_doHits(false),
      m_doSegments(false)
{
  m_doPosition = true;
  m_doHits = true;
  m_doSegments = true;
  book().ignore(); // Avoid coverity warnings.

  declareProperty("WritePosition",    m_doPosition    = false);
  declareProperty("WriteHits", m_doHits = false);
  declareProperty("WriteSegments", m_doSegments = false);
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode MuonSpShowerFillerTool::book()
{
  if(m_doPosition){
    CHECK( addVariable ("eta",  m_eta)  );
    CHECK( addVariable ("phi",  m_phi)  );
  }
  if(m_doHits){
    CHECK( addVariable ("trigHits", m_trigHits)  );
    CHECK( addVariable ("innerHits", m_innerHits)  );
    CHECK( addVariable ("middleHits", m_middleHits)  );
    CHECK( addVariable ("outerHits", m_outerHits)  );
  }
  if(m_doSegments){
    CHECK( addVariable ("innerSegments", m_innerSegments)  );
    CHECK( addVariable ("middleSegments", m_middleSegments)  );
    CHECK( addVariable ("outerSegments", m_outerSegments)  );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode MuonSpShowerFillerTool::fill(const Rec::MuonSpShower& p)
{

  if(m_doPosition){
    *m_eta = p.eta();
    *m_phi = p.phi();
  }
  if(m_doHits){
    *m_trigHits = p.numberOfTriggerHits();
    *m_innerHits = p.numberOfInnerHits();
    *m_middleHits = p.numberOfMiddleHits();
    *m_outerHits = p.numberOfOuterHits();
  }
  if(m_doSegments){
    *m_innerSegments = p.numberOfInnerSegments();
    *m_middleSegments = p.numberOfMiddleSegments();
    *m_outerSegments = p.numberOfOuterSegments();
  }

  return StatusCode::SUCCESS;
}


} // namespace QcdD3PD
