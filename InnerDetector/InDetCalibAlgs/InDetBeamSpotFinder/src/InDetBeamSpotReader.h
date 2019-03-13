/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETBEAMSPOTREADER_H
#define INDET_INDETBEAMSPOTREADER_H
//////////////////////////////
// Author jwalder@cern.ch
// Simple Alogrithm to read the current beamspot value
//////////////////////////////


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "VxVertex/VxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

namespace InDet {
  /** An example class of how to read beamspot position.
      Currently there must be a valid beamspot entry for the IOV of data that is 
      being read. Otherwise an error will occur.
   */
  class InDetBeamSpotReader : public AthReentrantAlgorithm {

  public:
    //Standard constructor and methods
    InDetBeamSpotReader  (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize() override;
    StatusCode execute(const EventContext& ctx) const override;
    StatusCode finalize() override;


  private:
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo
      {this, "EvtInfo", "EventInfo", "EventInfo name"};
    SG::ReadHandleKey<VxContainer> m_vxContainer
      {this, "VxContainer", "VxPrimaryCandidate", "Vertex container name"};
  };

}//end namespace 

#endif

