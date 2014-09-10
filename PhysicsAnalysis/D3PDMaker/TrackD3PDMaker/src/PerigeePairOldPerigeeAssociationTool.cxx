/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/PerigeePairOldPerigeeAssociationTool.cxx
 * @author scott snyder
 * @date Apr, 2012
 * @brief Associate from a PerigeePair to the perigee for the original track.
 */


#include "PerigeePairOldPerigeeAssociationTool.h"
#include "TrkParameters/TrackParameters.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PerigeePairOldPerigeeAssociationTool::PerigeePairOldPerigeeAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::Perigee*
PerigeePairOldPerigeeAssociationTool::get (const D3PD::PerigeePair& p)
{
  return p.oldtrack;
}


} // namespace D3PD
