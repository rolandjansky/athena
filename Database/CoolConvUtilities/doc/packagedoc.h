/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CoolConvUtilities_page CoolConvUtilities package

@section CoolConvUtilities_introductionCoolConvUtilities Introduction

This package provides several standalone utilties which aid the interaction
with the COOL conditions database.
They are each constructed as standalone executable commands
which get put into the offline software install/bin area. The executable 
DCSTxtToCool.exe allows DCS data stored in simple text files (output from
the DCS PVSS internal archive using a utility provided by Jim Cook)
to be input to COOL. The executable LisToCool.exe provides a simple migration
tool for data stored in the Lisbon MySQL conditions database to be put into
COOL - this is intended particularly for use in migrating the 2004 combined
testbeam conditions data to COOL.  AtlCoolCopy.exe is a flexible tool
for copying data between COOL databases and verifying their contents.
AtlCoolConsole.py is an enhanced version of the PyCoolConsole tool that comes
with COOL distribution, and provides an interactive 'file-system-like' shell
for interacting with COOL databases.

All executables require the offline software environment to be set up - 
the easiest way to do this is to do a source setup.sh of the CoolConvUtilities
package, i.e. if you are running in release 12.0.1 (with version 2.0.1 of the
AtlasConditions project), do:
<pre>
source /afs/cern.ch/atlas/software/builds/AtlasConditions/2.0.1/Database/CoolConvUtilities/cmt/setup.sh
</pre>
If you are in another full offline software environment (e.g. RecExCommon),
the environment should be setup for the utilities to work without problem.

@section CoolConvUtilities_DCSTxtToCool 

The DCSTxtToCool command is used as follows:
<pre>
DCSTxtToCool,exe <coolDBconnection> <configfile> <datafile> [<hours_offset>]
</pre>
The COOL database connection is specified in the usual COOL-native format,
e.g.
<pre>
oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_INDET;user=ATLAS_COOL_INDET;dbname=COMPROD;password=xxx
</pre>
for a connection to the Oracle server ATLAS_COOLPROD using schema/user ATLAS_COOL_INDET and database name COMPROD, or:
<pre>
sqlite://X;schema=myfile2.db;dbname=MYTEST
</pre>
for a connection to a COOL database named MYTEST in local SQLite file 
myfile2.db. This SQlite form is particularly useful for testing a database 
import to a local file, before importing the data to the Oracle production
database. When using SQLite, it may be useful to set the environment variable
POOL_OUTMSG_LEVEL=E to avoid excessive debug printout from the SQLiteAccess 
module.

The configuration file defines a mapping from DCS datapoint names to COOL
folders and channel numbers, and consists of a series of lines, each
containing a DCS datapoint name, a COOL folder, column name and COOL channel 
number. For example:
<pre>
SCT_Envr_22:ELMB/Envr_CANbus/Envr_ELMB_1/AI/ELMB_Channel_0.value /SCT/DCS/TEST1 data1 0
SCT_Envr_22:ELMB/Envr_CANbus/Envr_ELMB_1/AI/ELMB_Channel_1.value /SCT/DCS/TEST1 data1 1
SCT_Envr_23:ELMB/Envr_CANbus/Envr_ELMB_1/AI/ELMB_Channel_2.value /SCT/DCS/TEST1 data1 2
SCT_Envr_22:ELMB/Envr_CANbus/Envr_ELMB_1/AR/ELMB_Channel_0.value /SCT/DCS/TEST2 mydata 0
</pre>
This file maps several SCT datapoints to different COOL channels (0,1,2) in
the folder /SCT/DCS/TEST1, and another datapoint to channel 0 in the folder
/SCT/DCS/TEST2. The data point columns (assumed to be simple floats) are 
named data1 and mydata, respectively.

The data file contains the actual data to be imported, for example:
<pre>
SCT_Envr_22:ELMB/Envr_CANbus/Envr_ELMB_1/AI/ELMB_Channel_0.value
2006.01.09 09:53:32.120       -92.263568045107
2006.01.09 09:54:11.290       -92.044749165233
2006.01.09 09:54:21.306       -92.263568045107
2006.01.09 09:55:01.398       -92.044749165233
2006.01.09 09:55:41.413       -92.263568045107
2006.01.09 09:56:31.490       -92.044749165233

