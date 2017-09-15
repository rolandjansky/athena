/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page EventSelectorAthenaPool_page 

AthenaPool is a toolkit to support use of LCG POOL as a persistence technology in Athena.
It consists of several packages:  PoolSvc, AthenaPoolCnvSvc, AthenaPoolUtilities,
DBDataModel, DBDataModelAthenaPool, %EventSelectorAthenaPool(this one), and OutputStreamAthenaPool.

The EventSelectorAthenaPool package reimplements the Gaudi IEvtSelector interface and more.
The package also contains equivalent support (via CondProxyProvider) for
conditions data input from POOL.

@subsection BackNavigation

The EventSelectorAthenaPool implements BackNavigation using the "Provenance" vector in the DataHeader.


@section EventSelectorAthenaPool_Properties Settable EventSelectorAthenaPool Properties (Job Options)

@include EventSelectorAthenaPool_jobOptions.py


@section EventSelectorAthenaPool_Documentation Additional Documentation

See the ATLAS User/Developer HowTo document for additional information.

The code can be browsed using LXR
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Database/AthenaPOOL/EventSelectorAthenaPool/)


@section EventSelectorAthenaPool_Examples Examples
The package Database/AthenaPOOL/AthenaPoolExample contains running examples of algorithms writing and
reading Data Objects using AthenaPool.








@author Peter van Gemmeren <gemmeren@anl.gov>
*/
