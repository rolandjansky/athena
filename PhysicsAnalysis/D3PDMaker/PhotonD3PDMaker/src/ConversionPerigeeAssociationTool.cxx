/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConversionPerigeeAssociationTool.cxx
/**
 * @file PhotonD3PDMaker/src/ConversionPerigeeAssociationTool.cxx
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date November 2012
 * @brief Associate Perigee to conversions tracks
 */


#include "ConversionPerigeeAssociationTool.h"
#include "Particle/TrackParticle.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ConversionPerigeeAssociationTool::ConversionPerigeeAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
  {
  }
  
  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
const Trk::Perigee *ConversionPerigeeAssociationTool::get (const Trk::VxTrackAtVertex& track)
  {
    return dynamic_cast<const Trk::Perigee*>(track.perigeeAtVertex());
  }
  
} // namespace D3PD