SCT_Envr_22:ELMB/Envr_CANbus/Envr_ELMB_1/AI/ELMB_Channel_1.value
2006.01.09 09:53:32.120       -92.263568045107
2006.01.09 09:54:11.290       -92.044749165233
2006.01.09 09:54:21.306       -92.263568045107
2006.01.09 09:55:01.398       -92.044749165233
2006.01.09 09:55:41.413       -92.263568045107
2006.01.09 09:56:31.490       -92.044749165233
</pre>
This files are produced from the PVSS local archive using a tool developed
by Jim Cook, and consist of a datapoint name, followed by several lines
giving date, time and datapoint value. The datapoint time is specified in 
UTC (universal time or GMT) and NOT local time. If the file actually gives
local times (1 or 2 hours in advance of UTC in winter/summer), the
hours_offset argument can be used to specify a value (in hours) to be 
added/subtracted to the timestamps before input to COOL. For example,
an offset of -1 will correct local Genvea winter time to UTC. COOL assumes
the timestamps are stored in UTC.

The utility creates COOL (single-version) folders as needed, but does not 
overwrite existing data if the folder already exists. It can be used to add 
some subsequent DCS data to a folder already containing earlier DCS data, 
providing all the new data has later timestamps than the old data. This
is in accordance with the runs for updating COOL single-version folders.

@section CoolConvUtilities_LisToCool 

The LisToCool command is used as follows:
<pre>
LisToCool.exe <LisbonDBconnection> <lisbon_folder> <coolDBConnection> <cool_folder> <mode> [<time_offset>] [<new_column_names> | <tag_prefix>] [<channel>]
</pre>
The command will copy the data from the specified folder in a Lisbon MySQL
conditions database to the specified folder in a COOL conditions database.
The Lisbon database connection is specified using the Lisbon native syntax,
e.g.: 
<pre>
atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions
</pre>
for a readonly connection to the conditions_ctb_2004 database on atlobk02 
(the combined testbeam database for real data).
The COOL database connection is specified as discussed above for 
DCSTxtToCool. 

The mode parameter can take three values:

- mode=1: This is suitable for copying Lisbon CondDBTable data structures,
as typically used by DCS data stored using the Lisbon PVSS manager application.
In this mode, data is copied to COOL single-version folders (i.e. there is
no version tagging). Appropriate COOL data types are defined for each column
in the Lisbon tables (note the Lisbon long-long type is mapped to COOL long
as COOL does not support the storage of 64 bit integers). Array data is not
properly supported - if arrays are present, only the first member is copied
to COOL.

- mode=11: This also copies CondDBTables, but applies a form of 
'store on change'. All data is copied with open-ended intervals of validity
(in the way DCS data is usually stored). If the next entry has the same
payload as the previous one, it is not stored again. This can be used to
convert e.g. a set of Lisbon data with values stored every 5 minutes 
(and changing infrequently) to a COOL structure with individual IOVs
representing longer periods of stable values.

- mode=2: This is suitable for copying Lisbon Blob with Tag structures,
as typically used to store references to POOL objects in Athena. In this mode,
the data corresponding to each tag in the table is copied, but the HEAD 
tag is not copied (unless there are no other tags in the folder).

The time_offset parameter can be used to apply an offset (in hours) to the
data before copying, to compensate for various timing errors made with
the combined testbeam conditions data.

If the channels parameter is given, the data will be copied to COOL with a 
particular channel number. This is useful for merging a set of Lisbon 
CondDB folders into a single COOL folder, with each Lisbon folder being 
given a separate channel number. If this is done, the column(s) in the Lisbon
table have to be renamed, as they may be different in each folder. This can
be achieved by specifying the COOL column names using the new_column_names
parameter - they are given as a comma separated list of names in quotes.

The tag_prefix parameter (in the same place as the new_column_names parameter 
for CondDBTable folders) is used for Blob with Tag folders to apply a prefix
to the tag names before they are used in COOL. This helps to apply 
the COOL tagging convention where a unique folder-based prefix is applied
to tag names (e.g. MyFolder_MyTag) to ensure they are globally unique. If
no prefix is supplied, the Lisbon tags will be used unchanged.

