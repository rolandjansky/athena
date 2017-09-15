/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

This package is used to provide the Transient/Persistent separation for TileCal classes.

@page TileTPCnv_page 

@author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
@author Alexander Solodkov <Sanya.Solodkov@cern.ch>

@section TileTPCnv_Intro Introduction

  - Two separate data models are used:
    - Transient data model: principal, used almost everywhere
    - Persistent data model: supplementary, used for data storage
  - The primary motivation of T/P separation is to provide backwards compatibility (i.e. when the transient representation of a TileEvent class is modified, the old persistent representations can still be read)
  - Additionally, the persistent model introduces class versioning (ClassName_p1, ClassName_p2, etc.)
  - It allows more efficient and performant data storage, important to include information in ESD or AOD files
  - Data is being copied between transient and persistent objects before writing and after reading by the T/P converters

@section TileTPCnv_Info Related Packages

  The persistent classes and containers in TileTPCnv package have the correspondent transient classes and containers in TileSimEvent and TileEvent packages:

  - TileHit: stores the total energy visible by one PMT
  - TileL2: stores TileMuId and \f$ E_{\rm{T}} \f$ quantities as computed in the ROD DSPs
  - TileMu: stores TileMuId (processed offline) related information
  - TileCosmicMuon: stores information on TileMuonFitter results
  - TileTTL1Cell: stores the trigger tower energy, average time and quality factor built from Tile cell information

  Note that a GUID is assigned for each transient class in TileSimEvent/selection.xml and TileEvent/selection.xml files.

  The AthenaPool converters, where these GUIDs are used, are stored in TileEventAthenaPool package.

@section TileTPCnv_Class Persistent Classes

  The following persistent classes are implemented in this package:

  - TileHit_p1
  - TileL2_p1
  - TileMu_p1
  - TileCosmicMuon_p1
  - TileTTL1Cell_p1

  This persistent classes are meant to reduce the size of the objects, i.e. converting variables from double to float, for data storage. Note that a GUID is assigned for each persistent class in TileTPCnv/selection.xml file.

@section TileTPCnv_Cnt Persistent Containers

  The following persistent containers are implemented in this package:

  - TileHitVector_p1
  - TileL2Container_p1
  - TileMuContainer_p1
  - TileCosmicMuonContainer_p1
  - TileTTL1CellContainer_p1

@section TileTPCnv_Cnv T/P Converters
  
  The following T/P converters are implemented in this package:

  - TileHitCnv_p1
  - TileL2Cnv_p1
  - TileMuCnv_p1
  - TileCosmicMuonCnv_p1
  - TileTTL1CellCnv_p1

  In the T/P converters, the methods transToPers and persToTrans are used for the conversion from transient to persistent or vice versa.

@section TileTPCnv_CntCnv T/P Container Converters

  The following T/P container converters are implemented in this package:

  - TileHitVectorCnv_p1
  - TileL2ContainerCnv_p1
  - TileMuContainerCnv_p1
  - TileCosmicMuonContainerCnv_p1
  - TileTTL1CellContainerCnv_p1


*/
