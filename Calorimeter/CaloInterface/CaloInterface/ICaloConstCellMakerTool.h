// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloInterface/ICaloConstCellMakerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Operate on pointers to `const CaloCell`.
 */


#ifndef CALOINTERFACE_ICALOCONSTCELLMAKERTOOL_H
#define CALOINTERFACE_ICALOCONSTCELLMAKERTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"


class CaloConstCellContainer;


/**
 * @brief Operate on pointers to `const CaloCell`.
 *
 * This is analogous to @c ICaloCellMakerTool, except that it produces
 * a container of const cell pointers.  This is thus appropriate for
 * copying cell pointers from an existing container.
 */
class ICaloConstCellMakerTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (ICaloConstCellMakerTool, 1, 0);

  // update theCellContainer, fill more cells for example
  virtual StatusCode process( CaloConstCellContainer* theCellContainer) = 0;
};


#endif // not CALOINTERFACE_ICALOCONSTCELLMAKERTOOL_H