@section CoolConvUtilities_AtlCoolCopy 

Note that full up-to-date documentation for AtlCoolCopy is maintained at:
http://twiki.cern.ch/twiki/bin/view/Atlas/AtlCoolCopy

The AtlCoolCopy tool is used as follows:
<pre>
AtlCoolCopy.exe <sourceCoolDB> <destinationCoolDB> { <options> }
</pre>
where the source and destination databases are specified using the standard 
COOL syntax as detailed above. With this syntax, the whole COOL folder tree
on the source database is copied (from '/'), providing the (empty) destination
database already exists. Use the '-create' option to force creation of
the destination database if it is not existing. COOL singleversion folders are
copied in their entirity, whilst all defined tags of COOL multiversion folders 
are copied. The HEAD tag of multiversion folders is not copied, unless there 
are no other tags in the folder. Bulk copying operations are used with buffered
transfers, so the command can be used to copy very large COOL databases 
(though this may take many minutes).

The command takes a number of optional parameters, which can be given
multiple times (where appropriate). Parameters are case sensitive.
<pre>
-a, -alliov            : Set IOVs on destination to [ValidityKeyMin, ValidityKeyMax] or to the values specified by -nrls, -nrlu, -nts, -ntu
-al, -appendlocked     : Allow data insert to locked tags, providing IOVs do not overlap with existing ones (via tag explicit unlock/relock)
-alsv, -appendlockedsv : Allow data insert to locked tags, providing IOVs are openended and increasing, like SV-mode folders) (via tag explicit unlock/relock)
-ana, -analyse <delta_t> : Produce a ROOT file (filename given instead of destination DB) with histograms showing analysis of IOV structures
-bs, -buffersize <size>: Set size of bulk-storage output buffer to <size> objects  
-c, -create            : Create the database on the destination if it is not
                         already existing
-ch, channel <channel> : Restrict selection to given channel (numeric ID or name)
-ch1, channel1 <channel> : Restrict selection to given channel range between
-ch2, channel2 <channel> : channel1 and channel2 (inclusive)
-chdesc, -channeldesc : Copy channel descriptions as well as names
-cf, -checkfiles       : Check POOL files by opening  as well as references
-cr, -checkrefs        : Check POOL references
-co, -checkoutput <file> : write POOL reference checking info to file
-cti, -copytaginfo     : Copy tag description 
-ctl, -copytaglock     : Copy tag lock status and description 
-d, -debug             : Produce debug output
-ds, -dataset          : When checking POOL references, output a file 
                         register.sh containing DQ2 registration commands for
                         all the required files
-e, -exclude <pattern> : Exclude folders matching the pattern from the copy.
-f, -folder <pattern>  : Include folders matching the pattern in the copy.
-eh, -excludehead      : Exclude HEAD tag when copying multiversion folders
-ih, -includehead      : Include HEAD tag when copying multiversion folders,
                         even if there are explict tags in the folder
-fp, -forcepayload     : Force destination folders to be created with payload table
-fs, -forcesingle       : Force destination folder to be singleversion,
                         even if original is multiversion
                         Only makes sense if copying a single tag
-fm, -forcemulti       : Force destination folder to be multiversion,
                         even if original is singleversion
-frl, -forcerunlumi    : Force destination folder to have metadata indicating
                         indexing by run/lumiblock, even if source is 
			 by timestamp
-ftm, -forcetime       : Force destination folder to have metadata indicating
                         indexing by timestamp, even if source is 
			 by run/lumiblock
    -forcerecreate     : Force destination database to be deleted and recreated
                       from scratch, removing all previous data in that COOL db
-go, -getonline        : Set minimum IOV (-rls) to next run to be started using online run number server
-gt, -gettime          : Extract timestamp interval information for run range
                         given by -rs, -rl or -r (using COOLONL_TDAQ/COMP200 by default)
