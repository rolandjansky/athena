// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_FilterTracksInRoIs_h
#define ViewAlgs_FilterTracksInRoIs_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "AthContainers/ConstDataVector.h"
#include "TrkTrack/TrackCollection.h"

//#include "AthViews/View.h"
#include "./TrackRoIAssocTool.h"


class FilterTracksInRoIs : public AthAlgorithm {
public:
  FilterTracksInRoIs(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandle <TrackRoIAssocTool> m_assocTool;

  SG::ReadHandleKey< xAOD::TrigCompositeContainer > m_compContainer;
  SG::WriteHandleKey< ConstDataVector<TrackCollection> > m_outputTracks;

};
#endif
