/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSELECTIONTOOL_H
#define TRACKSELECTIONTOOL_H

// **********************************************************************
// TrackSelectionTool.h
// AUTHORS: Ben Cooper
// **********************************************************************

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>

namespace Trk{
  class ITrackSelectorTool;
  class Track;
}

namespace InDet {
  class IInDetTrackSelectionTool;
}


namespace InDetAlignMon{

  static const InterfaceID IID_TrackSelectionTool("InDetAlignMon::TrackSelectionTool", 1, 0);
  
  class TrackSelectionTool : public AthAlgTool
  {
    
  public:
    
    static const InterfaceID& interfaceID() {return IID_TrackSelectionTool;}
    
    TrackSelectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TrackSelectionTool ();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    //if this method is used the decision on which trackcollection
    //is made by the calling method
    DataVector<Trk::Track>* selectTracksNew(const std::string&);
    DataVector<Trk::Track>* selectTracks(const std::string &);
    DataVector<xAOD::TrackParticle>* selectTracksParticle(const std::string &);

    //if this method the decision on which trackcollection
    //is made from the configuration of the TrackSlectionTool (in jobOptions)
    DataVector<Trk::Track>* selectTracks();

    std::string getTrackColName(){
      return m_trackColName;
    }
    
    //Do we cut on the event phase
    bool m_doEventPhaseCut;
    bool m_useIDTrackSelectionTool;
    float m_maxEventPhase;
    float m_minEventPhase;
    std::string m_commTimeName;

  private:
    
    ToolHandle< Trk::ITrackSelectorTool > m_trackSelectorTool; //!<  Pointer to Trk::ITrackSelectorTool

    ToolHandle<InDet::IInDetTrackSelectionTool>   m_idtrackSelectionTool; //!< Pointer to InDet::IInDetTraclSelectionTool>

    bool m_passAllTracks;//switch that enables track selection to be bypassed completely

    //these member variables only play a role if use selectTracks() zero argument method above
    std::string m_trackColName;
    std::string m_VtxContainerName;
    unsigned int m_minTracksPerVtx;
    bool m_usePrimVtx;
    
  };
}


#endif 
