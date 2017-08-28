/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetAlignGenAlgs_page The InDetAligGenAlgs package

@section InDetAlignGenAlgs_introduction Introduction

This package provides 

  - the algorithms to produce the 'alignment ntuples' used in the ID alignment
             studies, and associated algorithms to write to the conditions
             database. 

  - the algorithms to produce track collections with added pseudo measurements
             (PM).

Reading of alignment constants is done via the ID detector description packages.

The algorithms provided are:

  - AddTRTMomConstr - takes a collection of extended tracks and produces a 
             collection of Si-only tracks with a PM of the track momentum
             measured by the TRT sub-detector. See https://twiki.cern.ch/twiki/bin/view/Atlas/TRTMomConstraint for further details.

  - InDetAlignNt - silicon hit/residual ntuple production 

  - InDetAlignWrt - create the transient objects and optionally write to the
             conditions database via FillIOVTool (see doc/InDetAlignWrt.txt)

  - InDetAlignTBEx - use the AlignTrks produced by InDetAlignNt to produce
             residual histograms for the Si detectors in the testbeam,
             compute alignment constant corrections, and save them back in the 
             conditions database. This algorithm was provided as an example
             for alignment algorithm builders, but is now obselete and is 
             no longer built as part of the release.

  - InDetAlignARes - independent algorithm which refits tracks to study 
            effect of SCT/pixel module distortions.

  - SiDistWriteAlg - algorithm for writing module distortion parameters to
                     the conditions database.

This document provides details of how the silicon alignment data is stored,
more details on each algorithm itself, followed by discussion of the 
job options available in the example subdirectory.

@section InDetAlignGenAlgs_dbsInDetAlignGenAlgs Alignment Data Storage

The alignment database for the SCT and pixels provides a hierarchy of 
transformation matrices (HepTransform3D) for all the modules in the silicon
detectors (pixel and SCT). The interpretation of the transforms is done
by GeoModel, which defines their precise meaning.
The matrices are stored in AlignableTransform objects (defined in the
DetDescrConditions package in DetectorDescription/DetDescrCond) each of which
contains a list of alignable transforms and associated identifiers.
The same scheme was used for both the DC1/DC2 and testbeam layouts, but some
changes were made at release 10.5.0 on the introduction of COOL for the
data storage (these are noted below)

The heirarchy for the ID has 3 levels as follows:

<pre>
StoreGate key
level 1:
/Indet/Align/ID     - offset of PIX,SCT barrel and each endcap 
level 2:
/Indet/Align/PIX    - offset of each of 3 pixel barrels and 2x3 endcap disks
/Indet/Align/SCT    - offset of each of 4 SCT barrels and 2x9 endcap disks
level 3:
/Indet/Align/PIXB<n>  - (<n>=1-3) offset of modules on pixel barrel layer <n>
/Indet/Align/PIXEA<n> - (<n>=1-3) offset of modules on pixel endcap A disk <n>
/Indet/Align/PIXEC<n> - (<n>=1-3) offset of modules on pixel endcap C disk <n>
/Indet/Align/SCTB<n>  - (<n>=1-4) offset of modules on SCT barrel layer <n>
/Indet/Align/SCTEA<n> - (<n>=1-9) offset of modules on SCT endcap A disk <n>
/Indet/Align/SCTEC<n> - (<n>=1-9) offset of modules on SCT endcap C disk <n>
</pre>
Endcap A is at z>0 and endcap C at z<0.

