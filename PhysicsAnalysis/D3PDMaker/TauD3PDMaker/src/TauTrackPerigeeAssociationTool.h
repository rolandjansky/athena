// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauTrackPerigeeAssociationTool.h 577665 2014-01-09 23:18:59Z ssnyder $
/**
 * @file TauD3PDMaker/src/TauTrackPerigeeAssociationTool.h
 * @author KG <kong.guan.tan@cern.ch>
 * @date October, 2011
 * @brief Associate from a TrackParticle to its perigee at the tau's origin
 */
#ifndef TAUD3PDMAKER_TAUTRACKPERIGEEASSOCIATIONTOOL_H
#define TAUD3PDMAKER_TAUTRACKPERIGEEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "tauEvent/TauJet.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>

namespace Rec {
  class TrackParticle;
}

namespace D3PD {

/**
 * @brief Associate from a TrackParticle to its perigee at the tau's origin.
 */
class TauTrackPerigeeAssociationTool
  : public MultiAssociationTool<Analysis::TauJet, Trk::Perigee>
{
public:
  typedef MultiAssociationTool<Analysis::TauJet, Trk::Perigee> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauTrackPerigeeAssociationTool (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);

  StatusCode initialize();

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Analysis::TauJet& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const Trk::Perigee* next();

  virtual void releaseElement (const Trk::Perigee* p);


private:
  /* Keeps track of track counter */
  const Analysis::TauJet* m_tauJet;
  unsigned int m_trackCounter;
  unsigned int m_numTrack;

  /* Track to vertex extrapolator **/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
};


} // namespace D3PD

#endif // not TAUD3PDMAKER_TAUTRACKPERIGEEASSOCIATIONTOOL_H
