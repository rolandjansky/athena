/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: SimpleTrigTrackCollMerger.h
 //
 // author: Jiri Masik 
 //
 // Description:  iterate over input track collections and merge their tracks
 //              
 //               based on Trk::TrackCollectionMerger
 */
////////////////////////////////////////////////////////////////////////////


#ifndef SIMPLETRIGTRACKMERGER_H
#define SIMPLETRIGTRACKMERGER_H

#include "GaudiKernel/ToolHandle.h"

//typedef
#include "TrkTrack/TrackCollection.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"



namespace Trk {
  
  //forward declarations
  class IPRD_AssociationTool;
  class ITrackSummaryTool;

  class SimpleTrigTrackCollMerger : public HLT::FexAlgo {

  public:
    SimpleTrigTrackCollMerger(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~SimpleTrigTrackCollMerger();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();

  private:
    HLT::ErrorCode mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol);

  private:
    ToolHandle<Trk::IPRD_AssociationTool>     m_assoTool;
    ToolHandle<Trk::ITrackSummaryTool>        m_sumTool;
    std::vector<std::string>                  m_inputColl;
    std::string                               m_outputColl;
  };

}


#endif
