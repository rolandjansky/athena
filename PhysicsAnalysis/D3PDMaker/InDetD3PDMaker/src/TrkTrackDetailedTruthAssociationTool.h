/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETD3PDMaker_TRKTRACKDETAILEDTRUTHASSOCIATIONTOOL_H
#define INDETD3PDMaker_TRKTRACKDETAILEDTRUTHASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "AtlasHepMC/GenParticle.h"
#include <vector>

namespace Trk {
class Track;
}

namespace D3PD {

class TrkTrackDetailedTruthAssociationTool
  : public MultiAssociationTool<Trk::Track, HepMC::GenParticle>
{
public:
  typedef MultiAssociationTool<Trk::Track, HepMC::GenParticle> Base;
  typedef std::pair<DetailedTrackTruthCollection::const_iterator,DetailedTrackTruthCollection::const_iterator> Range;

  TrkTrackDetailedTruthAssociationTool (const std::string& type,
					     const std::string& name,
					     const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode reset (const Trk::Track& p);
  virtual const HepMC::GenParticle* next();

private:

  StatusCode invalidate(StatusCode);

  DetailedTrackTruthCollection::const_iterator m_itr;
  DetailedTrackTruthCollection::const_iterator m_end;

  /* StoreGate keys **/
  std::string m_TruthMap;
  std::string m_TrkContainer;

  /* NTuple variables **/
  int* m_npixhits_common;
  int* m_nscthits_common;
  int* m_ntrthits_common;
  
  int* m_npixhits_reco;
  int* m_nscthits_reco;
  int *m_ntrthits_reco;
  
  int *m_npixhits_truth;
  int *m_nscthits_truth;
  int *m_ntrthits_truth;
  
  int *m_barcode;

  HepMC::GenParticle m_dummy;

}; // class TrkTrackDetailedTruthAssociationTool

} // namespace D3PD

#endif // not INDETD3PDMaker_TRKTRACKDETAILEDTRUTHASSOCIATIONTOOL_H
