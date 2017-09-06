/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page StoreGate_page 

%StoreGate is a toolkit to implement the Atlas Data Model.
It consists of four packages %StoreGate(this one), DataModel, SGComps, and SGTools.

This package contains the client API to the Transient Data
Store. The core data access API is provided by
StoreGateSvc. StoreGateSvc manages the Data Objects in transient form
in collaboration with DataStore (SGTools pkg). The
transient/persistent conversion of each data object type is managed by 
a DataProxy (again in SGTools). The DataProxy instances for object on disk
are added by a list of IAddressProvider services managed by the 
ProxyProviderSvc (in SGComps).

ActiveStoreSvc is not for general use: it allows to
access the instance of StoreGateSvc which is currently active (this is
used by classes dealing with multiple event stores, e.g. for pile-up studies).

@section StoreGate_Examples Examples

The package Control/AthenaExamples/AthExStoreGateExamples contains 
running examples of algorithms accessing Data Objects using StoreGateSvc.
This examples are (very close to) the ones described in the tutorial above

@section StoreGate_tests Unit Tests

The directory test contains several test programs that can be run issuing
a "gmake check". 
 StoreGateSvcClient_test.cxx should cover most of the high-level client API.


@section StoreGate_Documentation 

The user guide is part of the Data Model User Guide which is 
maintained in CVS
(http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/AtlasDoc/doc/DataModel/DataModel.pdf?rev=1.3). 

More information is available from the Athena wiki 
(https://twiki.cern.ch/twiki/bin/view/Atlas/AthenaFramework#StoreGate_The_Data_Model_Archite)

The code can be browsed using LXR 
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Control/StoreGate/)


\author Paolo Calafiura <Paolo.Calafiura@cern.ch>
\author Hong Ma <hma@bnl.gov>
\author Srini Rajagopalan <srinir@bnl.gov>
*/
