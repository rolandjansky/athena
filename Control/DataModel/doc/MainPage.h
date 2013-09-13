/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

   \mainpage

%StoreGate is a toolkit to implement the Atlas Data Model.
It consists of three packages %StoreGate, DataModel(this one) and SGTools.

This package contains the tools to design Data Objects to be used in Athena

\section links DataLink and ElementLink
these are two class templates to write persistable inter-object
references. They can be used as c++ pointers and read and written
to/from disk. For details see the doc of the two classes, the
StoreGate user guide (link below) and the examples in AthExStoreGateExamples

\section pool DataPool
a memory pool typically used by the containers above to speed-up
element allocation

\section Documentation Documentation

A tutorial is available from 
http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/OO/architecture/EventDataModel/Tutorial/

The user guide is part of the Data Model User Guide which is 
maintained as a CVS package
(http://atlassw1.phy.bnl.gov/lxr/source/atlas/AtlasDoc/doc/DataModel/). 

More information is available from the Data Model page 
(http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/OO/architecture/EventDataModel/index.html).

The code can be browsed using LXR 
(http://atlassw1.phy.bnl.gov/lxr/source/atlas/Control/StoreGate/)

To generate doxygen doc, run (from the cmt dir) gmake doxygen and point 
your browser to .../doc/Doxygen/html/index.html

\section Examples Examples

The package Control/AthenaExamples/AthExStoreGateExamples contains 
running examples of algorithms accessing Data Objects using StoreGateSvc.
This examples are (very close to) the ones described in the tutorial above

\section tests Unit Tests

The directory test contains several test programs that can be run issuing
a "gmake check" or a ../run/runUnitTests.sh 


\author Paolo Calafiura <Paolo.Calafiura@cern.ch>
\author Hong Ma <hma@bnl.gov>
\author Srini Rajagopalan <srinir@bnl.gov>
*/
