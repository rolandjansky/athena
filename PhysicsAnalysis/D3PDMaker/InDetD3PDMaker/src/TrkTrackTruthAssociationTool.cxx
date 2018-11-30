/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

// Local include(s):
#include "TrkTrackTruthAssociationTool.h"

namespace D3PD {

TrkTrackTruthAssociationTool::TrkTrackTruthAssociationTool (const std::string& type,
							    const std::string& name,
							    const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("MapKey", m_TruthMap = "TrackTruthMap");
  declareProperty ("SGKey", m_TrkContainer = "Tracks");

  book().ignore(); // Avoid coverity warnings.
}


StatusCode TrkTrackTruthAssociationTool::initialize()
{

  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TrkTrackTruthAssociationTool::book ()
{
  CHECK( addVariable ("probability", m_trueProb) );
  CHECK( addVariable ("barcode", m_trueBarcode) );

  return StatusCode::SUCCESS;
}


const HepMC::GenParticle* TrkTrackTruthAssociationTool::get (const Trk::Track& track)
{

   // Check if the track collection is available:
   if( ! evtStore()->contains< TrackCollection >( m_TrkContainer ) ) {
      return 0;
   }

   // Check if the truth collection is available:
   if( ! evtStore()->contains< TrackTruthCollection >(  m_TruthMap ) ) {
      return 0;
   }

   /// Get Track and TrackTruth Collections
   /// Should be using Getter tools intead of StoreGate
   // -- TrackParticles:
   const TrackCollection* tc = 0;
   if( evtStore()->retrieve( tc, m_TrkContainer ).isFailure() ) {
      REPORT_MESSAGE( MSG::WARNING )
         << "No TrackCollection with SG key \"" << m_TrkContainer
         << "\" found, even though StoreGateSvc::contains said it should be there!";
      return 0;
   }
   // -- TrackTruth:
   const TrackTruthCollection* tm = 0;
   if( evtStore()->retrieve( tm, m_TruthMap ).isFailure() ) {
      REPORT_MESSAGE( MSG::WARNING )
         << "No TrackTruthCollection with SG key \"" << m_TruthMap
         << "\" found, even though StoreGateSvc::contains said it should be there!";
      return 0;
   }

  const HepMC::GenParticle *particle = 0;
  *m_trueProb = 0;
  *m_trueBarcode = -1;

  ElementLink<TrackCollection> tlink;
  tlink.setElement(&track);
  tlink.setStorableObject(*tc);
  TrackTruthCollection::const_iterator found = tm->find(tlink);
  if(found != tm->end()) {
    *m_trueProb = found->second.probability();
    *m_trueBarcode = found->second.particleLink().barcode();
    particle = found->second.particleLink().cptr();
  }

  return particle;
}

} // namespace D3PD
