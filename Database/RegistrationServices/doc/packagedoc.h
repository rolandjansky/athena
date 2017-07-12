/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@page RegistrationServices_page 

RegistrationServices is a package containing services for different
database-related registrations. Currently there are three services:

<ul>
  <li>RegistrationStream</li> 
    <ul>
      <li>The RegistrationStream will find desired references in StoreGate.</li>
      <li>Each RegistrationStream uses a single RegistrationStream<x>Tool to write the output.</li>
    </ul>
  <li>IOVRegistrationSvc</li>
    <ul><li>
      The IOVRegistrationSvc is used to register time-varying conditions data into an Interval Of Validity (IOV) database.
      </li>
    </ul>
  <li>OutputConditionsAlg</li>
    <ul><li>
      The OutputConditionsAlg
algorithm provides a 'complete' algorithm to store conditions data objects:
first writing them on an output stream (using the AthenaOutputStreamTool), 
then (optionally) registering the
corresponding references into the IOV database using IOVRegistrationSvc.
    </li>
    </ul>
  <li>InputCollectionMetadataCopy</li>
    <ul><li>TagFile incident handler</li></ul>
    <ul><li>
      Copies the metadata from a Collection used as input into the Metadata store for inclusion 
in any output Collections.
    </li></ul>
  <li>RegistrationStreamDefMeta</li>
  <ul>
    <li>Simple algorithm with properties which are written into a CollectionMetadataContainer in the 
MetaDataStore.</li>
    <li>Properties</li>
    <ul>
      <li>StoreName</li>
      <li>Pass</li>
      <li>PassName</li>
      <li>Stream</li>
      <li>ConfigTag</li>
      <li>Project</li>
    </ul>
  </ul>
</ul>

All of these services are documented below.

@section RegistrationServices_RegistrationStream RegistrationStream

Whenever an AthenaOutputStream writes out an event, there is
always a DataHeader object written which contains references to all
other DataObjects written out in the event. As well, a DataHeader may
contain one or more references to events written in a previous
processing step. This allows, for example, navigation from AOD to ESD
to RAW data. Note that the references are implemented as POOL tokens
which are globally unique ids as character strings.

RegistrationStreams write out Event Collections. 
Event Collections are lists of references to DataHeader's ('events') 
and any associated metadata (called a 'Tag') that the creator 
has accumulated and associated with the reference using an 
(Athena)AttributeList. The Event Collection may then be used to
select events according to the Tag values and then make
sub-collections and/or navigate to the events. 

@subsection Properties Properties

A RegistrationStream has the following settable properties.
<ul>
  <li>ItemList</li>
  <li>CollectionType (DEFAULT: ExplicitROOT) DEPRECATED, should use Tool properties directly.</li>
  <li>Connection (DEFAULT: '') DEPRECATED, should use Tool properties directly.</li>
  <li>OutputCollection (DEFAULT: '') DEPRECATED, should use Tool properties directly.</li>
  <li>CollectionOpenMode (DEFAULT: CREATE_AND_OVERWRITE) DEPRECATED, should use Tool properties directly.</li>
  <li>WriteInputDataHeader (DEFAULT: false) whether to write a pointer to the input DataHeader</li>
  <li>WriteAllProv (DEFAULT: true)</li>
  <li>IncludeProvStages (DEFAULT: '') explicit list of provenance stages to include if not WriteAllProv</li>
  <li>ExcludeProvStages (DEFAULT: '') explicit list of provenance stages to exclude</li>
  <li>PersistencySvc (DEFAULT: 'EventPersistencySvc') DEPRECATED, should use Tool properties directly.</li>
  <li>StoreName (DEFAULT: 'StoreGateSvc')</li>
  <li>Tool (DEFAULT: RegistrationStreamLCGTool)</li>
</ul>

@subsection Sequencing Sequencing 

Athena supports several predefined sequences AthAlgSeq, AthOutSeq, and AthRegSeq. 
RegistrationStreams should be placed in the RegSeq to make sure they follow the data 
output streams. 

@subsection Filtering Filtering

Multiple RegistrationStreams may be used. 
Each RegistrationStream has a Tool which writes to a separate collection. 
Using the fact that a RegistrationStream is also a FilteredAlgorithm one 
can add corresponding selection criteria to decide whether each event being
processed should be registered or not. The selection is done by one
or more algorithm which has run before the RegistrationStream. The
selection algorithm uses:

@verbatim  /// Set the filter passed flag to the specified state
  virtual StatusCode setFilterPassed( bool state );@endverbatim
