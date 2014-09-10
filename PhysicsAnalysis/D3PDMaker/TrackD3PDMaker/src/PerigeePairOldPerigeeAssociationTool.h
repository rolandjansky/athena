// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/PerigeePairOldPerigeeAssociationTool.h
 * @author scott snyder
 * @date Apr, 2012
 * @brief Associate from a PerigeePair to the perigee for the original track.
 */
#ifndef TRACKD3PDMAKER_PERIGEEPAIROLDPERIGEEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_PERIGEEPAIROLDPERIGEEASSOCIATIONTOOL_H


#include "TrackD3PDMaker/PerigeePair.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"


namespace D3PD {


/**
 * @brief Associate from a PerigeePair to the perigee for the original track.
 */
class PerigeePairOldPerigeeAssociationTool
  : public SingleAssociationTool<D3PD::PerigeePair, Trk::Perigee>
{
public:
  typedef SingleAssociationTool<D3PD::PerigeePair, Trk::Perigee> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PerigeePairOldPerigeeAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::Perigee* get (const D3PD::PerigeePair& p);
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_PERIGEEPAIROLDPERIGEEASSOCIATIONTOOL_H
