/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page PoolSvc_page 

AthenaPool is a toolkit to support use of LCG POOL as a persistence technology in Athena.
It consists of several packages:  PoolSvc(this one), AthenaPoolCnvSvc, AthenaPoolUtilities,
DBDataModel, DBDataModelAthenaPool, EventSelectorAthenaPool, and OutputStreamAthenaPool.

PoolSvc provides mechanisms for POOL connection management, input and output
catalog specification, and querying and control of POOL configuration options.
PoolSvc is used via its IPoolSvc interface.


@section PoolSvc_Properties Settable PoolSvc Properties (Job Options)

Settable PoolSvc properties are in PoolSvc_jobOptions.py:

@include PoolSvc_jobOptions.py


@section PoolSvc_Documentation Additional Documentation

See the ATLAS User/Developer HowTo document for additional information.

The code can be browsed using LXR
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Database/AthenaPOOL/PoolSvc)


@section PoolSvc_Examples Examples
The package Database/AthenaPOOL/AthenaPoolExample contains running examples of algorithms writing and
reading Data Objects using PoolSvc.








@author Peter van Gemmeren <gemmeren@anl.gov>
*/
