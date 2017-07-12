/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page DetDescrCondExample_page The DetDescrCondExample package

@section DetDescrCondExample_introductionDetDescrCondExample Introduction

This package provides a set of examples demonstrating use of various
functionality related to conditions data in Detector Description. The available
example algorithms (so far ReadWriteCoolStrFile, ReadTopTrans, ReadAlignTrans and CoolHistExample) are described in more detail below.

@section DetDescrCondExample_ReadWriteCoolStrFile 

This algorithm demonstrates the functionality of the CoolStrFileSvc service in
the package DetDescrCondTools. It also provides a way to get new files of
detector description conditions data uploaded into the database. The algorithm
provides functionality to write files into the conditions database, and to
read them back as strings or back into files. The documentation here should
be read in conjunction with that for CoolStrFileSvc.

The full list of CoolStrFileSvc joboptions is as follows:

 - Read: read back data from conditions database (default: False)
 - Extract: If read is enabled, also extract data into file (first event only) (default: False)
 - Write: write data into conditions database (default: False)
 - Channel: channel to use (default: 1)
 - Tech: Technology to use (0=COOL string, limited to 4000 chars, 1=COOL CLOB, 
limited to 16M) (default: 0)
 - Folder: Folder in conditions database (default: /TEST/COOLSTR)
 - RFile: File to write data into when reading back (default: "")
 - WFile: File to take data from when writing into conditions database (default: coolstrfile.dat)

The job option fragment 
WriteCoolStrFiles.py can be included in an Athena job to
write file-based data into the conditons database using the CoolStrFileSvc.
Two independent instances of the algorithm are defined, each writing a separate
channel of the same COOL folder. The objects are written into the TDS on the
first event, printed from the TDS on each event, and finally written into
the conditions database using the OutputConditionsAlg at algorithm finalize.
As set up here, a local SQLite COOL database is used to write the data, but it
can also be used to write data into the main Oracle conditions database.

@include WriteCoolStrFiles.py

Note that <b>before</b> running the WriteCoolStrFiles.py example for the first
time on a new condDB folder, the folder must first be created using the
standalone Python tool make_coolstrfile.py (see the wiki page 
<a href=https://uimon.cern.ch/twiki/bin/view/Atlas/CoolUtilities#make_coolstrfile_py_Setup_a_COOL>here</a> for more details). The use of CLOBs to store the
data means that the folder itself cannot be created in Athena. On the other
hand, for folders containing small string payloads (less than 4000 characters)
with CoolStrFileSvc technology 0, this Python tool should NOT be used, and
the folder will be created (automatically) correctly in Athena, the first
time data is saved.

The job option fragment
ReadCoolStrFiles.py demonstrates reading back of the same
conditions data as written by WriteCoolStrFiles.py. The data is read into
the TDS, and accessed on each event using the CoolStrFileSvc getString method.
The data can also be saved (extracted to a file). The same SQlite database
as for the write example is used by default.

@include ReadCoolStrFiles.py

The write example can be used with small modifications to actually upload
data into the conditions database. The read example should be studied to
see how to access the resulting data in your own algorithms.

@section DetDescrCondExample_ReadTopTrans 

This algorithm provides a simple way to read back DetCondKeyTrans objects, and
demonstrates the various forms of callbacks that can be used.

@section DetDescrCondExample_ReadTopTransJobOptions ReadTopTrans jobOptions

@include ReadTopTrans.py

@section DetDescrCondExample_ReadAlignTrans 

This algorithm provides an example of setting a callback which is called 
whenever the inner detector alignment changes. This could be needed for
example by a reconstruction algorithm that needs to cache information about
the geometry, but needs to know when the modules move as a result of 
alignment corrections. The callback is registered on the align method of
IGeoModelSvc, and is called every time the ID alignment changes, AFTER all
the GeoModel code for each subdetector has already been called back to
process the alignment change. Thus reading module position information from
GeoModel in the callback method always gets the up to date information.

In this example algorithm, the positions of all modules are cached in the
callback method. The module positions are also read directly from GeoModel
in the execute method every event, and compared with the cached positions.
Error messages are printed if there are any differences, providing a simple
way to validate the caching process.

@include ReadAlignTrans.py

@section DetDescrCondExample_CoolHistExample 

CoolHistExample is a simple algorithm which demonstrates how to access
ROOT reference histograms through COOL using callbacks. The COOL data is
in an sqlite COOL database share/histdemo.db which should be copied to your run
directory. This database contains a COOL folder /DEMO/TEST1 with a tag 
demo_tag containing an entry for channel 10 pointing to the COOL histogram
file which is part of the conditions dataset 
oflcond.000003.conditions.simul.pool.v0000 . Normally histograms would
go into separate datasets than conditions data POOL files, but this is 
just a demo.

The algorithm contains a callback function which is used to set a pointer to
a reference histogram retrieved from the file. This pointer is used by the
execute method of the algorithm to access data from the ROOT histogram, which
will always be up to date. Note that this will only work from the execute
method - in initialise, the first event has not yet been seen, so the system
will not know which conditions data is appropriate.

For more details see the ATLAS wiki topic CoolHistograms

@include CoolHistExample_jobOptions.py 





*/
