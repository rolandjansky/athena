/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CoraCool_page CoraCool package

@section CoraCool_introductionCoraCool Introduction

CoraCool is a utility package that builds on top of the LCG COOL software 
to create an interval-of-validity indexed storage for sets of 
objects in payload tables. The idea is to use COOL folders to store references
to one or more rows in a separate payload table, which is managed by CORAL.
This allows a single COOL entry to reference several payload table rows
(implementing arrays), or several COOL entries to reference the same row
(implementing storage where a single object can be valid for many separate
IOVs). The interface to the payload is through vectors of CORAL AttributeLists.

This type of "COOL+CORAL" storage is being considered by several ATLAS
subdetectors for their online database applications. Providing a uniform
interface through CoraCool allows common code to be used in Athena (IOVDbSvc
can read a CoraCool table and present the data in the TDS as a CondAttrListVec
class), and also allows the data to be replicated using a centrally-provided
tool (AtlCoolCopy).

@section CoraCool_schema Data Model

The data model is that of a COOL folder plus an associated database table 
containing the payload, managed by CORAL. The API performs coordinated 
insertion and retrieval of data by making calls both to the COOL API to 
interact with the COOL folder, and direct CORAL calls to insert or query
data in the payload table. The COOL folder description (meta-data) is used
to store the name of the payload table associated to the folder, and the
names of the CORAL folder primary and foriegn key columns.

The following points should be noted:

- The CoraCool package makes use of the COOL data-structures only through the
COOL API - no direct SQL queries are made to the underlying table structures.
This means that read-back (CoraCoolFolder findObject and browseObjects) is 
a two-stage process: first retrieve the foreign keys from COOL, then make
CORAL queries to read the payload data associated with the foreign keys. This
is slower than a direct SQL query joining the COOL and CORAL tables, but
isolated CoraCool from COOL schema changes. If this proves to cause a 
significant performance bottleneck, direct SQL queries could perhaps be 
implemented, at a cost of closer coupling to the COOL schema model.

- The user supplies a name for the payload table, which has the COOL database
instance name prepended (thus following the COOL convention that the first
8 characters of all table names are the COOL database instance name). This
allows multiple COOL database instances (OFLP130, COMP130 etc) to exist
in a single databsae schema, and allows each to have a corresponding CORAL
payload table.

- Information on the name of the payload table, the name of the foreign key
column in the COOL table and in the CORAL table, and the primary key in the
CORAL table, is stored in the COOL folder description string, as metadata
surrounded by the <coracool> XML-like tag. This information is put in 
automatically by the CoraCoolDatabase createFolder method, and is interpreted
by the CoraCoolFolder class as neccessary - it does not need to be
manipulated by clients. If the folder description string supplied to 
CoraCoolDatabase createFolder includes a <coracool> tag, this part is 
stripped out and replaced by the appropriate one according to the
other parameters supplied to the method.

- The payload specification (number and type of columns) is specified by the
user as a COOL RecordSpecification. This class is used instead of
the coral::AttributeListSpecification as the latter cannot be instantiated
freely without a corresponding AttributeList class. The actual data is 
specified in terms of coral Attributes and AttributeLists - this may change
in the future to use the corresponding COOL 2 Record and Field classes.
The type of each column is encoded in a string and stored in the
special table <coolDBintance>_CORACOOLATTR. This ensures that the types
of columns remain the same even if the data is transferred between Oracle,
MySQL, SQLite and Frontier-accessed databases. CoraCool has been tested with
ints, unsigned ints, float, double, string (maximum 4000 characters), and 
coral:Blob data-types. 64 bit ints (long long) can also be used, providing
the special SEAL 64-bit integer types (SLeast and ULeast) are used - see
CoraCoolExample for more details.

- The payload specification must include columns (normally of int type) for
the foreign key linking to COOL, and optionally a separate primary key (this
is needed if arrays are being used, i.e. one COOL entry can reference several
in the payload table). The user is free to use signed or unsigned ints or long
longs for these keys, but the internal implementation is done in terms of
ints, which imposes a maximum on the number of rows which can be stored
in the database (hopefully sufficient!).
Although these columns are present in the payload 
objects, when storing data, new primary keys will be automatically generated
by CoraCool, rather than using any values specified by the user. This ensures
that the key values are unique within the database tables, even if e.g. the
input data is being merged from another CoraCoolDatabase instance. The user
should therefore not rely on the keys taking any particular values. The key 
generation is done by means of an auxillary sequence table called 
<coolDBinstance>_CORACOOLKEYS, which contains one row for each payload table
with the next key sequence value to be used.

- The CoraCoolFolder class includes setupStorageBuffer and flushStorageBuffer
methods for enabling bulk insert operations, in a similar way to COOL. However,
bulk operations are not currently implemented in CoraCool, and these methods
do not actually result in bulk operations being executed. For bulk readback
(using CoraCoolFolder browseObjects), bulk operations are used to 
fetch multiple rows from both COOL and CORAL, with some internal buffering.
However, due to the use of the COOL API, bulk retrieval still involves many
separate SQL queries for the payload data (though these are currently 
combined in groups of 10 to acheieve some speedup).

@section CoraCool_classes Classes

The main classes used in the CoraCool package are discussed below. For 
a detailed documentation of the API, see the class header files.

- the CoraCoolDatabase class provides an interface to open a CoraCool 
database (for reading or writing),
and allows new folders to be created (also creating the corresponding
COOL folder) and existing folders to be accessed or deleted. It also provides
methods to check if a folder exists, and to drop and reconnect to the 
underlying physical database connection. CoraCoolDatabase maintains the 
appropriate COOL database connection internally, so the user should not 
normally have to worry about keeping track of the corresponding COOL 
connection.
CoraCoolDatabase objects can be created directly (given the COOL database 
connection, connection string and SEAL context), but it is easier to use
CoraCoolDatabaseSvc for this purpose.

- the CoraCoolDatabaseSvc class provides a service to open new 
CoraCoolDatabases and create the corresponding objects, given the COOL
connection string and corresponding COOL IDatabase object. This is the
normal way to open CoraCool database connections.

- the CoraCoolDatabaseSvcFactory class provides a singleton to load all
the necessary SEAL, CORAL and COOL libraries and provide access to a
CoraCoolDatabaseSvc and a cool::IDatabaseSvc. It is intended for writing
standalone applications, such as the CoraCoolExample program in 
AtlasTest/DatabaseTest/CoraCoolTest.

- the CoraCoolFolder class provides an interface to interact with a 
CoraCool folder (COOL database folder plus corresponding CORAL payload table),
mirroring the functionality of the COOL IFolder class.
Methods are provided to store and retrieve payload data, together with
retrieving basic information about the payload table and the foreign key 
relations. Bulk storage is implemented by an associated storage buffer 
(use of which can be enabled with the setupStorageBuffer method),
and bulk retrieval proceeds using the browseObjects method which returns
a CoraCoolObjectIter pointer.

- the CoraCoolObject class represents the basic unit of CoraCool datastorage -
an interval of validity and channel number (stored using COOL), together with
a vector of CORAL AttributeLists representing the payload data. CoraCoolObjects
are returned by the CoraCoolFolder find method, and by the CoraCoolObjectIter
next method.

- the CoraCoolObjectIter class is an iterator interface allowing the client to 
iterate through the results of a CoraCoolFolder browseObjects query, using the
hasNext and next methods.

- the CoraCoolSequence class is an internal implementation class supporting
a sequence for the generation of primary keys. It should not be used by
clients.


*/
