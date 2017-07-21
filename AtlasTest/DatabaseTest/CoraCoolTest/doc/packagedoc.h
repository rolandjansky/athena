/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CoraCoolTest_page CoraCoolTest package

@section CoraCoolTest_introductionCoraCoolTest Introduction

The CoraCoolTest package provides various examples and test codes for the 
CoraCool database API (found in the Database/CoraCool package) that provides
an interface for storing conditions DB data in COOL but with the payload in 
a separate table managed directly by CORAL. At present, this package only 
includes the CoraCoolExample program.

@section CoraCoolTest_CoraCoolExample 

The CoraCoolExample class provides a standalone executable command 
CoraCoolExample.exe, which exercises the storing and retrieving of data through the CoraCool API. This functions both as a simple test of the API,
and as user example of how to use it. The full syntax of the command is:
<pre>
CoraCoolExample.exe <dbConnection> <options> <folder> <nfolder> <nobj> <nchan> <niov> <iov0> <iovinc>
</pre>
The database connection string specifies the COOL database connection in the standard way (e.g. "oracle://devdb10;schema=ATLAS_COOLTEST;dbname=TESTCORA;user=ATLAS_COOLTEST;password=XYZ", or "sqlite://X;schema=myfile.db;dbname=TESTCORA"). The options string consists of a number of single letter commands:
 - w: write data to the database
 - r: read data back from the database
 - b: include a Blob-type object
 - d: delete and recreate the COOL folder if it exists already
 - a: write additional references to the same data (using referenceObject)
 - c: execute the checkAPI function to exercise more of the CoraCool API (not yet implemented).

The folder option specifies the name of the COOL folder which will be accessed
if nfolder=1, or the base name for multiple folders (denoted _1, _2 etc).
 he CORAL table used for the payload storage is automatically
named as the COOL database instance name followed by the leaf-name of the
COOL folder. The next 3 parameters define the number of payload rows per 
COOL channel, the number of channels, and the number of IOVs to store/read.
The last two (optional) parameters specify the first IOV and the increment 
between IOVs.

For example, the command:
<pre>
CoraCoolExample.exe "sqlite://X;schema=mytest.db;dbname=TESTCORA" wr /CoraCool/Test1 10 5 20
</pre>
will write data to a local SQLite file and then test that it can be read back. The data consists of 10 records (CORAL database rows) for each of 5 COOL channels. 10 IOVs are written, each of length 1, starting at zero 
(these last two are the default options).

The actual structure of the data written/read is defined inside the
constructor of the CoraCoolExample class. It includes all the standard C++
types (signed and unsigned ints and long longs, floats and doubles), plus
optionally a Coral BLOB. Created objects include information on the channel and IOV they were created for, so data read back from the database can be
checked for consistency.





*/
