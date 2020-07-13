/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICELLWEIGHTTOOL_H
#define CALOINTERFACE_ICELLWEIGHTTOOL_H
/***********************************************************************
Filename : ICellWeightTool.h
Author   : Frank Paige
Created  : August 2004

Modified : Kyle Cranmer Feb, 2005
           - old ICellWeightTool now called IHadronicCalibrationTool.
           - this interface only has wtCell( cell ) now


DESCRIPTION:

     Pure interface base class for cell weight tools. Derived classes
must implement:
     wtCell:   Return just weight without ET or energy factor



Note the navigation weight factor is *not* in wtCell. One must include
this to get the right energy for a jet, e.g.

  NavigationToken<CaloCell,double> cellToken;
  NavigationToken<CaloCell,double>::const_iterator cell = cellToken.begin();
  NavigationToken<CaloCell,double>::const_iterator cellE = cellToken.end();
  for( ; cell != cellE ; ++cell ) {
    ...
    double cellWt = cellToken.getParameter(cell);
    double eCalib = cellWt * (*cell)->e() * wtCell(*cell);
    ...
  }

It *is* included via the second argument of etcell, which is used by
JetRec::JetCellCalibratorTool.

***********************************************************************/

#include "GaudiKernel/IAlgTool.h"

class CaloCell;

class ICellWeightTool : virtual public IAlgTool
{
 public:
  DeclareInterfaceID( ICellWeightTool, 1, 0 );

  virtual double wtCell(const CaloCell* thisCell) const = 0;
};

#endif

