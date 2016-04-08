/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: TrigTrackSlimmer.h
 //
 // author: Jiri Masik 
 //
 // Description:  Slim input track collection 
 */
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGTRACKSLIMMER_H
#define INDETTRIGTRACKSLIMMER_H

#include "GaudiKernel/ToolHandle.h"

//typedef
#include "TrkTrack/TrackCollection.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"


namespace Trk {
  class ITrackSlimmingTool;
  class ITrackSummaryTool;
}

namespace InDet
{

  class TrigTrackSlimmer : public HLT::FexAlgo {
  public:
    TrigTrackSlimmer(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigTrackSlimmer();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();

  private:
    ToolHandle<Trk::ITrackSlimmingTool>        m_trackSlimmingTool;
    ToolHandle<Trk::ITrackSummaryTool>         m_sumTool;
    const TrackCollection*                m_allTracksFromStoreGate;

    

    //monitoring stuff
    bool                     m_monitoring; //!< monitoring variables
    std::string              m_path;      //!< monitoring path


  };

}

#endif
