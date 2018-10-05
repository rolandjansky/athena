/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARREADOUTGEOMETRY_HECCELLCONSTLINK_H
#define LARREADOUTGEOMETRY_HECCELLCONSTLINK_H

#include "LArReadoutGeometry/HECCell.h"
#include "GeoModelKernel/ConstLink.h"

/**
 * @Class: HECCellConstLink 
 * 
 *	Smart Pointer to HEC Cells.  This reference counted link
 *	allocates on demand. It audits the total pointer count
 *	and collects the garbage when nobody's looking.
 */

typedef ConstLink<HECCell> HECCellConstLink;

#endif
