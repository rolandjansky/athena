/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TagCollectionTest_page 

TagCollectionTest provides some simple tests of the POOL Collection 
integration with the ATLAS Athena framework. It also contains a
general utility to create a collection of event references and associated
metadata from one or more persistent event data files or from one or
more additional collections. The collection storage technologies presently 
handled by the utility include ROOT and MySQL. This package was originally 
implemented as part of an ATLAS offline database test suite used in the 
Data Challenge 2 exercises.

@section TagCollectionTest_Tests Tests

The table in coverage.html gives a summary of the test coverage for the Root Collection 
building using the RegistrationStream. 

@section TagCollectionTest_Components Components

The package includes 3 classes called EventTagWriter, EventTagReader,
RunEventTagWriter each of which inherits from the Gaudi Algorithm class. 

EventTagWriter creates dummy event tag
metadata via a random number generator, stores this metadata in a POOL 
attribute list object, and records the attribute list object in the
Athena transient data store (TDS) so that it may be retrieved further 
downstream and added to a POOL collection of event tag metadata along with a 
reference to the event's data header object.

EventTagReader opens an existing POOL collection of dummy event tag metadata
created by EventTagWriter, gets an iterator over the events
in the collection that satisfy a query, iterates over this subset of events
and prints out the list of event tag metadata values for each of these
events.

RunEventTagWriter is used via a job options file called 
MakeSimpleCollection.py to create a POOL collection of event references 
and their associated run and event number metadata from one or more files
containing persistent event data (or from one or more additional collections). 
The resulting collection may then be used to query the events by run 
and/or event number. The collections can be of type ROOT or MySQL. 
See the following section for further details on usage. Regression tests of 
this utility are included in the nightly build and are described below.

@section TagCollectionTest_JobOptions Job Options

The tests in TagCollectionTest are run via the following set of Athena Python 
job options files:

The files test_MakeDummyRootCollection.py, test_MakeDummyMySQLCollection.py
and test_MakeDummyMySQLltCollection.py create a ROOT, MySQL and
link table version MySQL collection, respectively, from the dummy event tag
metadata. Each of these job options files persistifies a single type of Athena
data object called EventInfo. The persistent data is accessible from the 
collection via the event data header references. 

@include test_MakeDummyRootCollection.py
@include test_MakeDummyRootCollection2.py
@include test_MakeDummyRootCollection3.py
@include test_MakeDummyRootCollection4.py
@include test_MakeDummyRootCollection5.py
@include test_MakeDummyMySQLCollection.py
@include test_MakeDummyMySQLltCollection.py

These job options files make use of the job options file 
MakeDummyCollection.py, which itself uses the EventTagWriter algorithm to 
randomly generate the dummy event tag metadata.

@include MakeDummyCollection_jobOptions.py
@include EventTagWriter_jobOptions.py

The files test_ReadDummyRootCollection.py, test_ReadDummyMySQLCollection.py and
test_ReadDummyMySQLltCollection.py read back the event tag
metadata of the ROOT, MySQL and link table version MySQL collections,
respectively, that were created by the first 3 job options files
mentioned above. 

@include test_ReadDummyRootCollection.py
@include test_ReadDummyMySQLCollection.py
@include test_ReadDummyMySQLltCollection.py

The job options file test_ReadDummyImplicitCollection.py tests direct 
access to the persistent data file itself by treating this file as an 
"implicit" POOL collection.

@include test_ReadDummyImplicitCollection.py

These job options files make use of the job options file ReadCollection.py,
which itself uses the EventTagReader algorithm to read back the events 
during the Athena event loop.

@include ReadCollection.py
@include EventTagReader_jobOptions.py

MakeSimpleCollection.py takes one or more collections as input, which may
simply consist of one or more persistent ROOT data files (i.e. "Implicit"
collections), runs the RunEventTagWriter algorithm and produces a collection 
of event references and their associated run and event number metadata. 

