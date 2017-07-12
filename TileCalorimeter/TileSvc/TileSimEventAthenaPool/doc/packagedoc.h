/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TileSimEventAthenaPool_page TileEventAthenaPool Package

This package handles the conversion of Tile data classes to Pool format.

@author Alexander Solodkov <sanya.solodkov@cern.ch>

In order to write the Tile EDM (Event Data Model) objects, defined in the
TileEvent and TileSimEvent packages, into Pool files (ESD, AOD, ...), pool
converters are needed. In many cases, the persistent version of the object can
be a simple copy of the transient one, so the automatic conversion will be
enough. However, in some cases a custom converter is needed if, for instance,
type casts have to be used.

More details about Pool coversion here:
https://twiki.cern.ch/twiki/bin/view/Atlas/WriteReadDataViaPool

@section TileSimEventAthenaPool_Automatic Pool conversion
The AthenaPoolCnvSvc package provides generic converters via a CMT poolcnv
pattern. The Tile classes that use this are:  

  
@section TileSimEventAthenaPool_Custom Pool conversion

The other Tile EDM classes have custom converters, defined in this package.

TileSimEvent/TileHitVector

Both TileHit and TileHitVector (and soon, TileDigits), have implemented
Transient/Persistent separation. More details here:
https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation

*/
