/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DiMuonTool_H
#define DiMuonTool_H

// ***********************************************************************
// DiMuonTool.h
// AUTHORS: Weina Ji (weina.ji@cern.ch)
//          Peter Wijeratne (paw@hep.ucl.ac.uk)
// ***********************************************************************

#include "Particle/TrackParticle.h"
#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigJPsiTools/MuonTrackSelector.h"

class DiMuonTool : public MuonTrackSelector
{
 public:
  DiMuonTool();
  ~DiMuonTool();

  std::vector<const Wrapper::MuonTrack*> tracks() { return m_tracks; }
    
  double InvariantMass();
  double CurvatureDiff();
  double pt();
  double phi();
  double eta();

  const Wrapper::MuonTrack* TaggedTrack();
  const Wrapper::MuonTrack* ProbedTrack();

  bool doTagAndProbe();
  void clear();

  //Inputs given in MakeDiMuonsTool
  void setTracks(std::vector<const Wrapper::MuonTrack*> tracksIn) {m_tracks=tracksIn;}
  void setTracks(const Wrapper::MuonTrack* track1, const Wrapper::MuonTrack* track2) {m_tracks.push_back(track1); m_tracks.push_back(track2);}

  void setTagAndProbe(bool doTAndP) {m_doTagAndProbe=doTAndP;}
  void setTrackMass(double mass) {m_trackMass=mass;} 
  double getTrackMass() const { return m_trackMass; } 

  const std::vector<const Wrapper::MuonTrack*>& tracks() const { return m_tracks; }

 private:

  std::vector<const Wrapper::MuonTrack*> m_tracks;

  bool m_doTagAndProbe;

  double m_trackMass;
};



inline std::ostream& operator<<( std::ostream& s, const DiMuonTool& t) {
  return s << "[ mass = " << t.getTrackMass() << " m_tracks size= " << t.tracks().size() << " ] ";
}


#endif
