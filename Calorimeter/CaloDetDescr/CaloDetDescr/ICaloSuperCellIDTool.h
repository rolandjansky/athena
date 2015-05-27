// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetDescr/ICaloSupercellIDTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Interface for tool to map between calorimeter cells and supercells.
 */


#ifndef CALODETDESCR_ICALOSUPERCELLIDTOOL_H
#define CALODETDESCR_ICALOSUPERCELLIDTOOL_H


#include "Identifier/Identifier.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include <vector>


/**
 * @brief Interface for tool to map between calorimeter cells and supercells.
 */
class ICaloSuperCellIDTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (ICaloSuperCellIDTool, 1, 0);

  /**
   * @brief Given an offline cell identifier, return the corresponding
   *        supercell identifier.  If none exists, an invalid identifier
   *        is returned.
   */
  virtual Identifier offlineToSuperCellID (const Identifier& id) const = 0;


  /**
   * @brief Given a supercell identifier, return the list of corresponding
   *        offline cell identifiers.
   */
  virtual std::vector<Identifier>
  superCellToOfflineID (const Identifier& id) const = 0;


  /**
   * @brief Given an offline region identifier, return the corresponding
   *        supercell region identifier(s).  There will normally
   *        be only one, but it's possible for there to be multiple
   *        matches.  If none exists, an invalid identifier
   *        is returned.
   */
  virtual std::vector<Identifier>
  offlineToSuperCellRegion (const Identifier& reg_id) const = 0;


  /**
   * @brief Given a supercell region identifier, return the corresponding
   *        offline region identifier(s).  There will normally
   *        be only one, but it's possible for there to be multiple
   *        matches.  If none exists, an invalid identifier
   *        is returned.
   */
  virtual std::vector<Identifier>
  superCellToOfflineRegion (const Identifier& reg_id) const = 0;
};



#endif // not CALODETDESCR_ICALOSUPERCELLIDTOOL_H
