// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRACKD3PDMAKER_TRKTRACKTRUTHASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRKTRACKTRUTHASSOCIATIONTOOL_H

// EDM include(s):
#include "TrkTrack/Track.h"
#include "HepMC/GenParticle.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

class TrkTrackTruthAssociationTool
  : public SingleAssociationTool<Trk::Track, HepMC::GenParticle>
{
public:
  typedef SingleAssociationTool<Trk::Track, HepMC::GenParticle> Base;

  TrkTrackTruthAssociationTool (const std::string& type,
				       const std::string& name,
				       const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual const HepMC::GenParticle* get (const Trk::Track& p);

private:
  /* StoreGate keys **/
  std::string m_TruthMap;
  std::string m_TrkContainer;

  /* NTuple variables **/
  float* m_trueProb;
  int* m_trueBarcode;

}; // class TrkTrackTruthAssociationTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKTRUTHASSOCIATIONTOOL_H
