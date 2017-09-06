/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page DBReplicaSvc_page DBReplicaSvc package

@section DBReplicaSvc_introductionDBReplicaSvc Introduction

The DBReplicaSvc is used by ATLAS database applications which make database
connections through CORAL. It implements the coral::IReplicaSortingAlgorithm
interface, which allows it to choose and order a set of database replicas 
which are specified by CORAL. It is normally loaded by PoolSvc when the
latter sets up CORAL, and configures the replica sorting service to be used.

The choice of replicas is made based on the hostname on which the job is 
running (looked up via the $HOSTNAME environment variable). A configuration
file supplies an ordered list of database replica server names which are 
appropriate for different domains. Those which match the current domain are
considered as possible replicas when CORAL asks for the list of replicas
to be filtered and sorted.

@section DBReplicaSvc_configurationDBReplicaSvc Configuration file

The configuration file is normally called dbreplica.config, and is looked for 
via PathResolver. It will be kept separately, in the AtlasAuthentication
package. A typical configuration file looks like:
<pre>
cern.ch = ATLAS_COOLPROD atlas_dd
it = CNAF PIC
es = PIC CNAF
uk = RAL LYON
cz ch pl de = FZK SARA
nl ru su = SARA FZK
tw au = ASGC LYON
fr jp cn ro = LYON FZK ASGC
ca = TRIUMF BNL
gov edu org = BNL TRIUMF
default = ATLAS_COOLPROD FZK BNL
</pre>
Each line gives a list of network domains, followed by = and a list of server
names. Longer domain name matches are treated with higher priority, so 
jobs running in the cern.ch domain will use ATLAS_COOLPROD before FZK in
the above example. The 'default' line will be used for domains which do not
match at all.

By default, any SQLite file replicas offered by CORAL will be used before 
server-based replicas. This can be turned off separately for COOL replicas
(recognised by 'ALLP' in the file name) and geometry database replicas
(recognised by 'geomDB' in the filename) using job options. This can be 
used to force jobs which would normally read data from the database release
(which uses SQLite files) to instead use an Oracle (or other server) replica.

@section DBReplicaSvc_joboptDBReplicaSvc Job options

The following job options are defined:
 - ConfigFile (default 'dbreplica.config') - name of the configuration file
 - TestHost (default '') - if set, this is used instead of $HOSTNAME, allowing testing of the replica selections
 - UseCOOLSQLite (default 'True') - use SQLite replicas for COOL if offered
 - UseGeomSQLIte (default 'True') - use SQLite replicas for geometry if offered
 - DisableFailover (default 'False') - only allow one (primary) Oracle replica





*/
