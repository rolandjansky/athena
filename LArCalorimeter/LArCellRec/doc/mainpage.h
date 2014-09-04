/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage LArCellRec Package

This package contains the LAr cell reconstruction tools used to create LArCell objects

@section LArCellTool  LArCell Builderg Tools

LArCellBuilderFromLArHitTool and LArCellBuilderFromLArRawChannelTool
implement  ICaloCellMakerTool interface, and build LArCell from LArHit and LArRawChannel respectively.

@section LArCellCorrection LArCell Correction Tools

LArCellEmMiscalib LArCellHVCorrection LArCellRescaler LArG3Escale
implement CaloCellCorrection interface, and apply various modification to LArCells.

Details on CaloCell Reconstruction
https://twiki.cern.ch/twiki/bin/view/Atlas/CellReconstruction
*/