-h,  -help             : Print help text and exit
-ht, -headtag          : Always copy the HEAD tag of multiversion folders
-is, -ignorespec       : Ignore folder specification mismatches where possible
-lo, -lockedonly       : Only copy locked or partially-locked top-level tags
-nc, -nocopy           : Don't actually copy, just read source DB
-ncr -noclobroot       : Dont' copy CLOB data into ROOT files
-noc,-nocoracool       : Don't copy CoraCool payload tables if present
-nd, -nodata           : Copy only folder structures, not data
-nh, -nohitag          : Do not follow hierarchical tag relations
-mc, -mergecat <catfile>: Specify a POOL file catalogue to be used when making
                          a new dataset
-of, -outfolder <folder> : Rename input folder to <folder> on output. Only 
                           makes sense when coupled with single -folder option
-onr, -onlinerun       : get run number from ATONR rather than ATLR
-ot, -outtag <tag>     : Rename input tags to <outtag> on output. Only makes 
                        sense when coupled with a single -tag option.
-pc, -poolcat <catfile> : Specify a POOL file catalogue to be used in checking
                          POOL references
-pf, -parfile <file>   : Specify additional file containing parameters/options
-pt, -prunetags        : Only copy hierarchical tags referenced by specified toplevel tags
-rdo, -readoracle      : Force reading to go via oracle, using replcia resoultion via dbreplica.config
-sl, -seal <val>       : Set SEAL output level to <val> default 5=ERROR
-t, -tag <tag>         : Copy multiversion data with tag <tag> (default is all tags).
-tl, -taglabel <info>  : Set tag description to be copied into destinaton DB
-tr, -truncate         : If copied IOVs lie outside of rls/rlu range, truncate
                         their edges so they lie inside
-v, -verify            : Instead of copying, verify that the specified data
                         which would have been copied is already present
                         on the destination database.
-ro, -root             : Output data to ROOT file instead of COOL database
-zn, -zeronull         : Set NULLs to zero in ROOT ntuple instead of skipping

-rls, -runlumisince <run> <lumiblock> : Set minimum of IOV interval to <run> <lumiblock>
-rlu, -runlumiuntil <run> <lumiblock> : Set maximum of IOV interval to <run> <lumiblock> 
                                     (exclusive, i.e. the maximum run/LB is NOT included in the interval)
-rs, -runsince <run>  : Set minimum of IOV interval to run <run>
-ru, -rununtil <run>  : Set maximum of IOV interval to run <run> (inclusive)
-r, -run <run>         : Copy only run <run> (sets both min and max).
-rf, -runfile <file>        : Read file giving list of runs which can be updated for UPD2 tags
-srls, -skiprunlumisince <run> <lumiblock> : Set minimum of IOV interval used for skipping when copying (IOVs extending outside this interval will be skipped)

-srlu, -skiprunlumiuntil <run> <lumiblock> : Set maximum of IOV interval used for skipping when copying (IOVs extending outside this interval will be skipped)

-tdb, -timedb <conn>   : Set database connection used for -gettime option
                         Default is COOLONL_TDAQ/COMP200
-ts, -timesince <time> : Set minimum of time interval (SECONDS UTC since 1970)
                         or date in form YYYY-MM-DD:hh:mm:ss (UTC time)
-tu, -timeuntil <time> : Set maximum of time interval (SECONDS UTC since 1970)
                         or date in form YYYY-MM-DD:hh:mm:ss (UTC time)
-nrls, -newrunlumisince <run> <lumiblock> : Set minimum of output IOV interval to <run> <lumiblock> when used with -alliov option
-nrlu, -newrunlumiuntil <run> <lumiblock> : Set maximum of output IOV interval to <run> <lumiblock> when used with -alliov option
-nts, -newtimesince <time> : Set minimum of output IOV time interval (SECONDS UTC since 1970) when used with -alliov option
                         or date in form YYYY-MM-DD:hh:mm:ss (UTC time)
-ntu, -newtimeuntil <time> : Set maximum of output IOV time interval (SECONDS UTC since 1970) when used with -alliov option

