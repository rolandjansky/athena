/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOBADCELLHELPER_H
#define CALOBADCELLHELPER_H
//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloBadCellHelper.h 1.0 2010-04-16 22:19:05 menke Exp $
//
// Description: tests a cell for badness and returns true if it can not
//              be guessed from L1 trigger info
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke, Guillaume Unal
//
//-----------------------------------------------------------------------

#include "CaloEvent/CaloCell.h"

struct CaloBadCellHelper
{
 public:
  static bool isBad(const CaloCell * pCell, bool treatL1PredictedCellsAsGood) {
    bool badcell=false;
    if (pCell->badcell() ) { 
      if (treatL1PredictedCellsAsGood) {
	const CaloDetDescrElement* caloDDE = pCell->caloDDE();
	if (caloDDE) {
	  int theCellSubCalo = caloDDE->getSubCalo();
	  if ( (theCellSubCalo == CaloCell_ID::TILE)  ||
	       !((pCell->provenance()) & 0x0200) ) 
	    badcell=true;
	}
      }
      else 
	badcell=true;
    }
    return badcell;
  }
};

#endif // CALOBADCELLHELPER_H