to set/unset its filter flag. A RegistrationStream may have one or
more algorithms as filters, and the filters may be one of three types:

- Accept: event accepted if filterPassed is true
- Required: event accepted if and only if ALL required algs have filterPassed set to true
- Veto: event rejected if filterPassed is true
.
If there are no filter algorithms, then all events are
accepted. Otherwise an event is accepted if it has at least one Accept
alg AND ALL Required algs AND NO Veto algs with filterPassed set to
true.

@subsection Tools Using the Tools

The RegistrationStream communicates with its Tool using the IRegistrationStreamTool interface 
defined in AthenaKernel. The Tools used by the RegistrationStream can look for different classes 
in StoreGate as Input. The Tool makes this available to the RegistrationStream 
through its listID() method. The Tool also contains the code that is "persistency" 
specific. The current Tools are listed below. 
<ul>
  <li>RegistrationStreamLCGTool (Default)</li>
    <ul>
      <li>Input: AthenaAttributeList</li>
      <li>Output: POOL Collection</li>
      <li>Properties:</li>
      <ul>
        <li>CollectionType (DEFAULT: 'ExplicitROOT')</li>
        <li>Connection (DEFAULT: '')</li>
        <li>OutputCollection (DEFAULT: '')</li>
        <li>CollectionOpenMode (DEFAULT: CREATE_AND_OVERWRITE)</li>
        <li>Slave (DEFAULT: true) If true, then take property settings from RegistrationStream. 
        <li>PersistencySvc (DEFAULT: 'EventPersistencySvc')</li>
        <li>StoreName (DEFAULT: 'StoreGateSvc')</li>
      </ul>
    </ul>
  <li>RegistrationStreamTagTool</li>
    <ul>
      <li>Input: TagAthenaAttributeList</li>
      <li>Output: POOL Collection</li>
      <li>Properties:</li>
      <ul>
        <li>FragmentByGroup (DEFAULT: false) If true, then form collection fragments based on GRP settings in AttributeType.</li>
      </ul>
    </ul>
  <li>RegistrationStreamHDFTool</li>
    <ul>
      <li>Input: AthenaAttributeList</li>
      <li>Output: HDF5</li>
      <li>Properties:</li>
      <ul>
        <li>OutputCollection (DEFAULT: '')</li>
        <li>CollectionOpenMode (DEFAULT: CREATE_AND_OVERWRITE)</li>
        <li>AttributeSizeOverride (DEFAULT: <empty list>)</li>
      </ul>
    </ul>
</ul>

@subsection Items Items

The ItemList is used for three different, but related purposes.

<ul>
  <li>Reference designation</li>
  <ul><li>It should contain one element which is DataHeader\#Key. 
Because there may be 
multiple streams, there may be multiple DataHeaders in StoreGate. In fact, most of the 
time there will be at least two because there is normally an input and output DataHeader. 
The input DataHeader is chose using the Property WriteInputDataHeader, and not by the Key 
in the ItemList. Only a single DataHeader should be specified. If multiple are present, the 
first one will be chosen. Key="*" should not be used.</li></ul>
  <li>Event Metadata</li>
  <ul><li>It should also contain a <class>\#Key for the event metadata object to find in StoreGate. 
The class must match the class requested by the Tool in use by the RegistrationStream. Only a single 
object should be specified. Key="*" should not be used.</li></ul>
  <li>Collection Metadata</li>
  <ul><li>It may contain one or more CollectionMetadataContainer\#Key combinations. If the 
desired containers are available in the MetaDataStore, then they will be copied into the 
output. Key="*" will write all CollectionMetadataContainers in the MetaDataStore</li></ul>
</ul>

@section RegistrationServices_IOVRegistrationSvc IOVRegistrationSvc

The IOVRegistrationSvc is used  used to register conditions objects in
the Interval of Validity (IOV) database. We consider two steps for the
storing of conditions data. The first step writes out the conditions
data itself, and the second step registers the data with an IOV in the
IOV DB. This service currently provides only for the second step,
although it is envisaged to allow one to optionally integrate the
first step so that the service can provide a single step operation
when desired.

The service is accessed via its interface, IIOVRegistrationSvc, which
provides a number of registerIOV methods. To register an object one
must provide the following information, where options are indicated:

- type name: the class type (required)
- key: the StoreGate key (only needed if more than one key is being used)
- tag: the folder tag. (optional) 
- IOV: the interval of validity (required). This must be give in terms of run/luminosity blocks or time. See discussion below on intervals. It may be given via call arguments or job options.

