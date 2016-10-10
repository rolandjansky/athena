// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TrackHypoAlg_h
#define ViewAlgs_TrackHypoAlg_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


//#include "AthViews/View.h"
#include "./TrackHypoTool.h"
#include "./TrackRoIAssocTool.h"


class TrackHypoAlg : public AthAlgorithm {
public:
  TrackHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandleArray<TrackHypoTool> m_hypos;
  ToolHandle <TrackRoIAssocTool> m_assocTool;
  

  SG::ReadHandle< TrackCollection >     m_inputTracks;
  SG::ReadHandle< TrigRoiDescriptorCollection> m_inputRoIs;


  SG::WriteHandle< xAOD::TrigCompositeContainer >    m_output;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputAux;
};



#endif