The numbers of transforms stored in each object (for the DC1 layout) are thus:
<pre>
AlignableTransform::print  tagname:/Indet/Align/PIX vector size 6
AlignableTransform::print  tagname:/Indet/Align/PIXEA1 vector size 96
AlignableTransform::print  tagname:/Indet/Align/PIXEA2 vector size 96
AlignableTransform::print  tagname:/Indet/Align/PIXEA3 vector size 96
AlignableTransform::print  tagname:/Indet/Align/PIXB1 vector size 286
AlignableTransform::print  tagname:/Indet/Align/PIXB2 vector size 494
AlignableTransform::print  tagname:/Indet/Align/PIXB3 vector size 676
AlignableTransform::print  tagname:/Indet/Align/SCT vector size 13
AlignableTransform::print  tagname:/Indet/Align/SCTEA1 vector size 368
AlignableTransform::print  tagname:/Indet/Align/SCTEA2 vector size 528
AlignableTransform::print  tagname:/Indet/Align/SCTEA3 vector size 528
AlignableTransform::print  tagname:/Indet/Align/SCTEA4 vector size 528
AlignableTransform::print  tagname:/Indet/Align/SCTEA5 vector size 528
AlignableTransform::print  tagname:/Indet/Align/SCTEA6 vector size 528
AlignableTransform::print  tagname:/Indet/Align/SCTEA7 vector size 368
AlignableTransform::print  tagname:/Indet/Align/SCTEA8 vector size 368
AlignableTransform::print  tagname:/Indet/Align/SCTEA9 vector size 208
AlignableTransform::print  tagname:/Indet/Align/SCTB1 vector size 768
AlignableTransform::print  tagname:/Indet/Align/SCTB2 vector size 960
AlignableTransform::print  tagname:/Indet/Align/SCTB3 vector size 1152
AlignableTransform::print  tagname:/Indet/Align/SCTB4 vector size 1344
AlignableTransform::print  tagname:/Indet/Align/ID vector size 4
</pre>
Before release 10.5.0, a separate AlignableTransform object (going to 
a separate folder in the conditions database) was used for each of the above
objects. At release 10.5.0, the system was migrated to use the COOL conditions
database rather than the Lisbon MySQL one, and this storage was changed 
to exploit the possiblities of the CondMultiChanCollection class. This means
that all of the individual objects mentioned above became a member of
the collection/container object of type AlignableTransformContainer, which
is registered in Storegate under the key /Indet/Align, and is stored
in the COOL folder /Indet/Align. The individual objects are still separate
members within the  container, and can be accessed by first retrieving the
container pointer and then iterating over the contained AlignableTransform
objects. The code which reads the alignment (in InDetAlignNt and in 
GeoModel) can cope with both schemes (the one in use is automatically detected),and data can still be stored in the old scheme by setting the jobOption
parameter NewDB of the InDetAlignDBTool to False.

Within each level 3 object, the identifier used for each pair of 
identifier,HepTransform3D is that of the corresponding module (wafer_id). In
the old scheme, there are separate ones for the rphi and stereo sides of the 
SCT modules, but at release 10.5.0 this was changed so that only one
transform is stored per SCT module, having the identifier corresponding to
side 0. At the same time, the SCT GeoModel code was changed to use this
side 0 transform for both sides of the module.
Within level 2 objects, the position of the layer or disk is represented
with an identifier whose sector,ring (and side for SCT) are set to zero.
Within level 1 objects, the layer number is additionally set to zero.

For the combined testbeam, there are 3 pixel layers with 2 modules each,
and 4 SCT layers with 4 modules each (counting rphi and stereo as separate
layers). The numbers of transforms stored for the testbeam are:

<pre>
AlignableTransform::print  tagname:/Indet/Align/PIX vector size 3
AlignableTransform::print  tagname:/Indet/Align/PIXB1 vector size 2
AlignableTransform::print  tagname:/Indet/Align/PIXB2 vector size 2
AlignableTransform::print  tagname:/Indet/Align/PIXB3 vector size 2
AlignableTransform::print  tagname:/Indet/Align/SCT vector size 4
AlignableTransform::print  tagname:/Indet/Align/SCTB1 vector size 4
AlignableTransform::print  tagname:/Indet/Align/SCTB2 vector size 4
AlignableTransform::print  tagname:/Indet/Align/SCTB3 vector size 4
AlignableTransform::print  tagname:/Indet/Align/SCTB4 vector size 4
AlignableTransform::print  tagname:/Indet/Align/ID vector size 2
</pre>

Within GeoModel, the level 3 objects are interpretated as transformations
in the local coordinate system, which means x is along the module
measuring direction (i.e. 'phi'), y is along the strips (SCT) or the
'z' measuring direction (pixels), and z is out of the plane of the module.
For SCT modules, the transform x direction is strictly phi for the r-phi 
module sides, but  slightly different from local x for the stereo sides.
The coordinate systems are always right handed. The level 2 and level 1 objects
are interpreted as transformations in the global coordinate system:
 - for the full ATLAS detector, x horizontal, y vertical, z along the beamline
 - for the testbeam, x is along the beamline, y vertical and z towards the
   Saleve.