The IOVRegistrationSvc service has the following properties:

- BeginRun:        Begin run number   (default: IOVTime::MINRUN)
- EndRun:          End run number     (default: IOVTime::MAXRUN)
- BeginLB:         Begin LB number    (default: IOVTime::MINEVENT)
- EndLB:           End LB number      (default: IOVTime::MAXEVENT)
- BeginTime:       Begin time         (default: IOVTime::MINTIMESTAMP)
- EndTime:         End time           (default: IOVTime::MAXEVENT)
- TagDescription:  Description of tag stored into COOL (default: Athena IOVRegistrationSvc)
- userTags:        User COOL user tags (True) or HEAD-style tags (False) (default: True)
- userTagsUpdateHead : Insertion of user tags also updates head if True (default False)
- SVFolder:        Create single-version instead of multiversion folders (default: False)
- PayloadTable:    Create folders with separate payload tables (default: False)
- OverrideNames    List of attribute names whose default COOL types should be overridden
- OverrideTypes    Corresponding list of COOL types for these names

Note that the BeginTime and EndTime are specified in seconds since 00:00 GMT
on 1st January 1970, and not in nanoseconds as used internally by COOL.

There are a number of registerIOV methods of the IOVRegistrationSvc
service which allow different combination of optional arguments, and
as well to use the IOV as specified by the job options. Note that the ones 
specifying timestamps use full 64 bit timestamps (in nanoseconds since 
1/1/1970, and not seconds as in the job options).

@subsection IOVNOTE Note on IOVs

Normally when one registers a new IOV, the minimum corresponds to the
beginning of the new interval of interest one leaves the maximum at
infinity. Each insertion will thus redefine the interval maximum of
the previous IOV whose max was at infinity. For example, one might
add intervals as (run,LB):

- [ (2590, 0) , (IOVTime::MAXRUN, IOVTime::MAXEVENT) ]  : run 2590, LB 0 to infinity
- [ (2591, 0) , (IOVTime::MAXRUN, IOVTime::MAXEVENT) ]  : run 2591, LB 0 to infinity
- [ (2593, 0) , (IOVTime::MAXRUN, IOVTime::MAXEVENT) ]  : run 2593, LB 0 to infinity
- etc. 

This will register IOVs for run 2590, 2591-2, and 2593+. 

In special cases, there IS a GOTCHA that one must take into
consideration. This is the case where one wants to insert an exact
interval with the max NOT at infinity. In this case the max MUST be +1
larger than the desired interval. For example, if one wants to add in
a special interval for run 2592 LBs 7 to 15 inclusively, then one
would add the IOV:

- [ (2592, 7) , (2592, 16) ]  : run 2592, LB 7 to 15 i.e. at LB 16 the next IOV will be fetched

An implication of this logic is that as one keeps registering new IOVs
using max at infinity, the IOVDb actually sets the max of the previous
IOV to be equal to the min of the new IOV. That is IOV maximum values
are considered to NOT be included in the interval of validity,
sometimes referred as [min1, max1), [min2, max2), where '[' ==
"inclusive limit" and ')' == "exclusive limit" and thus max1 == min2.

@subsection TYPENOTE COOL type mapping

