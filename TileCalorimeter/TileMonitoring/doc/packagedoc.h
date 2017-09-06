/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileMonitoring_page TileMonitoring Package

This package provides Monitoring tools and top algorithm for TileCalorimeter, plus some root macro and TileCosmicView


@author Luca Fiorini <Luca.Fiorini@cern.ch>
@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Luciano Filho <lucianof@lps.ufrj.br>

@section TileMonitoring_Package_Structure 
The package contains a top algorithm, @c TileCosmicHisto, that collected and control the algorithm tools.
Algorithm tools exist for calibration runs reconstruction (for the so called Team5 analysis) and for cosmic runs.
The monitoring is done in the algorithm tools and they are organised by input container:
TileDigitsMonTool
TileRawChannelMonTool
TileCellMonTool
TileClusterMonTool
TileMuIdMonTool
TileMuonFitterMonTool

They all inherits from a common MonTool: TileFatherMonTool.
The base class is Control/AthenaMonitoring/ManagedMonitorToolBase

Calibration tools
TileDigitsMonTool and TileRawChannelMonTool inherit from a different class  TilePaterMonTool, that uses the 
old class Control/AthenaMonitoring/MonitorToolBase, because there is no need of the Managed sofistications for calibratrion
analysis

@section TileMonitoring_TileCosmicViewer 
TileCosmicViewer is a root-based tool developed by Luciano Filho to display Tile and LAr cells and TileMuonFitter objects.


*/
