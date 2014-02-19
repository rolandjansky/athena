/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__TRTCalibTrackSelectionTool_H
#define TRT_CALIBTOOLS__TRTCalibTrackSelectionTool_H
/********************************************************************
 
NAME:     TRTCalibTrackSelectionTool
PACKAGE:  TRT_AlignTOOLS
 
AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005
 
PURPOSE:  Tool to select Tracks to be used in alignment
 
********************************************************************/

#include "TRT_CalibTools/ISelectionTool.h"
#include "TrkTrack/Track.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class ITrackSummaryTool;
}

static const InterfaceID IID_ISelectionTool("TRTCalibTrackSelectionTool", 1, 0);

class TRTCalibTrackSelectionTool :  virtual public ISelectionTool<Trk::Track>, public AthAlgTool {
public:
  TRTCalibTrackSelectionTool(const std::string& type, const std::string& name, const IInterface* parent);
  
  virtual ~TRTCalibTrackSelectionTool() {}
  
  virtual StatusCode initialize();

  virtual bool isAccepted(const Trk::Track* element) const;

  static const InterfaceID& interfaceID();

private:
  float m_PtMin;
  float m_EtaMin;
  float m_EtaMax;
  float m_PhiOffset;
  float m_DeltaPhi;
  ToolHandle<Trk::ITrackSummaryTool> m_TrackSummaryTool;
  int m_MinPixelHits;
  int m_MinSCTHits;
  int m_MinTRTHits;
  
};

inline const InterfaceID& TRTCalibTrackSelectionTool::interfaceID()
{ 
        return IID_ISelectionTool; 
}


#endif //  TRT_CALIBTOOLS__TRTCalibTrackSelectionTool_H