Athena users define inline COOL data objects as AthenaAttributeLists and
CondAttrListCollections, which use coral::AttributeListSpecification to define
the data types. In COOL 2.0, the mapping between these transient data types and
the persistent COOL database types is not unique (for example transient 
std::strings can map to COOL String<x> types of different length: 255, 4k,
64k and 16M CLOB strings. Two job option parameters OverrideNames and 
OverrideTypes can be used to customise this mapping. For example, consider a
transient coral::AttributeList containing two strings named SmallString and
BigString, and defined on the transient side as:
<pre>
    coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification();
    attrSpec->extend("SmallString", "string");
    attrSpec->extend("LargeString", "string");
    AthenaAttributeList* attrList = new AthenaAttributeList(*attrSpec);
</pre>
The first time this AthenaAttributeList gets stored into COOL using the 
IOVRegistrationSvc, the desired COOL types can be specified with the job
options:
<pre>
IOVRegistrationSvc.OverrideNames+=['SmallString','LargeString']
IOVRegistrationSvc.OverrideTypes+=['String255','String64k']
</pre>
which will result in the COOL folder being created with the columns having 
maximum size of 255 characters and 64k characters. The default mapping
for strings is set to 4k, for compatibility with COOL1.3.

@section RegistrationServices_OutputConditionsAlg OutputConditionsAlg

Storing and registering conditions objects from Athena is normally done in two
steps - first writing the objects out to a POOL/ROOT file using the 
AthenaOutputStreamTool, then writing the corresponding POOL object references
to the IOV database using IOVRegistrationSvc. This can be done from user
algorithms during execute or finalise. For doing 'trial' runs, typically
only the objects are written out (allowing them  to be read back in and 
verified using CondProxyProvider), and the registration step is skipped until
the user knows the objects are correct. 

The AthenaOutputStreamTool and IOVRegistrationSvc provide a lot of flexibility,
but for simple situations (writing one or a few conditions data objects
on finalize), the OutputConditionsAlg can be used, without the need to write 
any extra user code. This algorithm does nothing on execute, and on finalize
first calls AthenaOutputStreamTool to write the conditions objects to a POOL
file, then optinally registers them in the IOV database. 
The algorithm can be configured entirely from joboptions,
typically as the last algorithm in the theApp.topAlg sequence (to ensure
any user calibration algorithms producing conditions data objects
have already finalized). A typical jobOption sequence would be:
<pre>
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("BeamPosWriter",outputFile="mycondobjects.root",
                          ObjectList=[ "AthenaAttributeList#/Indet/Beampos" ],
                          WriteIOV=True,IOVTagList=["nominal"])
# further parameters can be set using the passed back algorithm instance
myOCA.Run1=3
myOCA.LB1=3
myOCA.Run2=9999
myOCA.LB2=999999

# specify POOL converter package containing the converters for MyCondClass
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
</pre>
The COOL database output is written to the default database connection,
which is normally in a local SQLite file mycool.db. This can be changed
using the IOVDbSvc.dbConnecction property if needed.

The OutputConditionsAlg has the following properties:

- StreamName (default: "ConditionsAlgStream")
- ObjectList (default: "")
- WriteIOV (default: False)
- Run1 (default IOVTime::MINRUN)
- LB1 (default: IOVTime::MINEVENT)
- Run2 (default: IOVTime::MAXRUN)
- LB2 (default: IOVTime::MAXEVENT)
- Time1 (default IOVTime::MINTIMESTAMP) (in seconds)
- Time2 (default IOVTime::MAXEVENT) (in seconds)
- UseTime (default False - use specifications based on run/event)
- IOVTagList (default: "")

If WriteIOV is not set, only the output stream is written, without registration
in the IOV database.
The ObjectList is used to identify the conditions data objects to be written
from the Transient Detector Store (TDS).
Each entry in the list is either a classname, in which case the default
instance of the class in Storegate TDS will be written, using the corresponding
Storegate key as conditions database folder name; or an entry of the
form 'classname#key' (e.g. AlignableTransform#/Indet/Align/SCT). This latter
form is appropriate when several instances of the same object exist, and
need to be written out to several folders. For example, the following joboption
tells the tool
to write out two MyCondObjects with seperate keys /MyCond/Obj1 and 
/MyCond/Obj2 to folders /MyCond/Obj1 and /MyCond/Obj2.
<pre>
myOCA.ObjectList=[ "MyCondObject#/MyCond/Obj1", "MyCondObject#/MyCond/Obj2" ]
</pre>
A third form 'classname#key#folder' can be used to override the default 
where the conditions DB folder is set to the same name as the Storegate key;
for example the specification 'MyCondObject#/MyCond/Obj1#/MyCond/AlternateFolder' writes the object with key /MyCond/Obj1 to folder /MyCond/AlternateFolder.
The AthenaOutputStream-like syntax "MyCondObject#*" to write ALL instances 
of MyCondObject does NOT work with this algorithm.

The IOVTagList should contain one tag for each entry in the ObjectList. If 
no (or an insufficient number) of tags are specified, the corresponding
objects are not tagged (i.e. just visible in the HEAD of the conditions
data.)

Note that CondMultiChanCollections (container objects with several references
to different POOL objects, mapped to COOL folders with several channels) 
can also be written using this tool, by giving the container object name as the
classname.

@section RegistrationServices_RegistrationStreamJobOptions RegistrationStream JobOptions

@include RegistrationStream_jobOptions.py

@section RegistrationServices_IOVRegistrationSvcJobOptions IOVRegistrationSvc JobOptions

@include IOVRegistrationSvc_jobOptions.py

@section RegistrationServices_OutputConditionsAlgJobOptions OutputConditionsAlg JobOptions

@include OutputConditionsAlg_jobOptions.py






*/
