/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************
//
// NAME:     ITrigTrackJetFinderTool.h
//
// AUTHOR:   Andrea Coccaro
// MAIL:     Andrea.Coccaro@ge.infn.it
// 
// **************************************************

#ifndef TRIGINDETTOOLINTERFACES_ITRIGTRACKJETFINDERTOOL_H
#define TRIGINDETTOOLINTERFACES_ITRIGTRACKJETFINDERTOOL_H

#include "GaudiKernel/IAlgTool.h"



class TrigInDetTrack;
namespace Rec { class TrackParticle; }
namespace Trk { class Track;}


static const InterfaceID IID_ITrigTrackJetFinderTool("ITrigTrackJetFinderTool", 1, 0);

class ITrigTrackJetFinderTool : virtual public IAlgTool {

 public:

  static const InterfaceID& interfaceID() {return IID_ITrigTrackJetFinderTool;}

  virtual void setDefTrackSel(bool) = 0;
  virtual void setExtTrackSel(bool) = 0;

  virtual void inputPrimaryVertexZ(const float&) = 0;
  virtual void addTrack(const TrigInDetTrack*&, unsigned int) = 0;
  virtual void addTrack(const Rec::TrackParticle*&, unsigned int) = 0;
  virtual void addTrack(const Trk::Track*&, unsigned int) = 0;

  virtual void findJet(std::vector<int>&, float&, float&) = 0;

  virtual void clear() = 0;
        
};

#endif
