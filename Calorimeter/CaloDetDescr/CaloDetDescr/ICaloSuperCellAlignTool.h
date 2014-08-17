// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetDescr/ICaloSuperCellAlignTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2012
 * @brief Provide an interface to request an update of the supercell geometry.
 */


#ifndef CALODETDESCR_ICALOSUPERCELLALIGNTOOL_H
#define CALODETDESCR_ICALOSUPERCELLALIGNTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"
class CaloDetDescrManager;
class CaloSuperCellDetDescrManager;


/**
 * @brief Provide an interface to request an update of the supercell geometry.
 *
 * The supercell geometry is derived from the offline geometry, and is updated
 * via calls to the align hook.  However, during initialization, the
 * converter needs to explicitly run an update, in order to get everything
 * initialized the first time.  We can't just use the align method there,
 * since that will try to get the supercell geometry from the detector store,
 * and it won't be available there yet.  So just use this interface
 * to run it explicitly.
 */
class ICaloSuperCellAlignTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (ICaloSuperCellAlignTool, 1, 0);


  /**
   * @brief Update supercell geometry from the offline geometry.
   * @param mgr The supercell geometry manager.
   * @param cellmgr The offline geometry manager.
   */
  virtual StatusCode doUpdate (CaloSuperCellDetDescrManager* mgr,
                               const CaloDetDescrManager* cellmgr) = 0;
};



#endif // not CALODETDESCR_ICALOSUPERCELLALIGNTOOL_H