The number of events generated, POOL file catalog connection string, 
input collection name(s), input collection type, input collection
connection string, output collection name, output collection type, output 
connection string, output collection open mode and message service output 
level may all be changed on the athena command line. The default number
of events generated is 200000. The default file catalog connection string 
is "xmlcatalog_file:PoolFileCatalog.xml". The default input collection name
is "test.root". The default input collection type is "ImplicitROOT". The
default input collection connection string is "". The default output
collection name is "test.coll". The default output collection type is
"ExplictROOT". The default value for the collection open mode is 
"CREATE_AND_OVERWRITE" but this may be changed to "CREATE" so that 
the collection cannot be accidentally overwritten if it already exists. 
By default, the message service output level is set to WARNING. Other
possible values for the input collection type are "ExplicitROOT", 
"ExplicitMySQL" and "ExplicitMySQLlt" (the link table version of the
POOL MySQLCollection package). Other possible values for the output
collection type are "ExplicitMySQL" and "ExplicitMySQLlt". For MySQL
collections the database connection string must be specified.

For example, to create a MySQL collection called "MyCollection" consisting
of 1500 events read in successively from 2 data files called 
"MyDataFile1.root" and "MyDataFile2.root" one could do the following:

@verbatim
athena.py -c "EvtMax=1500; \
              Catalog='MyCatalog.xml'; \
              In=['MyDataFile1.root','MyDataFile2.root']; \
              Out='MyCollection'; \
              Type='ExplicitMySQL'; \
              Connect='mysql://MyUserName:MyPassWord@MyServer/MyDatabase'; \
              Mode='CREATE'; \
              Level=DEBUG" \
          MakeSimpleCollection.py
@endverbatim

where "MyServer" and "MyDatabase" are the MySQL server and database names, 
respectively, where the collection is to be stored and "MyUserName" and 
"MyPassWord" provide the necessary authentication to access the server.
The collection is registered in a local XML file catalog called 
"MyCatalog.xml". The message service output level in this example is set 
to DEBUG.

To read this collection back one could include the following lines in one's
job options file:

@verbatim
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
PoolSvc = Service( "PoolSvc" )
PoolSvc.ReadCatalog = [ "xmlcatalog_file:MyCatalog.xml" ]
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "MyCollection" ]
EventSelector.CollectionType = "ExplicitMySQL"
EventSelector.Connection = "mysql://MyUserName:MyPassWord@MyServer/MyDatabase"
EventSelector.Query = "MyQuery"
@endverbatim

where "MyQuery" is a cut on the attributes of "MyCollection". For instance, if
all events in run 3 except event number 8 are needed then one would use a
query like: "runNumber == 3 && eventNumber != 8".

@include MakeSimpleCollection_jobOptions.py

The following job options files provide nightly regression tests of the
MakeSimpleCollection utility:

@include test_MakeSimpleRootCollection.py
@include test_MakeSimpleMySQLCollection.py
@include test_MakeSimpleMySQLltCollection.py

The files test_MakeSimpleRootCollection.py, test_MakeSimpleMySQLCollection.py 
and test_MakeSimpleMySQLltCollection.py use the MakeSimpleCollection utility 
to create a ROOT, MySQL and link table version MySQL collection, respectively.

The files test_ReadSimpleRootCollection.py,
test_ReadSimpleMySQLCollection.py and test_ReadSimpleMySQLltCollection.py
read back the respective collections created by the first 3 job options files.

@include test_ReadSimpleRootCollection.py
@include test_ReadSimpleMySQLCollection.py
@include test_ReadSimpleMySQLltCollection.py

@section TagCollectionTest_SourceCode Source Code

TagCollectionTest resides in the package AtlasTest/DatabaseTest and can be 
browsed in the ATLAS CVS repository via, for instance, the following URL:

http://atlassw1.phy.bnl.gov/lxr/source/atlas/AtlasTest/DatabaseTest/TagCollectionTest

@section TagCollectionTest_RegressionTests Regression Tests

The test directory of the TagCollectionTest package contains the necessary
script, reference and pattern files to perform regression tests on the
package via the CMT call "gmake check". When these tests are run a log file is
created in the run directory which contains only those lines matching the 
patterns specified in a corresponding pattern file in the test directory.
The test passes if the log file is identical to its corresponding ref file
in the test directory. These tests have also been incorporated into the 
ATLAS nightly tests via the script TestTagCollectionTest.sh, which can be
found in the package AtlasTest/TestTools:

http://atlassw1.phy.bnl.gov/lxr/source/atlas/AtlasTest/TestTools/scripts/nightlies/

@section TagCollectionTest_ExtraPages Extra Pages

*/
