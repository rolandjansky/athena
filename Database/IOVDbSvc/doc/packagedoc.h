/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Mainpage for doxygen

/**
@page IOVDbSvc_page package IOVDbSvc

IOVDbSvc is an Athena service interface to the Interval Of Validity
(IOV) database access, also known as the conditions database.

@author Antoine Perus, RD Schaffer, Richard Hawkings

This service is primarily for clients to setup the access to intervals
of validity for their particular conditions objects. This is done by
specifying which folders should be accessed using jobOptions. Details
are given below. For each folder, one may specify different
characteristics such as the folder tag, or special server if
different from the default server.

For each folder specified, IOVDbSvc interacts with the IOVSvc to
maintain a valid set of objects in the DetectorStore. The IOVSvc keeps
track of each transient DetectorStore object's validity, checked at
the begin of run and for each new event. For invalid objects, a client
retrieve will trigger a request to the IOVDbSvc (via updateAddress)
for a valid object. IOVDbSvc finds the interval of validity for the
corresponding folder (one data object per folder) from the db. This
IOV contains the validity interval AND information on how to get the
data object, which may either be the data itself or a reference to the
data object (e.g. a POOL ref). The data payload is dispatched to the
conversion services which will create the object to be recorded in the
DetectorStore.

The IOV DB maintains a description field for each folder. This is
used to maintain a set of folder characteristics which are used when
dealing with the decoding of the data object information. The
description is created at folder creation time (see
IOVRegistrationSvc). The description is a set of xml tags:
  - typeName:<pre>       - the type name, required information</pre>
  - symlinks:<pre>       - the extra StoreGate keys, if any</pre>
  - key:<pre>       - the SG key, optional, only needed if key != folder name</pre>
  - timeStamp:<pre>      - units of time axis, either run-event (default) or time</pre>
  - address_header:<pre>      - added by convention, prefix for the IOpaqueAddress</pre>


This service also provides an interface to access to the CondDBMgr for
other clients who need the extended interface for direct interaction
with the conditions db implementation. This for example is used by
IFillIOVTool to insert intervals of validity to the db. Through the
IIOVDbSvc interface, it is also possible to get a list of folders which are
configured to be read in (getKeyList method), determine the current IOV and 
associated tag for a folder (getKeyInfo method) and indicate a particular
conditions object is no longer required and can be deleted to save memory
(dropObject method).

@section IOVDbSvc_config Configuration

IOVDbSvc is normally configured using the IOVDbSvc.CondDB helper python class,
rather than directly setting job options. This is particularly important
for large applications such as RecExCommon, where the low-level database
access configuration is done automatically, and users just configure folders
to be used via joboption fragments such as:

<pre>
from IOVDbSvc.CondDB import conddb
conddb.addFolder('INDET','/Indet/Align')
</pre>
In this case, the first argument is the standard subdetector schema to 
connect to, and the second is the folder name, which is used to set
IOVDbSvc.Folders as described below. For more details, see 
<a href=https://twiki.cern.ch/twiki//bin/view/Atlas/CoolIOVDbSvcConfigurable>CoolIOVDbSvcConfigurable</a>

@section IOVDbSvc_jobopts Job option parameters

IOVDbSvc supports the following joboption parameters:

 - dbConnection: Give the default connection string in normal COOL syntax (see
<a href=https://twiki.cern.ch/twiki//bin/view/Atlas/CoolConnections>CoolConnections</a>). The default connection is used for any folders which do not have an explicit connection, and for writing via IOVRegistrationSvc

 - GlobalTag: Set the global top-level conditions tag used by IOVDbSvc. If none
is given, the tag recorded in the input file is used, if available.

 - Folders: A list of folder definitions. Each folder definition is a string
specifying the folder name, and often additional information (e.g. the
database connection to be used) via XML-style markups. More detail is given 
 below.

 - overrideTags. A list of strings specifying override tags to be used for 
particular folders. Each is of the form '&lt;prefix&gt;folder&lt;/prefix&gt; &lt;tag&gt;tagname&lt;/tag&gt;', which sets the tag for all folders matching the
prefix to the given tag value.

 - forceRunNumber / forceLumiblockNumber: Forces IOVDbSvc to use the given run/LB number when reading conditions data, rather than that from the EventSelector.

 - forceTimestamp: Forces IOVDbSvc to use the given timestamp when reading
conditions data, rather than that from the EventSelector. The timestamp is 
specified in seconds.

- manageConnections: Make IOVDbSvc open and close database connections as
needed, rather than keeping them open throughout the job (default True).

- managePoolConnections: Make IOVDbSvc manage the total number of POOL files
open at once, rather than keeping them open throughout the job.

- MaxPoolFilesOpen: Maximum number of POOL files allowed to be open at once 
(default 5).

- FoldersToMetaData: A list of string folder names which should be written
to file-level meta data on the output file.

- DumpKeys: Produce a printout of all the Storegate keys / folders being managed by IOVDbSvc, and the associated IOVs and tags at the end of the first event 
(default False).

- TimeStampSlop: Number of seconds subtracted from the start of timebased 
IOVs, to allow some 'slop' so that events with a slightly earlier timestamp
do not force reloading of conditions data. Needed for processing real data
where the events can be slightly out of order due to the differing processing
times in the HLT.

- CacheRun: Global override for number of runs to cache at once (see below). 
Default 0, i.e. use what is specified for the individual folder.

- CacheTime: Global override for number of seconds of timestamp data to cache 
(see below). Default 0, i.e. use what is specified for the individual folder.

- CacheAlign: Number of sub-intervals of cache to use for IOV alignment. Needed
when using Frontier - see below.

- OnlineMode: Flag to set when using online mode, disables some checks
for missing channels outside cache IOV.


@section IOVDbSvc_HowTo How to qualify the IOVDbSvc.Folders parameters in jobOptions:

There are essentially 5 markups available:

<table border="1" cellspacing="0" cellpadding="5">
  <tr><td><ul><li>the folder tag:</li></ul></td><td>&lt;tag&gt;...&lt;/tag&gt;</td></tr>
  <tr><td><ul><li>the time offset:</li></ul></td><td>&lt;offset&gt;...&lt;/offset&gt;</td></tr>
  <tr>
    <td>
      <table border="0" cellspacing="0" cellpadding="5">
        <tr><td><ul><li>the database connection specification:</li></ul></td></tr>
        <tr><td>The COOL database connection string syntax should be used.
</td></tr>
        </table>
      </td>
      <td>
        <table border="0" cellspacing="0" cellpadding="5">
          <tr><td>&lt;dbConnection&gt;...&lt;/dbConnection&gt; or &lt;db&gt;...&lt;/db&gt;<br /><br /><br /></br></td></tr>
        </table>
      </td>
    </tr>
    <tr><td><ul><li>a channel number selection:</li></ul></td><td>&lt;channelSelection&gt;...&lt;/channelSelection&gt;</td></tr>
  <tr><td><ul><li>an override run number:</li></ul></td><td>&lt;forceRunNumber&gt;...&lt;/forceRunNumber&gt;</td></tr>
</table>

The folder name can accept any characters, except for the markup
strings used for the optional parameters (<tag>, <offset>,
<dbConnection>, <channelSelection> and <forceRunNumber> at the current time).

One can have several markups in the same folder specification.

Example:
<pre>
IOVDbSvc.Folders += [ "/IOVDb:Test/* <tag>myTagDC2</tag>" ]
IOVDbSvc.Folders += [ "/IOVDb/Elsewhere/* <offset>0</offset>" ]
IOVDbSvc.Folders += [ "/IOVDb_Test/* <tag>myTag</tag> <dbConnection>myServer.athome.org:ConditionsDB_Test</dbConnection>" ]
IOVDbSvc.Folders += [ "/Test/* <tag>DC2</tag> <offset>-1</offset> <dbConnection>localServer:db</dbConnection>" ]
IOVDbSvc.Folders += [ "/Test/ <forceRunNumber>12345</forceRunNumber>" ]
</pre>

@section IOVDbSvc_ints Internal structure

This IOVDbSvc implementation is structured as four classes internally:
 - IOVDbSvc, which implements the service with appropriate interfaces, and 
maintains lists of folders and connections.
 - IOVDbFolder, which controls the access to one database folder and holds the corresponding data cache.
 - IOVDbConn, which controls access to one databsae schema and manages the
corresponding connection.
 - IOVDbParser, which implements a simple XML-like parser used in decoding
folder specifications and description strings.

IOVDbSvc is now heavily optimised to optimise database access and minimise 
the time idle database connections are open. From the list of folders given
in job options, IOVDbSvc builds a list of the required database connections
(each corresponding to a separate server/schema combination). During 
initialise, it accesses each schema in turn and processes all folders belonging
to that schema one after the other. This processing includes checking the
folder exists in the database, and reading the folder description string
to determine the type of data. The connection for each schema is kept open
until just after the one for the next schema has been opened (so they are
overlapped slightly), which ensures CORAL uses the same physical database 
connection for all the schema, saving some server resources. Once the data 
for all folders/schemas have been read, the last database connection is
closed.

IOVDbSvc next accesses a database when the first request to read data from
a particular folder is
generated through a IOVSvc callback or client direct retrieve through 
Storegate. At this point, the run/LB number and timestamp are known, so 
payload data can be retrieved from COOL for the corresponding validity key 
(either run/LB or timestamp).  A connection to the
appropriate schema is opened, and data is read for all configured folders
associated to that schema. The data is read into memory caches managed by
the IOVDbFolder objects, so does not immediately appear in Storegate. Data is
read not only for the validity key required, but for a range around this
value, to reduce the number of times data has to be read from database. 
Storegate is then filled by using the appropriate payload data from the 
cache (using reference counted pointers to avoid actually copying the data),
and the database connection is closed. Requests for data in other folders 
from the same schema will then be satisfied from the cache, without the need
to retrieve more data from the database. Requests for data from different 
schemas will generate further retrievals from the database to fill caches,
automatically grouping the requests by schema, so that only one connection to
a database is open at once. In this way, the caches for all folders 
will be filled over the course of the first event.

During subsequent event processing, the IOV for conditions data in a particular
folder may go out of scope, meaning new conditions data has to be loaded into
Storegate. The new data may well be available in the cache, in which case
Storegate will be filled from the cache and no new database request will be
generated. If not, a new connection will be made to the database, and the 
cache will be 'refilled' starting from the current event validity key. At this
point, all other folders associated with this schema will also be checked,
and their caches updated from the database if neccessary, again making maximum
use of the database connection while it is open.

@section IOVDbSvc_configcache Configuring cache behaviour

The length of the caches can be controlled in various ways. If a &lt;cache&gt; 
specification is present in the folder meta-data or folder job options, 
this is used to set the cache length for this folder
(in units of seconds for timestamp-based folders and LBs for run-LB folders). 
If both are present, the job options take priority, allowing the default
settings in the meta-data to be overridden.
Typically DCS folders are setup with 600 second length caches in the folder
meta-data. If no specification is present, a default length of 600 seconds
or one complete run is used. However, the CacheRun and CacheTime job option
parameters can be used to globally override this:
 - IOVDbSvc.CacheRun=M
 - IOVDbSvc.CacheTime=N

sets the cache length for all run-LB indexed folders to M runs, and for all
timestampe indexed folders to N seconds. This can be used for example to 
increase the cache length for a job which is processing data over a large
time period (e.g. a sparse skim of events).

If these parameters are not set, IOVDbSvc will also automatically double the 
length of the internal cache each time it has to be refilled, so trying to 
dynamically adapt the size of the cache to the job being processed.

@section IOVDbSvc_frontiercache Cacheing and Frontier

If data is being accessed via Frontier and Squid caches, it is important that
many Athena jobs make exactly the same data requests, even if their
first events are slightly different in time. This can be helped
by 'cache alignment'. For example, if the cache length is 600, jobs where the
first event is in the range 0-299 can make a cache request for data from
0-599, jobs where the first event is in the range 300-599 should request
300-899, 600-899 should request 600-1199 and so on. In this way, the Squid
cache will see requests only for 0-599, 300-899, 600-1199 etc and hopefully 
many of these requests can be satisfied from the cache. Otherwise, it will
see requests for 0-599, 20-619, 50-649 etc which will all 'look' different and
hence be referred back to the Frontier/Oracle server. This comes at the expense
of some extra data being stored in the Athena client caches, which will contain
some 'redundant' data which is probably never used.

This mode can be enabled by setting:
 - IOVDbSvc.CacheAlign=N

which will cause queries to be 'quantised' in N submultiples of the cache 
length (N=2 corresponding to queries at 0 and 300 for the above 600 second 
cache example). The best value of N is a compromise between more cacheable
queries and more 'redundant' data - N=3 may be optimial. The default value
of N=0 means no alignment is done, which is appropriate for more traditional
database servers.

@section IOVDbSvc_dblog Database read logging

IOVDbSvc now prints a summary during finalize of how much data has been 
read from each folder, which can be used for debugging, spotting unncessary
reads and opportunities for improving cache configuration. An example:

<pre>
IOVDbSvc  INFO Folder /GLOBAL/BField/Map (AttrListColl) db-read 1/1 objs/chan/bytes 1/2/64
IOVDbSvc  INFO Folder /GLOBAL/TrackingGeo/LayerMaterial (PoolRef) db-read 1/1 objs/chan/bytes 1/1/232
IOVDbSvc  INFO Folder /Indet/Align (PoolColl) db-read 1/1 objs/chan/bytes 34/34/5440
IOVDbSvc  INFO Folder /Indet/Beampos (AttrList) db-read 1/1 objs/chan/bytes 1/1/40
IOVDbSvc  INFO Folder /SCT/DAQ/Configuration/MUR (CoraCool) db-read 1/1 objs/chan/bytes 1/1/203328
IOVDbSvc  WARNING Folder /SCT/DAQ/Configuration/RODMUR (CoraCool) db-read 1/0 objs/chan/bytes 1/1/19768
</pre>

Each foldername is printed, followed by information about the folder type, and
'dbread A/B objs/chan/bytes C/D/E'. A indicates the number of times data is 
read from the database, and B the number of times data is requested (so B-A
is the number of times it was read from the cache without going back to the
database). C indicates the number
of individual  objects read from the database and D the number of channels in the folder. E represents the total number of bytes of payload data read from 
COOL, giving an indication of the overall amount of data access.

If B is zero and A>0, this indicates data was read from the database, but never
retrieved from Storegate, i.e. a folder is configured to be accessed, but 
never used. This represents a 'waste' of database resources due to a badly
configured job, so a WARNING is printed.

If A is large, this indicates many separate reads from the database during
the course of the job, suggesting the folder cache size may need to be
increased.

After all folders have been printed, a further summary of the total payload 
from COOL is printed, and for each connection, the number of times each 
connection was accessed and the number of associated folders.

<pre>
IOVDbSvc                                   INFO Total payload read from COOL: 16202709 bytes
IOVDbSvc                                   INFO Connection sqlite://;schema=mycool.db;dbname=OFLP200 : nConnect: 0 nFolders: 0
IOVDbSvc                                   INFO Connection COOLONL_INDET/OFLP200 : nConnect: 2 nFolders: 4
IOVDbSvc                                   INFO Connection COOLONL_TRT/OFLP200 : nConnect: 2 nFolders: 7
IOVDbSvc                                   INFO Connection COOLONL_GLOBAL/OFLP200 : nConnect: 2 nFolders: 2
</pre>

Normally, the number of connections will be at least 2, one connection
during initialisation to read the meta-data, and once during the first event. 
Larger numbers indicate the schema was reaccessed later in the job to
read more data.

 
@section IOVDbSvc_joFile The example jobOptions file:
@include IOVDbSvc_jobOptions.py

*/
