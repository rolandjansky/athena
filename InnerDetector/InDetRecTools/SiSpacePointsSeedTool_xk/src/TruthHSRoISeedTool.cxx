/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TruthHSRoISeedTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "SiSpacePointsSeedTool_xk/TruthHSRoISeedTool.h"
#include "TVector2.h"
#include <map>
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TruthHSRoISeedTool::TruthHSRoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_inputTruthEvents("TruthEvents")
{

  //
  declareInterface<IZWindowRoISeedTool>(this);

  //
  declareProperty("InputTruthEventsCollection", m_inputTruthEvents );  
  declareProperty("TrackZ0Window", m_z0Window = 1.0);

}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::TruthHSRoISeedTool::~TruthHSRoISeedTool()
{
}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode InDet::TruthHSRoISeedTool::initialize()
{
  StatusCode sc = AlgTool::initialize();   
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TruthHSRoISeedTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

/////////////////////////////////////////////////////////////////////
// Compute RoI
/////////////////////////////////////////////////////////////////////

std::vector<InDet::IZWindowRoISeedTool::ZWindow> InDet::TruthHSRoISeedTool::getRoIs()
{

  // prepare output
  std::vector<InDet::IZWindowRoISeedTool::ZWindow> listRoIs;  
  listRoIs.clear();

  //retrieve truth collection
  xAOD::TruthEventContainer *truthEvents;
  if (evtStore()->retrieve(truthEvents, m_inputTruthEvents).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve xAOD truth information. Bailing out with empty RoI list.");
    return listRoIs;
  }
  ATH_MSG_DEBUG("xAOD Truth available!");

  //get HS position
  for (xAOD::TruthEvent *evt : *truthEvents) {
    const xAOD::TruthVertex *hsPos = evt->signalProcessVertex();
    if (hsPos == nullptr) {
      ATH_MSG_DEBUG("Invalid signal process vertex! Trying next TruthEvent.");
      continue;
    }
    InDet::IZWindowRoISeedTool::ZWindow RoI;
    RoI.zReference = hsPos->z();
    RoI.zWindow[0] = RoI.zReference - m_z0Window; 
    RoI.zWindow[1] = RoI.zReference + m_z0Window;
    listRoIs.push_back(RoI);
    ATH_MSG_DEBUG("Found RoI: " << RoI.zReference << " [" << RoI.zWindow[0] << ", " << RoI.zWindow[1] << "]");
    //use only the first one
    break;
  }

  return listRoIs;
  
}

