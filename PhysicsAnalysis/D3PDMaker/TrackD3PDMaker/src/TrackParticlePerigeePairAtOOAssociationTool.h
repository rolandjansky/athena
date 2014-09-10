// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackParticlePerigeePairAtOOAssociationTool.h
 * @author scott snyder
 * @date Dec, 2011
 * @brief Associate from a TrackParticle to the default Perigees
 *        for both itself and the original track (if it was refitted).
 */
#ifndef TRACKD3PDMAKER_TRACKPARTICLEPERIGEEPAIRATOOASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEPERIGEEPAIRATOOASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "TrackD3PDMaker/PerigeePair.h"
#include "GaudiKernel/ToolHandle.h"


namespace Rec {
  class TrackParticle;
}

namespace D3PD {


class IObjGetterTool;


/**
 * @brief Associate from a VxCandidate to its Position ErrorMatrix.
 */
class TrackParticlePerigeePairAtOOAssociationTool
  : public SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>,
                                 D3PD::PerigeePair>
{
public:
  typedef SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>,
                                D3PD::PerigeePair> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticlePerigeePairAtOOAssociationTool (const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent);


  /// @brief Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const D3PD::PerigeePair* get (const Rec::TrackParticle& p);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const D3PD::PerigeePair* get (const xAOD::TrackParticle& p);


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   */
  virtual void releaseObject (const D3PD::PerigeePair* p);


private:
  /// Property: Getter for the association object.
  ToolHandle<IObjGetterTool> m_assocGetter;


  /**
   * @brief Find the original track corresponding to a possibly refitted track.
   */
  const Rec::TrackParticle& findOldtrack (const Rec::TrackParticle& track);
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_TRACKPARTICLEPERIGEEPAIRATOOASSOCIATIONTOOL_H