All transforms are tested and working for the full ATLAS geometry (in release
8.4.0). The level 3 transforms are known to be working for the testbeam,
but the level 2 and level 1 have not been tested.

The alignment database is enabled by including the joboption file ReadDBS.py
(ATLAS geometry) or ReadTBDBS.py (testbeam geometry) . This enables
the IOVDBSvc service (should eventually be enabled elsewhere) and sets the
database name and tag, then adds all the needed folders to those monitored
by IOVDBSvc. It also sets the 'Alignable' flags in the GeoModel SCT and pixel
detector description services, causing them to pick up and monitor the
alignment information.

The production alignment database is stored in COOL on the INDET schema, 
database instance OFLPROD. The ReadDBS.py file shows an example of
accessing this database.

A couple of tools exist to check what is actually in the alignment database.
The algorithm ReadAlignTrans in DetDescrCondExample will list out details
of the transforms (more if dumplen is set to true) - it was used to produce
the detailed listings above. The following joboptions are required:

<pre>
theApp.Dlls += [ "DetDescrCondExample" ]
theApp.TopAlg += [ "ReadAlignTrans" ]
ReadAlignTrans=Service("ReadAlignTrans")
ReadAlignTrans.dumplen=TRUE
</pre>

Grant Gorfine has also provided an algorithm to calculate the difference of
the aligned module positions from the defaults. In the testbeam geometry,
this can be used as follows:

<pre>
theApp.Dlls += ["InDetDetDescrExample"]
theApp.TopAlg += ["TestSiAlignment/TestPixelAlignment"]
theApp.TopAlg += ["TestSiAlignment/TestSCT_Alignment"]
TestPixelAlignment=Service("TestPixelAlignment")
TestPixelAlignment.ManagerName="Pixel"
TestSCT_Alignment=Service("TestSCT_Alignment")
TestSCT_Alignment.ManagerName="SCT"
</pre>

@section InDetAlignGenAlgs_IndetAlignNt The InDetAlignNt algorithm

This algorithmn produces the alignment ntuple containing the SCT/pixel and
optionally TRT information. The ntuple is produced first as a transient
class in the TES (class InDetAlignTrkInfo/AlignTrk) which can be manipulated
by downstream algorithms and also written to an ntuple. The ntuple uses
the standard athena interfaces so can be saved as an hbook or root file.
Documentation on the ntuple contents can be found in the note at:

http://rhawking.home.cern.ch/rhawking/atlas/aligntrk/ntup.ps.gz

or on the ID alignment group web pages. Note that an extra field 'sinlocal'
has been added to the ntuple 100, and fields 'hitcount' and 'dead'
to ntuple 110, but this is not documented in the above note.

The file 
<pre>
   InDetAlignAlgs/InDetAlignNt.py 
</pre>
provides the fragments to turn on just the algorithm in the context
of InDetRecExample. The algorithm can read different types of tracks
from the TDS (ConvertedXKalman or IPAt tracks, tracks from the NewTracking
algorithms, or native iPatRec tracks). The different input sources
are selected using joboptions within this file.

An option to refit tracks using the new refitter tools (either from PRD or 
RIO_OnTrack) has also recently been added. Refitting from RIO_OnTrack
also allows the alignment ntuples to be produced from ESD data (in release
10.0).

A full list of the available joboptions is given below, with default values in 
brackets:
<pre>
InDetAlignNt.PtMin (2000.)  - minimum reconstructed pt for considered tracks
InDetAlignNt.Phi0 (0.)      \ centre and delta phi for considered modules
InDetAlignNt.Delphi (3.3)   / can be used to study a small area of the detector
InDetAlignNt.Etamin (-3.0)  \  min/max eta for considered modules
InDetAlignNt.Etamax (3.0)   /
InDetAlignNt.Zmin (-3000.) \  min/max z of considered modules
InDetAlignNt.Zmax ( 3000.) /
InDetAlignNt.TRT (0)    put TRT info in tracks ntuple (100) if >0
InDetAlignNt.ECAL (0)   save associated ECAL information  (not implemented)
InDetAlignNt.Layout (2) assume DC1 layout (=2) - not used
InDetAlignNt.DMatrix (false) - save raw detector module matrices (do not use)
InDetAlignNt.GlobAng (false) - save global track angles (do not use)
InDetAlignNt.AlignNt (true) - save AlignTrk info in ntuples
InDetAlignNt.Overlap (false) - produce histograms of overlaps
InDetAlignNt.NewTrk (false)  - take input from Trk::Track - does not work yet
InDetAlignNt.NewTrkCol("ConvertediPatTracks") - collection name for Trk::Tracks
InDetAlignNt.ReadAl(false) - read level 3 alignment transforms into ntuple 110
                            useful for checking if expected constants are there
