/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/**
 * @file egammaD3PDMaker/src/GSFOriginalTrackParticleAssociationTool.cxx
 * @author Mike Hance <mike.hance@cern.ch>
 * @date March, 2012
 * @brief Associate from a GSF fitted track to the original trackparticle
 */


#include "GSFOriginalTrackParticleAssociationTool.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "ParticleEvent/TrackParticleAssocs.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GSFOriginalTrackParticleAssociationTool::GSFOriginalTrackParticleAssociationTool
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
StatusCode GSFOriginalTrackParticleAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_assocGetter.retrieve() );
  CHECK( m_assocGetter->configureD3PD<TrackParticleAssocs> () );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Rec::TrackParticle*
GSFOriginalTrackParticleAssociationTool::get (const Rec::TrackParticle& track)
{

  if (track.info().trackFitter() != Trk::TrackInfo::GaussianSumFilter)
    return &track;

  const TrackParticleAssocs* assocs = m_assocGetter->get<TrackParticleAssocs>();
  if (!assocs) {
    REPORT_MESSAGE (MSG::WARNING) << "Can't find GSF track association.";
    return &track;
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
    return &track;
  }

  return ret;
}


} // namespace D3PD
