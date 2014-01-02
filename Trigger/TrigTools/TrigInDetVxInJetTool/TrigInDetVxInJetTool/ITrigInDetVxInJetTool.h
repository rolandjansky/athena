/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************
//
// NAME:     ITrigInDetVxInJetTool.h
// PACKAGE:  Trigger/TrigTools/TrigTrackJetFinderTool
//
// AUTHOR:   Andrea Coccaro
// MAIL:     Andrea.Coccaro AT cern.ch
//
// **************************************************

#ifndef ITRIGINDETVXINJETTOOL_H
#define ITRIGINDETVXINJETTOOL_H

#include "GaudiKernel/IAlgTool.h"



class TrigInDetTrack;
class TrigVertex;
namespace CLHEP { class HepLorentzVector; }
namespace Trk   { class RecVertex; }


static const InterfaceID IID_ITrigInDetVxInJetTool("ITrigInDetVxInJetTool", 1, 0);

class ITrigInDetVxInJetTool : virtual public IAlgTool {

 public:

  static const InterfaceID& interfaceID() { return IID_ITrigInDetVxInJetTool;}

  virtual const TrigVertex* findSecVertex(const Trk::RecVertex&, 
					  const CLHEP::HepLorentzVector&, 
					  const std::vector<const TrigInDetTrack*>&) =0;

};

#endif