InDetAlignNt.Truth(true)  - read truth info from iPatRec - can be used to
                            disable this if truth is flaky
InDetAlignNt.Refit(false) - use trackrefitting tools to refit specified
                            tracks (name from InDetAlignNt.NewTrk) using
                            a track refit tool before producing alignment 
                            ntuple.
InDetAlignNt.RefitFromRIO(true) - if refitting, refit from RIO (default)
                                  or PRD (false)
InDetAlignNt.RefitTrim(false) - trim outliers from refitted track (option
                                passed to refit method).
InDetAlignNt.RefitterName("") - refit tool to be used (only
                                           TrkKalmanFitter tested so far)
InDetAlignNt.RefitterInst("") - instance of refit tool to be used
InDetAlignNt.ReFitName("AliRefitTracks") - Storegate collection name for
                                           refitted tracks.
InDetAlignNt.NumDeriv(0) - calculate derivatives numerically using 
                               Trk extrapolation tools (makes it a bit slower)
                           Argument is bit pattern for which track parameter
                           derivatives are done numerically (a0=1, z0=2, phi=4,
                           theta=8,q/pt=16; so all=31)
InDetAlignNt.PropagName("Trk::RungeKuttaPropagator") - name of propagator
                            to use for numerical propagation (and 
                            calculation of residuals from converted iPatTracks)
InDetAlignNt.BField(20.83) - assumed B-field at centre of ID for analytic
                            derivative calculation
</pre>

@section InDetAlignGenAlgs_InDetAlignWrt The InDetAlignWrt algorithm

This algorithm provides tools to create the transient objects for ID alignment
in the TDS, to modify them (applying systematic or random misalignments)
and then to write them to the conditions database. The information is created
during the processing of the first event.

<pre>
Joboptions (defaults in brackets)
InDetAlignWrt.Create(true)    create alignment datastructure on first event
InDetAlignWrt.Write(false)    write data strructures to conditions database
InDetAlignWrt.RFile("")       text file for reading \ see below
InDetAlignWrt.WFile("")       text file for writing /
InDetAlignWrt.dispFile("")    text recipe file for displacements - see below
InDetAlignWrt.WriteRun(0)     \ run and event to do the writing
InDetAlignWrt.WriteEvent(0)   /
InDetAlignWrt.ValidRun1(0)    \ first run and event of interval of validity 
InDetAlignWrt.ValidEvent1(0)  /
InDetAlignWrt.ValidRun2(0)    \ last run and event of interval of validity
InDetAlignWrt.ValidEvent2(0)  /
InDetAlignWrt.IOVTag("")      IOV tag string to use
InDetAlignWrt.DBToolInst("InDetAlignDBTool")  - name of InDetAlignDBTool instance to use
InDetAlignWrt.DispMode(0)     apply displacements to database after creation
InDetAlignWrt.DispDet(-1)     detectors to shift (1=pixel,2=SCT,-1=both)
InDetAlignWrt.DispBec(-1)     barrel/endcap to shift (0=barrel, 2=e/c,-1=both)
InDetAlignWrt.DispLayer(-1)   layers to shift (-1 =-all)
InDetAlignWrt.DispRing(-1)   rings to shift (-1 =-all)
InDetAlignWrt.DispSector(-1)   sectors to shift (-1 =-all)
InDetAlignWrt.DispRphi(0.1)   rphi displacement to apply
InDetAlignWrt.DispR(0.2)      r displacement to apply
InDetAlignWrt.DispZ(0.3)      z displacement to apply
InDetAlignWrt.Dispsyst(1)     shift randomly (1) or systematic (2)
                              if 3/4 interpret (Rphi,R,Z) as (x,y,z)
                              if (5) randomise the systematic shift (x,y)
