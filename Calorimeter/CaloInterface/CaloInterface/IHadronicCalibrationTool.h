/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_IHADRONICCALIBRATIONTOOL_H
#define CALOINTERFACE_IHADRONICCALIBRATIONTOOL_H
/***********************************************************************
Filename : IHadronicCalibrationTool.h
Author   : Frank Paige
Created  : August 2004

Modified : Kyle Cranmer Feb. 2005.  
           - Rename old ICellWeightTool to IHadronicCalibrationTool.  
           - Strip down interface in ICellWeightTool

DESCRIPTION:

     Pure interface base class for cell weight tools. Derived classes
must implement:
     etCell:   Return calibrated ET for given cell and weight.
     etCryo:   Return cryostat correction proportional to 
               sqrt(ET_ACCB3*ET_Tile1)
     jetScale: Return scale factor for jets
     wtCell:   Return just weight without ET factor
     wtCryo:   Return just weight without sqrt factor

Concrete implementations include:
     H1WeightTool2003:  Fit based on ET/cell used in 6.0.3/7.0.2.
     H1WeightToolAug04: Fit based on E/volume used in 8.x/9.x.
     H1WeightToolG4:    Fit based on E/volume for DC2/G4

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

#include "CaloInterface/ICellWeightTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/extend_interfaces.h"

class CaloCell;

class IHadronicCalibrationTool : virtual public extend_interfaces<ICellWeightTool>
{
 public:
  DeclareInterfaceID( IHadronicCalibrationTool, 1, 0 );

  virtual double etCell(const CaloCell* thisCell, double weight)  = 0;
  virtual double etCryo(double etAccb3, double etTile1)           = 0;
  virtual double jetScale(double e, double eta)                   = 0;
  virtual double wtCryo()                                         = 0;

};

#endif

