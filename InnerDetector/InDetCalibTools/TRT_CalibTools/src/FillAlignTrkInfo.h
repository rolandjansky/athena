// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__FILLALIGNTRKINFO_H
#define TRT_CALIBTOOLS__FILLALIGNTRKINFO_H
/********************************************************************

NAME:     FillAlignTrkInfo
PACKAGE:  TRT_AlignTOOLS

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Tool to fill Track info into the TRT::Track messenger class

********************************************************************/

// INCLUDES:

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TRT_CalibTools/IFillAlignTrkInfo.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"

class FillAlignTrkInfo :  virtual public IFillAlignTrkInfo, public AthAlgTool {
 public:

  FillAlignTrkInfo(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~FillAlignTrkInfo() {}

  virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output,
                    const ComTime* theComTime, const xAOD::EventInfo& eventInfo,
                    const xAOD::VertexContainer& vertices);
  //virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output) const;
//  virtual double mygetEventPhase(const Trk::Track *aTrack);

  virtual StatusCode initialize();
 private:
  ToolHandle<Trk::ITrackSummaryTool> m_TrackSummaryTool;
};

#endif //  TRT_CALIBTOOLS__FILLALIGNTRKINFO_H
