/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackParticleAtOOPerigeePairAtOOAssociationTool.cxx
 * @author scott snyder
 * @date Dec, 2011
 * @brief Associate from a TrackParticle to the default Perigees
 *        for both itself and the original track (if it was refitted).
 */


#include "TrackParticlePerigeePairAtOOAssociationTool.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "ParticleEvent/TrackParticleAssocs.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticlePerigeePairAtOOAssociationTool::TrackParticlePerigeePairAtOOAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_assocGetter (this)
{
  declareProperty ("AssocGetter", m_assocGetter,
                   "Getter for the association object.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TrackParticlePerigeePairAtOOAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  if (!m_assocGetter.empty()) {
    CHECK( m_assocGetter.retrieve() );
    CHECK( m_assocGetter->configureD3PD<TrackParticleAssocs> () );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const D3PD::PerigeePair*
TrackParticlePerigeePairAtOOAssociationTool::get
  (const Rec::TrackParticle& track)
{
  const Rec::TrackParticle& oldtrack = findOldtrack (track);
  return new D3PD::PerigeePair (track.perigee(), oldtrack.perigee());
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const D3PD::PerigeePair*
TrackParticlePerigeePairAtOOAssociationTool::get
  (const xAOD::TrackParticle& track)
{
  static SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer> > 
    orig ("originalTrackParticle");
  if (!orig.isAvailable(track) || !orig(track).isValid())
    return 0;

  return new D3PD::PerigeePair (&track.perigeeParameters(),
                                &(*orig(track))->perigeeParameters());
}


/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 */
void TrackParticlePerigeePairAtOOAssociationTool::releaseObject
 (const D3PD::PerigeePair* p)
{
  delete p;
}


/**
 * @brief Find the original track corresponding to a possibly refitted track.
 */
const Rec::TrackParticle&
TrackParticlePerigeePairAtOOAssociationTool::findOldtrack
 (const Rec::TrackParticle& track)
{
  if (track.info().trackFitter() != Trk::TrackInfo::GaussianSumFilter)
    return track;

  const TrackParticleAssocs* assocs = m_assocGetter->get<TrackParticleAssocs>();
  if (!assocs) {
    REPORT_MESSAGE (MSG::WARNING) << "Can't find GSF track association.";
    return track;
  }

  TrackParticleAssocs::asso_iterator beg = assocs->beginAssociation (&track);
  TrackParticleAssocs::asso_iterator end = assocs->endAssociation (&track);
  const Rec::TrackParticle* ret = 0;
  if (beg != end && beg.isValid())
    ret = dynamic_cast<const Rec::TrackParticle*> (*beg);
  m_assocGetter->releaseObject (assocs);
  if (!ret) {
    REPORT_MESSAGE (MSG::WARNING)
      << "Can't find original track for GSF refitted track.";
    return track;
  }

  return *ret;
}


} // namespace D3PD
