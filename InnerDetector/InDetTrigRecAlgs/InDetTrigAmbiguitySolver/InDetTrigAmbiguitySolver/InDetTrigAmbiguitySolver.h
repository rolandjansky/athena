/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
//
// filename: InDetTrigAmbiguitySolver.h
//
// author: Patricia Conde Muino 07/09/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Trigger version of the InDetAmbiguitySolver
//
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGAMBIGUITYSOLVER_H
#define INDETTRIGAMBIGUITYSOLVER_H

#include <string>
#include "GaudiKernel/ToolHandle.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapExchangeTool.h"
#include "CxxUtils/checker_macros.h"

namespace Trk { class ITrackAmbiguityProcessorTool; }

namespace InDet {

  /**Algorithm does ambiguity processing This algorithm uses the
     TrkAmbiguityProcessorTool AlgTool to resolve ambiguities in the passed
     tracks.
  */

  class InDetTrigAmbiguitySolver:public HLT::FexAlgo
  {

  public:
    InDetTrigAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator);
    ~InDetTrigAmbiguitySolver();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();

  private:
    HLT::ErrorCode savePRDtoTrackMap ATLAS_NOT_THREAD_SAFE // Thread unsafe Trk::ITrackAmbiguityProcessorTool::setPRDtoTrackMap is used.
      (std::unique_ptr<Trk::PRDtoTrackMap> map);

    bool                    m_resolveTracks;     //!whether to resolve tracks, defaults to true
    const TrackCollection  *m_oldTracks;
    TrackCollection        *m_tracks;
  
    /**Number of tracks processed. Useful for debugging*/
    uint32_t           m_trackInCount;
    uint32_t           m_trackOutCount;
    long int           m_TotalTrackInCount; 
    long int           m_TotalTrackOutCount;  
    std::vector<float> m_chi2ndof;
    std::vector<float> m_d0;
    std::vector<float> m_z0;
    std::vector<float> m_phi;
    std::vector<float> m_eta;
    std::vector<float> m_qoverp;

    /** responsible for actual amiguity processing*/
    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiTool;

     /** Optional tool to export a PRD-to-track map to allow downstream clients to compute shared hits.*/
    ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
       {this, "AssociationTool", "" };

     /** Tool to emulate the run2 behaviour.
      * The downstream clients will use the latest PRD association map indpendent of the feature that created it.
      * If this tool is set, the map will be exchanged via the tool rather than being  attached as a feature.
      */
    PublicToolHandle<Trk::IPRDtoTrackMapExchangeTool>         m_prdToTrackMapExchange
       {this, "PRDToTrackMapExchange", "" };

    std::string      m_inputTracksLabel;
    std::string      m_outputTracksLabel;
    std::string      m_outputPRDMapLabel;

    uint32_t         m_ntimesInvoked;

    bool                   m_doTimeOutChecks;   //check global timer
  };

}

#endif 




