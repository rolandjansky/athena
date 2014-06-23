/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage LArRecEvent Package

This package contains reconstruction data classes specific for LAr 

@author Hong Ma <hma@bnl.gov>

@section LArCell LArCell 
  
LArCell is a derived class that extends CaloCell for LAr readout cells.

LArCellContainer : obsolete class.  LArCells are stored in CaloCellContainer.

LArCellIDC :  IdentifiableContainer for LArCell. Only used in Event Filter

@section LArFebEnergy LArFebEnergy

LArFebEnergy holds the sum of Ex, Ey and Ez for each FEB, which can be prepared in ROD.

LArFebEnergyCollection is a container class for LArFebEnergy


*/
