/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

AthenaPool is a toolkit to support use of LCG POOL as a persistence technology in Athena.
It consists of several packages:  PoolSvc, AthenaPoolCnvSvc, AthenaPoolUtilities,
%DBDataModel(this one), DBDataModelAthenaPool, EventSelectorAthenaPool, and OutputStreamAthenaPool.

The DBDataModel package contains the definition of the persistent Data Model classes such as
DataHeader and DataHeaderElement.

@subsection DataHeader

The DataHeader class contains a status flag, a processing tag string and two vectors of
DataHeaderElements.
Each DataHeaderElement stores the ClassID (and a set of symlinked ClassIDs), Key (plus a set of
StoreGate aliases) and Token string of a DataObject.
One vector stores DataHeaderElements for all persistified data objects (called the "DataObject" vector)
and the other one stores the same information for all upstream DataHeader ("Provenance" vector).
A DataHeader is written automatically by AthenaPOOL for each "event". It is used to load the persistified
DataObject from POOL and provide support for BackNavigation.


@section Properties Settable DBDataModel Properties (Job Options)

This package has no properties and no jobOptions.


@section Documentation Additional Documentation 

See the ATLAS User/Developer HowTo document for additional information.

The code can be browsed using LXR 
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Database/AthenaPOOL/DBDataModel/)
   

@section Examples Examples
The package Database/AthenaPOOL/AthenaPoolExample contains running examples of algorithms writing and
reading Data Objects using AthenaPool. 


@section Requirements Requirements 

@include requirements 


@section PackagesUsed Packages Used 

@htmlinclude used_packages.html


@author Peter van Gemmeren <gemmeren@anl.gov>
*/