-us, -updatesource     : Open source DB in UPDATE mode to overcome CORAL bug in opening large SQLite files readonly
</pre>
Note that:
- Several -folder or -exclude can be given in one command, and AtlCoolCopy will
copy all folders below the given node  (e.g. -folder /a/b -folder /a/c copies all folders below /a/b/ and /a/c. If no folders are specified, the root folder
is used (copy everything). Exclusions are either given as '-exclude /a/b'
which excludes /a/b and everything below it, or '-exclude xyz' which excludes
all folders with the string 'xyz' in their name. This is useful for excluding 
folders containing 'DCS' in their path, signifying DCS information.
- Several tags can be given in multiple '-tag' options. Tags are only used in
the folder in which they are defined.
- AtlCoolCopy understands the ATLAS folder meta data convention to distinguish
between IOVs stored as run/event and absolute timestamps, and the appropriate
settings (run/event or timestamp limit) are used in each case. The default
is to copy all IOV ranges. Timestamps are specified in seconds since 
1st January 1970 for convenience, even though nanoseconds since 1970 are
used internally in COOL.
- Folders will be created as needed on the destination database. If they 
are already there, the existing data is NOT removed first (so it can be
used for 'merge' or 'insert' operations, e.g. of a small SQLite update
database into the main Oracle COOL repository). If the database instance
does not exist at all on the destination, it will only be created if the
'-create' option is explicitely given. If the destniation database is Oracle,
and new folders have been created, the ATLAS coolprod_granter script 
may be needed afterwards to make them accessible to the read-only accounts.
- The COOL channels table is also copied - all channels which exist in the
source folder will be checked. Channels which do not exist on the destination
folder will be created, whilst those which do exist will have their names
and descriptions changed if they differ from those in the source folder 
(unless the names or descriptions are null in the source folder).
- The '-verify' option can be used to check (in a second run of the tool)
that all the copied data was successfully transferred. Note that the
verification algorithm is rather 'simple', just checking that the results
of bulk queries on the source and destination database are the same (same
IOVs, channels and payload). If additional data is already present on the
destination database in the checked folders, this may raise an error.
- The '-nodata' option can be used just to copy the folder structure to the
destination database, without copying any data or tags. This can be useful
when transferring some folder structures from a development to a production
database, without also copying the test data.
- If hierarchical tags are present in the source database, every hierarchical
tag which references a leaf tag which actually gets copied will also be copied.
This action is recursive, in that if the folder structure is /A/B/C, with 
hierarchical tag a in folder /A referring to tag b in folder /A/B and tag b
referring to leaf tag c in folder /A/B/C, then if leaf tag c is copied during
the copying process, tags b and a will also be copied. Copying of leaf tag
c will also be triggered if hierarchical tags a or b are specified as '-tag'
options. Both the copying and resolution of hierarchical tags can be disabled
by using the '-nohitag' option.
- The '-checkrefs' option can be used to generate a list of POOL file GUIDs
referenced by the COOL database folders, which can then be checked against
a set of one or more POOL file catalogues (specified with the -poolcat option)
to check all files referenced by the database can be resolved. This can
be done during copying or verification, or using the '-nocopy' information,
in which case only the source database is accessed. If no POOL file catalogues
are attached, then the GUIDs of all referenced files are printed, together
with a count of the number of times objects from each file are used. If one 
or more catalogues are attached, the logical file names of attached
files are printed instead, and a warning is issued for files which cannot
be resolved, together with a list of folders/tags referencing those files.
Catalogues my be specified as plain filenames (assumed to be XML catlogues)
or using the full POOL 'protocol:catalog' syntax. To reduce the amount of 
printout from the PoolFileCatalog library, it may help to set the environment
variable POOL_OUTMSG_LEVEL to E when checking catalogue references. Note that
this option will check both 'standard' POOL references and fileGUID fake POOL
references used by CoolHistSvc.
- The '-listpfn' option will also list the physical filenames of the 
referenced files, including files for which no logical filename is defined.
- The '-dataset' option will work in conjunction with the -checkrefs 
and -poolcat options to produce a file register.sh containing 
dq2 registerFileInDataset commands for all the POOL referenecs included
in the COOL database which was scanned, using the LFNs found from the input
POOL file catalogues specified with -poolcat. This file can then be sourced
in a shell with access to the dq2 command to actually register the new files
to the target dataset. Additionally, the -mergecat option can be used to 
prevent files which are already known from being registered. Files which
appear in the mergecat catalogues will not be added to the list of output files
in the register.sh. This can be used to 'append' new files to the target 
dataset, taking into account those which are already registered.
- The '-forcerecreate' option will use the COOL API to delete the destination
database before recreating it from scratch, removing any previous data which
was stored there. Since this is a dangerous option, there is no short form
for it.
- The -root option will copy data from the source database to a destination
ROOT file, which is specified (simply as e.g. 'myfile.root') in place of the
destination database identifier. Each folder will produce a separate
ROOT TTree, organised in a ROOT directory sub-structure under the 
top directory COOL, corresponding to the names of the corresponding COOL
database folder sets. For timestamp-indexed folders, the ROOT TTree has 
branches 'IOVSince' and 'IOVUntil' which are the raw 63 bit IOV keys;
run-LB folders are stored with branches 'RunSince',
'RunUntil', 'LBSince' and 'LBUntil'. 
The COOL channel number is given in a branch 'ChannelID'. Payload
columns are represented by ROOT branches of the same name, except that
CLOB and BLOB columns are not copied to COOL. For multiversion folders
with more than one tag, all tags specified are copied, and the tag names
are saved in a branch 'TagID'. For some applications, it maks more sense to 
use the AtlCoolCopy -tag option to restrict the selection to a given tag.

\subsection atlcoolcopyexam  Examples

Copy a source Oracle database to new destination SQlite file, excluding DCS
data. Note that the standard CORAL authentication.xml file will be used for
authentication, so the username/password are not given, and the SQLite database
server name is dummy (so X is given).
<pre>AtlCoolCopy.exe "oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_LAR;dbname=OFLP130" \
 "sqlite://X;schema=replica.db;dbname=OFLP130" -create -exclude DCS
</pre>
Verify the above copy succeeded (note the exclusion must still be given, to 
avoid warnings about non-copied DCS data).
<pre>
AtlCoolCopy.exe "oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_LAR;dbname=OFLP130" \
"sqlite://X;schema=replica.db;dbname=OFLP130" -verify -exclude DCS
</pre>
Copy only the folders under '/LAR/Calib', for a particular run:
<pre>
AtlCoolCopy.exe "oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_LAR;dbname=OFLP130" \
"sqlite://X;schema=replica.db;dbname=OFLP130" -create -folder '/LAR/Calib' -run 1234
</pre>
List all the POOL files referenced by a particular database (note in this case
the destination database name is ignored and can be anything, here 'X'):
<pre>
AtlCoolCopy.exe "oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_INDET;dbname=OFLP130" \ 
X  -nocopy -checkrefs
</pre>
Copy some calibration data from a database filled from calibration analysis
of real data (COMP130) to a destination Monte Carlo database (CMCP130), so 
it can be used in Monte Carlo digitisation to simulate realistic noise. The
copy is done to a sqlite file calib.db, which can then be uploaded back to 
Oracle (it could also be done directly Oracle to Oracle, but this way allows
verification first). The folder is indexed by timestamp on the source
database, and the values valid for a particular time (1149776870 seconds 
after 1970) are extracted, and inserted into the Monte Carlo database with
an IOV valid for all run/event numbers, and given a tag 
SCTCalibDataSR1_Noise_run2878. This tag can then be used in Monte Carlo
processing to simulate events with the same noise as seen in real data.
The same technique could be used for source folders indexed by run/event,
omitting the -forcerunlumi and using -r run instead of -ts and -tu.
<pre>
AtlCoolCopy.exe "oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_SCT;dbname=COMP130" \
"sqlite://X;schema=calib.db;dbname=CMCP130"  \
-alliov -forcemulti -forcerunlumi -folder /SCT/CalibDataSR1/ISNoiseOccupancySummaryData \
 -ts 1149776870  -tu 1149776871  -create -outtag SCTCalibDataSR1_Noise_run2878
</pre>
Dump COOL DCS data in the COMP200 database on the logical connection COOL_SCT 
to a root file sctdcs.root, for all folders under /SCT/DCS, and timestamps 
between UTC times of midnight on 1st April 2007 to midnight on 
8th April 2007.
<pre>
AtlCoolCopy.exe "COOL_SCT/COMP200" sctdcs.root -root -folder /SCT/DCS -ts 2007-04-01:00:00:00 -ts 2007-04-08:00:00:00
</pre>


\subsection atlcoolcopyerror Error codes

AtlCoolCopy.exe produces the following error codes:
- 1: Insufficient parameters
- 2: Syntax error in parameters
- 3: Error reading file specified by -parfile option
- 5: Problem extracting times from run range (-gettime option)
- 6: Problem extracting last online run number (-getonlline option)
- 7: Problem reading runlist files (-runfile option)
- 10: Cannot open source or destination database
- 11: lockedonly option and source DB has no locked tags
- 20: Could not open source folder for copy
- 21: Could not open destination folder
- 22: Source and destination payload specifications differ
- 23: Source and destination channel names/descriptions inconsistent
- 30: Destination folder create failed
- 31: Exception raised in copying operation
- 32: Exception raised in tagging
- 33: Exception raised in no-copy read
- 34: Exception raised in hierarchical tag operation
- 35: Attempt to update UPD1 mode folders without -getonline option
- 36: Attempt to update non-UPD3 folder with -appendlockedsv option
- 100: Verification failure for IOVs
- 101: Verification failure for channels
- 102: Verification failure for payload content
- 103: Verficiation failure for number of entries
- 104: Exception raised in verification loop
- 105: Exception raised in verification iterator setup
- 110: Could not setup POOL catalogues for GUID resolution
- 111: Some POOL references were not resolved to files on attached catalogues
- 112: Problems starting POOL for file consistency checks
- 123: Inconsistent ROOT buffers setup (internal error)
- 124: Exception thrown from ROOT copying loop
- 125: Exception raised in ROOT file copying iterator setup
- 134: Exception raised in folder analysis loop
- 135: Exception raised in folder analysis iterator setup



@section CoolConvUtilities_AtlCoolConsole 

AtlCoolConsole is an extension to the python shell which allows simple 
inspection and modifying of data in a COOL database instance, with a 
unix-filesystem-like interface. It is particularly useful for examining 
COOL tags and managing hiearchical tag trees, and can also delete folders 
and foldersets (if connected using an account with the required privileges).
AtlCoolConsole is an ATLAS-enhanced version of PyCoolConsole written by 
Sven Schmidt and part of the COOL 1.3 distribution. 

To use AtlCoolConsole, just do:
<pre>
AtlCoolConsole.py {<connection>} 
</pre>
where connection optionally specifies the conneciton to a COOL database with 
the usual COOL connection string (see below).  For example, to connect to 
the production COOL INDET schema, taking advantage of the authentication.xml 
file which is included with the software release (hence avoiding the need t
o specify a password explicitly), and then see the folders and tags available:
<pre>
AtlCoolConsole.py "oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_INDET;dbname=OFLP130"
>>> ls Indet
  Name              Description     
  /Indet/TrackingGeo                  
  Name              Description       Count     Size        
  /Indet/Align      run-eventAlignableTransformContainer  68        272000      
  /Indet/Beampos    run-eventAthenaAttributeList  1         36          
  Name              Description       Count     Size        
>>> lstags Indet
Listing tags for folderset/Indet
Indet-DEFAULTCOND
Indet-CSC-000-00
Indet-CSC-001-00
</pre>
Type 'help' within AtlCoolConsole to see a list of available commands, 
including open, ls, cd, pwd, rm, less, lstags, usetag, usechan, pws, settag, 
rmtag, tracetags.

Note that it is also possible to connect via a logical database identifier, 
which will use the dblookup.xml and authentication.xml files to choose an
appropriate database replica (e.g. it will use the local SQLite replicas if 
you are working with a kit installation away from CERN). The connection string 
in this case has the form 'COOL_LAR/OFLP130' to e.g. connect to the 
ATLAS_COOL_LAR schema on the OFLP130 database instance:
<pre>
AtlCoolConsole.py COOL_LAR/OFLP130

Connected to 'COOL_LAR/OFLP130'
Welcome to AtlCoolConsole. Type 'help' for instructions.
>>> ls
  Name              Description     
  /LAR                              
  Name              Description       Count     Size        
>>> ls /LAR
  Name              Description     
  /LAR/ElecCalibMC                  
  /LAR/HVBARREL                     
  /LAR/Identifier                   
  Name              Description       Count     Size        
</pre>





*/
