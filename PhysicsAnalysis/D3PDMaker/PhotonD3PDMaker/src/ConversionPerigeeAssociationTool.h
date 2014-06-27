// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConversionPerigeeAssociationTool.h
/**
 * @file PhotonD3PDMaker/src/ConversionPerigeeAssociationTool.h
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date November 2012
 * @brief Associate Perigee to conversions tracks
 */

#ifndef PHOTOND3PDMAKER_CONVERSIONPERIGEEASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_CONVERSIONPERIGEEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class VxTrackAtVertex;
}

namespace D3PD {

  /**
   * @brief Associate from a VxTrackAtVertex to a Perigee.
   */
  class ConversionPerigeeAssociationTool
    : public SingleAssociationTool<Trk::VxTrackAtVertex, Trk::Perigee>
  {
  public:
    typedef SingleAssociationTool<Trk::VxTrackAtVertex, Trk::Perigee> Base;
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    ConversionPerigeeAssociationTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent);
    
    /**
     * @brief Return the target object.
     * @param p The source object for the association.
     *
     * Return the target of the association, or 0.
     */
    virtual const Trk::Perigee* get (const Trk::VxTrackAtVertex& p);
  };
  
  
} // namespace D3PD



#endif // not PHOTOND3PDMAKER_CONVERSIONPERIGEEASSOCIATIONTOOL_H
