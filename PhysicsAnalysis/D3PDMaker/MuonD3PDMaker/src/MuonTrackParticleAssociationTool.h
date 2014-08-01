// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/MuonTrackParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an egamma to its primary TrackParticle.
 */


#ifndef MUOND3PDMAKER_MUONTRACKPARTICLEASSOCIATIONTOOL_H
#define MUOND3PDMAKER_MUONTRACKPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include <string>


namespace D3PD {


/**
 * @brief Associate from an Muon to its primary TrackParticle.
 */
class MuonTrackParticleAssociationTool
  : public SingleAssociationTool<xAOD::Muon, xAOD::TrackParticle>
{
public:
  typedef SingleAssociationTool<xAOD::Muon, xAOD::TrackParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonTrackParticleAssociationTool (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);


  /// Standard Gaudi initialize method.
  StatusCode initialize();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::TrackParticle* get (const xAOD::Muon& p);


private:
  /// Property: Which track particle should we return.
  std::string m_typeprop;

  enum {
    PRIMARY,
    COMBINED,
    INDET,
    INDET_OR_PRIMARY,
    MUONSPECTROMETER
  } m_type;
};


} // namespace D3PD



#endif // not MUOND3PDMAKER_MUONTRACKPARTICLEASSOCIATIONTOOL_H
