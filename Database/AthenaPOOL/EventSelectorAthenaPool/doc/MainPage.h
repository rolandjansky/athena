/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

AthenaPool is a toolkit to support use of LCG POOL as a persistence technology in Athena.
It consists of several packages:  PoolSvc, AthenaPoolCnvSvc, AthenaPoolUtilities,
DBDataModel, DBDataModelAthenaPool, %EventSelectorAthenaPool(this one), and OutputStreamAthenaPool.

The EventSelectorAthenaPool package reimplements the Gaudi IEvtSelector interface and more.
The package also contains equivalent support (via CondProxyProvider) for
conditions data input from POOL.

@subsection BackNavigation

The EventSelectorAthenaPool implements BackNavigation using the "Provenance" vector in the DataHeader.


@section Properties Settable EventSelectorAthenaPool Properties (Job Options)

@include EventSelectorAthenaPool_jobOptions.py


@section Documentation Additional Documentation

See the ATLAS User/Developer HowTo document for additional information.

The code can be browsed using LXR
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Database/AthenaPOOL/EventSelectorAthenaPool/)


@section Examples Examples
The package Database/AthenaPOOL/AthenaPoolExample contains running examples of algorithms writing and
reading Data Objects using AthenaPool.


@section Requirements Requirements

@include requirements


@section PackagesUsed Packages Used

@htmlinclude used_packages.html


@author Peter van Gemmeren <gemmeren@anl.gov>
*/