</pre>

Two joboption fragments are provided to use this algorithm:
<pre>
  WriteDBS.py - for writing the alignment database using full ATLAS geometry
  WriteTBDBS.py - for writing the alignment database for the combined testbeam
</pre>
The WriteDBS.py file can be used to write data for the full ATLAS geometry, 
making use of the new AthenaOutputStreamTool and IOVRegistrationSvc tools.
Writing the alignment conditions data is done in two steps - first the
data objects must be written to an output POOL file, and then references
to the objects must be written in the IOV database itself. The WriteDBS.py
can do one or both steps, with either the COOL or Lisbon MySQL IOV databases.
If only the POOL file is written, this can be read back using the ReadPool.py
joboptions without going via the IOV database, allowing alignment constants
sets to be debugged and verified before registration in the IOV database.

The WriteTBDBs.txt can be used to write data for the combined testbeam
(this is possibly obselete).
The user must set the database name and tag (ConditionsDB_rh82 and xxxx_dc2
or xxxx_ctb2 are given as examples), and the database will be created/updated
on the atlasdev1 server - this has not yet been updated to use the production
servers.
At the end of the job, a write file catalogue mywrite.xml and an associated
POOL file SimplePoolFile.root will be produced - these contain the actual
conditions database objects, now pointed to by updated entries in the 
IOV database, and they must be preserved, e.g. in the testbeam using the
POOL file management tools disucssed in:

http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/project/calib/testbeam/poolcond.html

The property DBToolInst sets the name of the instance of InDetAlignDBTool which
will be used by InDetAlignWrt to manipulate the conditions database. By setting
this to something other than the default (instance name = tool name, i.e. 
InDetAlignDBTool), and setting the DBRoot parameter of the tool instance,
it is possible to use InDetAlignWrt to manipulate alternative sets of
alignment constants. For example, the following joboptions sets up
InDetAlignWrt to manipulate a set of alignment constants stored in 
the conditions database and TDS as /Indet/SiSurvey :
<pre>
InDetAlignWrt.DBToolInst="SiSurveyDBTool"
ToolSvc.SiSurveyDBTool.DBRoot="/Indet/SiSurvey"
</pre>
This allows SiSurvey constants to be manipulated, loaded and saved, without 
disturbing the real alignment constants stored at /Indet/Align (which may
be independently manipulated using another instance of the tool).


\subsection txtfiles Reading/writing using text files


InDetAlignWrt now provides options to translate between POOL conditions data
and simple text files. Setting 
<pre>
    InDetAlignWrt.wfile="mydb.txt"
</pre>
in WriteTBDBS.py (with InDetAlignWrt.write=TRUE) will write the alignment 
constants to a simple textfile (mydb.txt) instead of to the IOVDB and POOL
files. This can be used in two ways, either with InDetAlignWrt.create=TRUE
to create a new set of alignment constants from scratch (possibly using
InDetAlignWrt.dispmode to introduce some misalignments before saving them)
or with InDetAlignWrt.create=FALSE and a set of alignment constants read
in from the POOL file - this allows an existing POOL alignment database
to be dumped to a text file (this only works for combined testbeam, not 
for DC2). The writing still happens on the run/event
specified by InDetAlignWrt.WriteRun and InDetAlignWrt.WriteEvent as
for writing via POOL.

Setting :
<pre>
     InDetAlignWrt.rfile="mydb.txt"
</pre>
will make InDetAlignWrt read in a set of alignment constants from the file 
mydb.txt and use them to modify the AlignableTransforms created by having 
InDetAlignWrt.create=TRUE. InDetAlignWrt.write can then also be set
to write these constants to a POOL file - allowing alignment constants
to be imported from a text file into the POOL conditions database.

