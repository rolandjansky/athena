// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Header file for class SiCombinatorialTrackFinderData_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiCombinatorialTrackFinderData_xk_H
#define SiCombinatorialTrackFinderData_xk_H

#include "SiSPSeededTrackFinderData/SiTrajectory_xk.h"
#include "SiSPSeededTrackFinderData/SiTools_xk.h"
#include "TrkTrack/TrackInfo.h"

#include <list>

namespace Trk {
  class Track;
}

namespace InDet {

 /**
  @class SiCombinatorialTrackFinderData_xk
  
  InDet::SiCombinatorialTrackFinderData_xk holds event dependent data
  used by SiCombinatorialTrackFinder_xk.
  @author Susumu.Oda@cern.ch
  */

  class SiCombinatorialTrackFinderData_xk {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

  public:
    SiCombinatorialTrackFinderData_xk();
    ~SiCombinatorialTrackFinderData_xk() = default;

    void setTools(const Trk::IPatternParametersPropagator* propTool,
                  const Trk::IPatternParametersUpdator* updatorTool,
                  const Trk::IRIO_OnTrackCreator* rioTool,
                  MagField::IMagFieldSvc* fieldService,
                  const IInDetConditionsTool* pixCondTool,
                  const IInDetConditionsTool* sctCondTool,
                  const Trk::MagneticFieldProperties* fieldProp);

    const Trk::PRDtoTrackMap* PRDtoTrackMap() const { return m_tools.PRDtoTrackMap(); }

    bool isInitialized() const;

    SiTrajectory_xk& trajectory();
    Trk::TrackInfo& trackinfo();
    InDet::SiTools_xk& tools();
    std::list<Trk::Track*>& tracks();
    int& nprint();
    int& inputseeds();
    int& goodseeds();
    int& findtracks();
    int& inittracks();
    int& roadbug();
    bool& heavyIon();
    int& cosmicTrack();
    int& nclusmin();
    int& nclusminb();
    int& nwclusmin();
    int& nholesmax();
    int& dholesmax();
    bool& simpleTrack();
    double& pTmin();
    double& pTminBrem();
    double& xi2max();
    double& xi2maxNoAdd();
    double& xi2maxlink();

  protected:
    virtual void dummy() = 0;
    void setPRDtoTrackMap(const Trk::PRDtoTrackMap* prd_to_track_map) {
      m_tools.setPRDtoTrackMap(prd_to_track_map);
    }

  private:

    bool m_initialized{false};
    SiTrajectory_xk m_trajectory; // Track trajectory
    Trk::TrackInfo m_trackinfo;
    InDet::SiTools_xk m_tools;
    std::list<Trk::Track*> m_tracks; // List found tracks
    int m_nprint{0}; // Kind output information
    int m_inputseeds{0}; // Number input seeds
    int m_goodseeds{0}; // Number accepted seeds
    int m_findtracks{0}; // Number found tracks
    int m_inittracks{0}; // Number initial tracks
    int m_roadbug{0}; // Number wrong DE roads
    bool m_heavyIon{false};
    int m_cosmicTrack{0};  // Is it cosmic track (0 or 1)
    int m_nclusmin{0}; // Min number clusters
    int m_nclusminb{0}; // Min number clusters
    int m_nwclusmin{0}; // Min number weighted clusters
    int m_nholesmax{0}; // Max number holes
    int m_dholesmax{0}; // Max holes gap
    bool m_simpleTrack{false};
    double m_pTmin{0.}; // min pT
    double m_pTminBrem{0.}; // min pT for brem noise model
    double m_xi2max{0.}; // max Xi2 for updators
    double m_xi2maxNoAdd{0.}; // max Xi2 for clusters
    double m_xi2maxlink{0.}; // max Xi2 for clusters

  };

} // end of name space

#endif // SiCombinatorialTrackFinderData_xk_H
