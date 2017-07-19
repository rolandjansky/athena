/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page CLIDSvc_page 

Tools for generating and managing CLIDs. In particular the include file 
CLASS_DEF.h provides the CLASS_DEF macro that allows to associate a
CLID to a type. 
The clid  script provides command-line access to the CLID
"database" clid.db and uses a hash function to generate a
unique CLID (ignoring the possibility of "hash collisions"). 
For usage enter "clid -h". 
 
Notice that currently (13.0.X) clid.db is not used to verify at run-time
in athena that there are no duplicates in CLID assignments. The
problem is that we have no mechanism to generate clid.db as part of
the build process. Also since everyone seems to be using the clid
script now to assign new CLIDs to their data objects we haven't
observed a conflict in CLID assignment since a couple of release cycles.

More information on CLID assignment is available from
http://atlas-computing.web.cern.ch/atlas-computing/packages/edm/clid.php.



@section CLIDSvc_Tests Unit Tests

The directory test contains test programs that can be run issuing
a "gmake check" (after "source setup.sh"). 
 - CLIDRegistry_test.cxx
 - clid_unittest.py




@section CLIDSvc_Docs More Documentation

The code can be browsed using LXR 
(http://atlassw1.phy.bnl.gov/lxr/source/atlas/Control/CLIDSvc/)


@author Craig Tull <CETull@lbl.gov>
@author Paolo Calafiura <PCalafiura@lbl.gov>
*/
