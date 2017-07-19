/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

This package is used to provide the Transient/Persistent separation for TileCal simulation hits classes.

@page TileSimEventTPCnv_page 

@author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
@author Alexander Solodkov <Sanya.Solodkov@cern.ch>

@section TileSimEventTPCnv_Intro Introduction

  - Two separate data models are used:
  - Transient data model: principal, used almost everywhere
  - Persistent data model: supplementary, used for data storage
  - The primary motivation of T/P separation is to provide backwards compatibility (i.e. when the transient representation of a TileEvent class is modified, the old persistent representations can still be read)
  - Additionally, the persistent model introduces class versioning (ClassName_p1, ClassName_p2, etc.)
  - It allows more efficient and performant data storage, important to include information in ESD or AOD files
  - Data is being copied between transient and persistent objects before writing and after reading by the T/P converters

@section TileSimEventTPCnv_Info Related Packages

  The persistent classes and containers in TileTPCnv package have the correspondent transient classes and containers in TileSimEvent packages:

  - TileHit: stores the total energy visible by one PMT

  Note that a GUID is assigned for each transient class in TileSimEvent/selection.xml files.

  The AthenaPool converters, where these GUIDs are used, are stored in TileSimEventAthenaPool package.

@section TileSimEventTPCnv_Class Persistent Classes

  The following persistent classes are implemented in this package:

  - TileHit_p1

  This persistent classes are meant to reduce the size of the objects, i.e. converting variables from double to float, for data storage. Note that a GUID is assigned for each persistent class in TileSimEventTPCnv/selection.xml file.

@section TileSimEventTPCnv_Cnt Persistent Containers

  The following persistent containers are implemented in this package:

  - TileHitVector_p1

@section TileSimEventTPCnv_Cnv T/P Converters
  
  The following T/P converters are implemented in this package:

  - TileHitCnv_p1

  In the T/P converters, the methods transToPers and persToTrans are used for the conversion from transient to persistent or vice versa.

@section TileSimEventTPCnv_CntCnv T/P Container Converters

  The following T/P container converters are implemented in this package:

  - TileHitVectorCnv_p1


*/