This tool has been set up primarily for the combined testbeam. Due to some
problems with the identifiers, the 'SCT' and 'PIX' AlignableTransforms
(which are intended for global shifts of complete layers, and are in any
case not used by the testbeam GeoModel) cannot be read and written properly,
and some warning messages will be printed ('Cannot find existing transform
for ...' and 'Ident for unknown detector type in ...') These can safely
be ignored.

An example textfile database representation for the combined testbeam
is given below, with O(mm) displacements for all the individual SCT and pixel
modules:
<pre>
/Indet/Align/PIX
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/PIXB1
1 0 0 5 0 0 -0.128733 -1.56757 -0.495015 0 0 0
1 0 0 5 1 0 0.401012 0.402224 1.94232 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/PIXB2
1 0 1 5 0 0 -0.610921 0.0917879 0.0772904 0 0 0
1 0 1 5 1 0 -0.0247045 -0.274953 0.949225 0.1 0.2 0.3
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/PIXB3
1 0 2 5 0 0 0.204861 1.43386 0.284171 0 0 0
1 0 2 5 1 0 -0.69091 2.58871 0.122096 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/SCT
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/SCTB1
2 0 0 1 0 0 -0.0687601 -2.18524 -1.93866 0 0 0
2 0 0 1 0 1 0.0318488 -0.872103 1.03905 0 0 0
2 0 0 1 1 0 0.798971 -0.545313 0.503613 0 0 0
2 0 0 1 1 1 -1.10118 0.0970995 -1.46327 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/SCTB2
2 0 1 1 0 0 0.635859 1.46525 0.540431 0 0 0
2 0 1 1 0 1 -0.206773 -1.6062 0.710701 0 0 0
2 0 1 1 1 0 -0.837394 1.35655 2.08447 0 0 0
2 0 1 1 1 1 -0.35404 0.398366 0.105719 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/SCTB3
2 0 2 1 0 0 -0.150866 1.41258 1.79273 0 0 0
2 0 2 1 0 1 0.388456 2.74423 1.06971 0 0 0
2 0 2 1 1 0 -0.501279 0.841515 0.957964 0 0 0
2 0 2 1 1 1 0.156619 -1.51097 0.540872 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/SCTB4
2 0 3 1 0 0 -0.141011 -1.69742 -0.0924625 0 0 0
2 0 3 1 0 1 -0.182414 -1.29188 0.767111 0 0 0
2 0 3 1 1 0 -0.157702 -1.70766 -1.48131 0 0 0
2 0 3 1 1 1 -0.114899 -2.48781 -1.58852 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
/Indet/Align/ID
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0. 0. 0. 0. 0. 0.
</pre>

\subsection recipetxt Reading from text recipe files

The string property InDetAlignWrt.dispFile can be used to specify a text
file containing a list of recipes to be applied to generate misalignments.
The format of the file is one or more lines of:
<pre>
<level> <syst> <det> <bec> <lay> <ring> <sector> <rphi> <R> <z>
</pre>
where 
 - <level> is 1-3 for level 1 - 3 transforms
 - <syst> 1=systematic rphi, 2=rndm rphi, 3=systematic xyz 4=rndm xyz,
             5= randomised systematic
             for xyz, <rphi> <r> <z> are interpreted as x y z             
 - <det> = -1 for both, 1 for pixel, 2 for SCT
 - <bec> = -1 for all, 0 for barrel, 2 for endcap (both of them)
 - <lay> = layer number, -1 for all layers numbered 0 to n-1
 - <ring> = ring number, -1 for all rings
 - <sector> = sector number, -1 for all sectors
 - <rphi> <R> and <z> are magnitudes (syst or RMS) in mm.

This can be used to set up a set of random or systematic misalignments
fr the different layers of detector.

@section InDetAlignGenAlgs_InDetAlignARes The InDetAlignARes algorithm

The InDetAlignARes algorithm is an end-user algorithm for studying the effects
of module distortions and other alignment-related problems on tracking 
resolution. It takes a set of tracks produced by a trackfitting package, 
and refits each one using the track refitting tools. It then repeats the fit
a number of times, slighlty moving the position of the hits in their local
coordinate systems to simulate the effect of module distortions. Histograms
are produced showing the RMS change in track parameters as a function of 
momentum, for each set of distortions introduced. The distortions are 
parameterised as one of several types, and random parameter values 
according to a given RMS are chosen for each module.

The algorithm has the following jobOption parameters and default values:
<pre>
InDetAlignARes.a0cut(0.5)    Cut on track parameter a0 for good tracks
InDetAlignARes.z0cut(200.)   Cut on track parameter z0 for good tracks
InDetAlignARes.SigmaCut(8.)  Cut on number of standard deviatons any track
                             parameter can be from truth value in order to
                             use track. If negative, truth checks are skipped
                             and truth data is not accessed.
InDetAlignARes.TracksName("Tracks")
InDetAlignARes.Refitter("TrkKalmanFitter")
# the parameters below are all arrays, and the same number of array entries
# need to be given for all - between them they define the parameter sets
# to be studied
InDetAlignARes.RFtype (array)  Type of distortion (see below)
InDetAlignARes.RFPar1 (array)  RMS value for parameter 1
InDetAlignARes.RFPar2 (array)  RMS value for parameter 2
InDetAlignARes.RFName (array)  Name for the distortion (used in the histograms)
InDetAlignARes.RFDet (array)   Detector to affect (1=pix, 2=SCT, -1=both)
InDetAlignARes.RFBec (array)   Affect barrel (0), endcap (2) or both (-1)
InDetAlignARes.RFLayer (array) Layer to affect (-1=all)
</pre>
The distortion types (parameter RFType) are as follows:

- 1: tilt around module y axis (par1) and x axis (par2), in radians
- 2: bend in middle of moudle around y axis (par 1) and x axis(par 2). Parametergives 1/2 bend angle, in radians.
- 3: bow of module, par 1 gives radius of curvature (mm), par2 gives angle of
bow axis away from module y axis.
- 11: misalignment in x (par1) and y (par2) local coordiantes, in mm
- 12: misaligmment in z (out of local plane), par1 in mm . Par2 not used.

If a parameter is negative, it is only applied to the r-phi side of SCT 
modules. If positive, it is applied independently (with separate random 
numbers) to both sides of the module.

The RMS parameters par1 and par2 are used to choose values randomly for each
module, which are stored and used for all tracks in all events passing through
the module.

@section InDetAlignGenAlgs_SiDistWriteAlg The SiDistWriteAlg algorithm

The SiWriteDistAlg algorithm provides a simple way to store distortion 
parameters (basically an array of floats per module) into the conditions
database. The algorithm reads them from a text file, and puts them into
a DetCondCFloat object stored in the TDS, from where it can be written to the
conditions database using OutputConditionsAlg. The joboption fragment 
SiWriteDist.py configures the algorithm to do both tasks.

Once in the conditions database, the information can be read back using 
a joboption of the form:
<pre>
IOVDbSvc=Service("IOVDbSvc")
IOVDbSvc.Folders+=[CondDBCool.Indet+"/Indet/PixelDist <tag>InDetPixelDist-000-00</tag>"]
</pre>
where InDetPixelDist-000-00 is the conditions DB tag corresponding to the
version of the distortion parameters which are required. The parameters 
for any particular module can then be read in Athena by using the following:
<pre>
  Identifier ident= ... // the value of the Identifier for the module in question
  // retrieve the pointer to the pixel distortions (p_detstore is the SG detector store)
  const DetCondCFloat* pdist=0;
  if (StatusCode::SUCCESS==p_detstore->retrieve(pdist,"/Indet/PixelDist")) {
    float* distpars=pdist->find(ident)
    // distpars is zero if the module is not found
    if (distpars!=0) {
      float distx=distpars[0];
      float disty=distpars[1];
      float z=distpars[2];
    }
  } else {
    log << MSG::ERROR << "Cannot find PixelDisp object" << endmsg;
  }
</pre>
For a concrete example, see the methods CreateModuleNtuple in AlignNtuple.cxx
and getDistPars in AlignCore.cxx (both in the InDetAlignGenAlgs package).

@section InDetAlignGenAlgs_Examples Examples of use

The example directory contains a number of complete Athena joboptions 
to perform various tasks. Note that the alignjobOption.py and 
alignESDjobOption.py files have recently been removed, as their functionality
can be better obtained by including the fragment InDetAlignNt.py in 
standard reconstruction jobs.

 - The IDAlignWrite.py joboption file provides a utility to input ID alignment
constants into the conditions database. An existing POOL file containing
ID alignment objects (written e.g. with the tools in the InDetAlignWrt 
algorithm) is read, a new file of constants output and the constants are
optionally registered in a local SQlite or central Oracle COOL conditions
database. The input file can be in the 'new' format used with COOL, or
the old Lisbon format, in which case a conversion is used. This joboption
does not use the full ID reconstruction framework, but runs a stripped down
Athena job - it therefore has less external dependencies and should be more
robust.

*/
