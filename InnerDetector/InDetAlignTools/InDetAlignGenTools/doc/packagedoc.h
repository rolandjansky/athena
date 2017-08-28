/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetAlignGenTools_page The InDetAlignGenTools package

@section InDetAlignGenTools_introductionInDetAlignGenTools Introduction

This package provides a place to put AlgTools related to ID alignment. 
The available AlgTools (so far InDetAlignDBTool and InDetAlignTrackSelTool) 
are discussed below.

@section InDetAlignGenTools_InDetAlignDBTool 

The InDetAlignDBTool AlgTool provides tools (methods) to manipulate the
set of AlignableTransform objects which make up the ID alignment database.
For more details on the object format, see documentation for the InDetAlignAlgs
package. Note that the format changed at release 10.5.0 when COOL was 
introduced and the writing of seperate transforms for each side of SCT 
modules was suppressed.

On initialisation, the InDetAlignDBTool builds a list of AlignableTransform 
names appropriate to the current geometry (e.g. CTB or full ATLAS, with or 
without middle pixel layers). It provides the following methods (see the 
header file for detailed parameter list).

 - createDB: Create a null database in memory, with all modules at default
   positions.

 - dispGroup: displace a group of modules (specified by detector and layer),
   either randomly or systematically.

 - printDB: Print the contents of the database to the screen (transform IDs
   and displacements).

 - writeFile: Write the contents of the database to a text file or ntuple.

 - readTextFile: Read back the database from a text file, and fill the
   corresponding structures in the TDS

 - readNtuple: Read back the database from an ntuple (9002), and fill the 
   corresondping structures in the TDS.

 - dirkey: return the leafname of the AlignableTransform corresponding to
   a particular SCT or pixel identifier (e.g. SCTB3 or PIXEA1).

 - setTrans: set the transformation for a particular module to a given value.

 - tweakTrans: modify (i.e. add to existing transform) the transformation
   for a particular module, allowing incremental changes to the alignment.

 - get Trans: return the value of the transform for a particular module and 
   level.

 - outputObjs: Stream out the alignment database objects from the TDS 
   to a output stream, using the AthenaOutputStreamTool.

 - fillDB: Register the alignment database objects to the conditionsDB, using
   the FillIOVTool provided by the database group. Note that the objects
   must already have been streamed out using outputObjs.

When reading/writing to an ntuple, the ntuple is numbered 9002. The format
is similar to the old ntuple 9000 alignment ntuples, but the transformation
'level' has been added, allowing the global transforms for the whole
detector or detector layers to be written and read back too. Unlike ntuple
9000, the r-phi and stereo sides of SCT modules are not included as separate
modules.

Note that when reading back an alignment database through the tool (e.g. from
a text file or ntuple, the database will be
loaded into memory, but there is currently no method to tell the ID detector
descrption to 'pick up' the new alignment constants (no IOVSvc 
call backs available). This can only be done when the database is stored
as objects in a POOL file. The readback can then be triggered either
by loading the POOL file directly using the CondProxyProviderSvc, or by
loading it via entries in the conditions database. See the joboptions
ReadDBS.py and ReadPool.py in InDetAlignAlgs for examples of how to do this.

The tool has the following properties:

 - NewDB: Flag to use new COOL DB format (default: TRUE)
 - FakeDB: Flag allowing geometry information to be faked if GeoModel 
   information is not available. This allows limited database manipulation to
   be performed in an Athena job without GeoModel (database I/O but not
   creation/modification of databases). FakeDB may be set to 1 (full ATLAS
   geometry with 3 pixel layers) or 2 (combined testbeam geometry).
 - CondStream: Name of conditions output stream on which outputObjs will
   write conditions data (default: "Condstream1")
 - DBRoot: Name of the conditions database directory and Storegate container
   where the alignment data is stored (default: "/Indet/Align").
 - properties related to the DispCSC method (see below).

The algorithm InDetAlignWrt in InDetAlignAlgs provides some examples of how
to use the methods in this AlgTool.

The DBRoot parameter can be changed to allow the tool to manipulate a different
instance of the alignment constants in the TDS and conditions database.
This can be used for example to store the SCT/pixel survey data 
(also consisting of AlignableTransforms for every module) in the conditions
database in the folder /Indet/SiSurvey. For more details of this, see
the documentation of the InDetAlignWrt algorithm in InDetAlignGenAlgs.

@section InDetAlignGenTools_DispCSC 

The DispCSC method inside InDetAlignDBTool allows to generate the misalignments 
of the Pixel and SCT detectors for the CSC exercise. The misalignments are generated at 
three different levels, where, depending on which, the reference frame for the transformation
to be applied may change. 
These levels are quoted below:
- LEVEL 3: module level       (local frame)
- LEVEL 2: layers/disks level (global frame)
- LEVEL 1: subdetector level  (global frame)

For more details you may take a look on the wiki topic SiliconMisaCSC and related links inside.

This DispCSC method is triggered while setting the property:
<pre>
InDetAlignWrt.DispCSC
</pre>
of the InDetAlignWrt algorithm in InDetAlignGenAlgs to the desired level of misalignment. 
A combination of different levels is valid. For example, if we want to generate misalignments
for all silicon modules (level 3) and silicon subdetectors (level 1), without any
misalignment for silicon layers nor disks (level 2), we should set:
<pre>
InDetAlignWrt.DispCSC=31
</pre>
For the CSC sets, misalignments at all different levels need to be generated. Thus, it
remains mandatory to set:
\verbatim
InDetAlignWrt.DispCSC=321
\endverbatim

The properties the user can set are almost identical for all the three levels. 
The misalignment constants can be generated in four different ways:
- from fixed values
- from a flat distribution
- from a gaussian distribution
- from a truncated gaussian distribution

The properties
<pre>
DistTypePixelBaModules
DistTypePixelEcModules
DistTypeSCTBaModules
DistTypeSCTEcModules
DistTypePixelLayers
DistTypeSCTLayers
DistTypePixelDisks
DistTypeSCTDisks
DistTypeID
</pre>
allow to select between the different ways explained above and have an effect for
Pixel barrel modules (DistTypePixelBaModules), Pixel endcap modules (DistTypePixelEcModules), 
SCT barrel modules (DistTypeSCTBaModules), SCT endcap modules (DistTypeSCTEcModules), 
Pixel barrel layers (DistTypePixelLayers), SCT barrel layers (DistTypeSCTLayers), 
Pixel endcap disks (DistTypePixelDisks), SCT endcap disks (DistTypeSCTDisks) and 
ID subdetectors (DistTypeID).

The only allowed values for these properties is an integer number in the range [0;3] 
where each number means:
<pre>
0: fixed numbers
1: flat distribution
2: gaussian distribution
3: truncated gaussian distribution
</pre>

If other value different from the above is set, StatusCode::FAILURE is returned. 

If the numbers are chosen to be generated from a distribution, then additional properties may be set,
as the mean and the sigma of the given distribution. The distribution characteristics can be specified
for all 6 variables considered for the transformations leading up to the desired misalignment: 3 translations and 
3 rotations (defined by three angles: alpha, beta, gamma). 

As an example for the pixel endcap modules, if the following properties are set as:
<pre>
DistTypePixelEcModules(2)

Pix_Mod_Ec_Mean_x = 10
Pix_Mod_Ec_Mean_y = 10
Pix_Mod_Ec_Mean_z = 30
Pix_Mod_Ec_Mean_a = 0
Pix_Mod_Ec_Mean_b = 0
Pix_Mod_Ec_Mean_g = 0

Pix_Mod_Ec_Sigma_x = 0.020
Pix_Mod_Ec_Sigma_y = 0.020
Pix_Mod_Ec_Sigma_z = 0.060
Pix_Mod_Ec_Sigma_a = 0.001
Pix_Mod_Ec_Sigma_b = 0.001
Pix_Mod_Ec_Sigma_g = 0.001
</pre>
the in-plane translations will be generated according to a gaussian distribution centered in 10 microns and 
with sigma 20 microns, the out-of plane translation according to a gaussian centered in 30 microns and 
with sigma 60 microns, and the three rotations according to gaussians centered in zero and with sigma equal
to 1 mrad.

If a truncated gaussian is chose, additional variables may be set:
<pre>
TruncGaussCut_X
TruncGaussCut_Y
TruncGaussCut_Z
TruncGaussCut_A
TruncGaussCut_B
TruncGaussCut_G
</pre>
which specify, for each of the different movements, 
the limit value times the sigma out-of which the generated numbers are rejected. The default values are:
\verbatim
InDetDBTool.TruncGaussCut_X(2.)
InDetDBTool.TruncGaussCut_Y(2.)
InDetDBTool.TruncGaussCut_Z(2.)
InDetDBTool.TruncGaussCut_A(2.)
InDetDBTool.TruncGaussCut_B(2.)
InDetDBTool.TruncGaussCut_G(2.)
\endverbatim
which means that the generated numbers that do not remain in the range [-2*sigma; 2*sigma] are rejected.

For completeness, all available properties including their default values 
for all three levels are shown below. 

All displacements are given in mm and rotations in mrad.

Note the different suffix for level 3 misalignments (lowercase) with respect level 2 and 1 
(uppercase). This is intended for differentiating the local to the global frame.

\subsection subsection1 LEVEL 3
  -# Pixel barrel modules:
  \verbatim
  InDetDBTool.DistTypePixelBaModules(1) - distribution type, Pixel barrel modules (in this case: flat)
  
  InDetDBTool.Pix_Mod_Ba_Mean_x(0) - mean value of distribution for x translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Mean_y(0) - mean value of distribution for y translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Mean_z(0) - mean value of distribution for z translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Mean_a(0) - mean value of distribution for alpha rotation, Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Mean_b(0) - mean value of distribution for beta rotation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Mean_g(0) - mean value of distribution for gamma rotation, Pixel barrel modules
  
  InDetDBTool.Pix_Mod_Ba_Sigma_x(0.030) - sigma value of distribution for x translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Sigma_y(0.030) - sigma value of distribution for y translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Sigma_z(0.050) - sigma value of distribution for z translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Sigma_a(0.001) - sigma value of distribution for alpha rotation, Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Sigma_b(0.001) - sigma value of distribution for beta rotation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Sigma_g(0.001) - sigma value of distribution for gamma rotation, Pixel barrel modules
  
  InDetDBTool.Pix_Mod_Ba_Fix_x(0) - fixed value for x translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Fix_y(0) - fixed value for y translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Fix_z(0) - fixed value for z translation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Fix_a(0) - fixed value for alpha rotation, Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Fix_b(0) - fixed value for beta rotation,  Pixel barrel modules
  InDetDBTool.Pix_Mod_Ba_Fix_g(0) - fixed value for gamma rotation, Pixel barrel modules
  \endverbatim
  -# Pixel endcap modules:
  \verbatim
  InDetDBTool.DistTypePixelEcModules(1) - distribution type, Pixel endcap modules (in this case: flat)
  
  InDetDBTool.Pix_Mod_Ec_Mean_x(0) - mean value of distribution for x translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Mean_y(0) - mean value of distribution for y translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Mean_z(0) - mean value of distribution for z translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Mean_a(0) - mean value of distribution for alpha rotation, Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Mean_b(0) - mean value of distribution for beta rotation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Mean_g(0) - mean value of distribution for gamma rotation, Pixel endcap modules
  
  InDetDBTool.Pix_Mod_Ec_Sigma_x(0.030) - sigma value of distribution for x translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Sigma_y(0.030) - sigma value of distribution for y translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Sigma_z(0.050) - sigma value of distribution for z translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Sigma_a(0.001) - sigma value of distribution for alpha rotation, Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Sigma_b(0.001) - sigma value of distribution for beta rotation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Sigma_g(0.001) - sigma value of distribution for gamma rotation, Pixel endcap modules
  
  InDetDBTool.Pix_Mod_Ec_Fix_x(0) - fixed value for x translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Fix_y(0) - fixed value for y translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Fix_z(0) - fixed value for z translation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Fix_a(0) - fixed value for alpha rotation, Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Fix_b(0) - fixed value for beta rotation,  Pixel endcap modules
  InDetDBTool.Pix_Mod_Ec_Fix_g(0) - fixed value for gamma rotation, Pixel endcap modules
  \endverbatim
  -# SCT barrel modules:
  \verbatim
  InDetDBTool.DistTypeSCTBaModules(1) - distribution type, SCT barrel modules (in this case: flat)
  
  InDetDBTool.SCT_Mod_Ba_Mean_x(0) - mean value of distribution for x translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Mean_y(0) - mean value of distribution for y translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Mean_z(0) - mean value of distribution for z translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Mean_a(0) - mean value of distribution for alpha rotation, SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Mean_b(0) - mean value of distribution for beta rotation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Mean_g(0) - mean value of distribution for gamma rotation, SCT barrel modules
  
  InDetDBTool.SCT_Mod_Ba_Sigma_x(0.150) - sigma value of distribution for x translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Sigma_y(0.150) - sigma value of distribution for y translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Sigma_z(0.150) - sigma value of distribution for z translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Sigma_a(0.001) - sigma value of distribution for alpha rotation, SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Sigma_b(0.001) - sigma value of distribution for beta rotation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Sigma_g(0.001) - sigma value of distribution for gamma rotation, SCT barrel modules
  
  InDetDBTool.SCT_Mod_Ba_Fix_x(0) - fixed value for x translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Fix_y(0) - fixed value for y translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Fix_z(0) - fixed value for z translation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Fix_a(0) - fixed value for alpha rotation, SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Fix_b(0) - fixed value for beta rotation,  SCT barrel modules
  InDetDBTool.SCT_Mod_Ba_Fix_g(0) - fixed value for gamma rotation, SCT barrel modules
  \endverbatim
  -# SCT endcap modules:
  \verbatim
  InDetDBTool.DistTypeSCTEcModules(1) - distribution type, SCT endcap modules (in this case: flat)
  
  InDetDBTool.SCT_Mod_Ec_Mean_x(0) - mean value of distribution for x translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Mean_y(0) - mean value of distribution for y translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Mean_z(0) - mean value of distribution for z translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Mean_a(0) - mean value of distribution for alpha rotation, SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Mean_b(0) - mean value of distribution for beta rotation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Mean_g(0) - mean value of distribution for gamma rotation, SCT endcap modules
  
  InDetDBTool.SCT_Mod_Ec_Sigma_x(0.100) - sigma value of distribution for x translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Sigma_y(0.100) - sigma value of distribution for y translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Sigma_z(0.150) - sigma value of distribution for z translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Sigma_a(0.001) - sigma value of distribution for alpha rotation, SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Sigma_b(0.001) - sigma value of distribution for beta rotation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Sigma_g(0.001) - sigma value of distribution for gamma rotation, SCT endcap modules
  
  InDetDBTool.SCT_Mod_Ec_Fix_x(0) - fixed value for x translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Fix_y(0) - fixed value for y translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Fix_z(0) - fixed value for z translation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Fix_a(0) - fixed value for alpha rotation, SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Fix_b(0) - fixed value for beta rotation,  SCT endcap modules
  InDetDBTool.SCT_Mod_Ec_Fix_g(0) - fixed value for gamma rotation, SCT endcap modules
  \endverbatim

\subsection subsection2 LEVEL 2
  -# Pixel Barrel layers
  \verbatim
  InDetDBTool.DistTypePixelLayers(0) - distribution type, Pixel barrel layers (in this case: fixed values)

  InDetDBTool.Pix_Layers_Mean_X(0) - mean value of distribution for X translation,   Pixel barrel layers
  InDetDBTool.Pix_Layers_Mean_Y(0) - mean value of distribution for Y translation,   Pixel barrel layers
  InDetDBTool.Pix_Layers_Mean_Z(0) - mean value of distribution for Z translation,   Pixel barrel layers
  InDetDBTool.Pix_Layers_Mean_A(0) - sigma value of distribution for Alpha rotation, Pixel barrel layers
  InDetDBTool.Pix_Layers_Mean_B(0) - mean value of distribution for Beta rotation,   Pixel barrel layers
  InDetDBTool.Pix_Layers_Mean_G(0) - mean value of distribution for Gamma rotation,  Pixel barrel layers

  InDetDBTool.Pix_Layers_Sigma_X(0) - sigma value of distribution for X translation,  Pixel barrel layers
  InDetDBTool.Pix_Layers_Sigma_Y(0) - sigma value of distribution for Y translation,  Pixel barrel layers
  InDetDBTool.Pix_Layers_Sigma_Z(0) - sigma value of distribution for Z translation,  Pixel barrel layers
  InDetDBTool.Pix_Layers_Sigma_A(0) - sigma value of distribution for Alpha rotation, Pixel barrel layers
  InDetDBTool.Pix_Layers_Sigma_B(0) - sigma value of distribution for Beta rotation,  Pixel barrel layers
  InDetDBTool.Pix_Layers_Sigma_G(0) - sigma value of distribution for Gamma rotation, Pixel barrel layers

  InDetDBTool.Pix_Ba0_X(0.020)  - fixed value for X translation,  pixel barrel layer 0
  InDetDBTool.Pix_Ba0_Y(0.010)  - fixed value for Y translation,  pixel barrel layer 0
  InDetDBTool.Pix_Ba0_Z(0)      - fixed value for Z translation,  pixel barrel layer 0
  InDetDBTool.Pix_Ba0_A(0)      - fixed value for Alpha rotation, pixel barrel layer 0
  InDetDBTool.Pix_Ba0_B(0)      - fixed value for Beta  rotation, pixel barrel layer 0
  InDetDBTool.Pix_Ba0_G(0.006)  - fixed value for Gamma rotation, pixel barrel layer 0
  
  InDetDBTool.Pix_Ba1_X(-0.030) - fixed value for X translation,  pixel barrel layer 1
  InDetDBTool.Pix_Ba1_Y(0.030)  - fixed value for Y translation,  pixel barrel layer 1
  InDetDBTool.Pix_Ba1_Z(0)      - fixed value for Z translation,  pixel barrel layer 1
  InDetDBTool.Pix_Ba1_A(0)      - fixed value for Alpha rotation, pixel barrel layer 1
  InDetDBTool.Pix_Ba1_B(0)      - fixed value for Beta  rotation, pixel barrel layer 1
  InDetDBTool.Pix_Ba1_G(0.005)  - fixed value for Gamma rotation, pixel barrel layer 1
  
  InDetDBTool.Pix_Ba2_X(-0.020) - fixed value for X translation,  pixel barrel layer 2
  InDetDBTool.Pix_Ba2_Y(0.030)  - fixed value for Y translation,  pixel barrel layer 2
  InDetDBTool.Pix_Ba2_Z(0)      - fixed value for Z translation,  pixel barrel layer 2
  InDetDBTool.Pix_Ba2_A(0)      - fixed value for Alpha rotation, pixel barrel layer 2
  InDetDBTool.Pix_Ba2_B(0)      - fixed value for Beta  rotation, pixel barrel layer 2
  InDetDBTool.Pix_Ba2_G(0.004)  - fixed value for Gamma rotation, pixel barrel layer 2
  \endverbatim

  -# Pixel Endcap disks
  \verbatim
  InDetDBTool.DistTypePixelDisks(1)  - distribution type, Pixel endcap disks (in this case: flat)

  InDetDBTool.Pix_Disks_Mean_X(0) - mean value of distribution for X translation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Mean_Y(0) - mean value of distribution for Y translation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Mean_Z(0) - mean value of distribution for Z translation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Mean_A(0) - mean value of distribution for Alpha rotation, Pixel endcap disks
  InDetDBTool.Pix_Disks_Mean_B(0) - mean value of distribution for Beta rotation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Mean_G(0) - mean value of distribution for Gamma rotation, Pixel endcap disks

  InDetDBTool.Pix_Disks_Sigma_X(0.150) - sigma value of distribution for X translation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Sigma_Y(0.150) - sigma value of distribution for Y translation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Sigma_Z(0.200) - sigma value of distribution for Z translation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Sigma_A(0.001) - sigma value of distribution for Alpha rotation, Pixel endcap disks
  InDetDBTool.Pix_Disks_Sigma_B(0.001) - sigma value of distribution for Beta rotation,  Pixel endcap disks
  InDetDBTool.Pix_Disks_Sigma_G(0.001) - sigma value of distribution for Gamma rotation, Pixel endcap disks
  \endverbatim
  Pixel Endcap-A disks
  \verbatim
  InDetDBTool.Pix_EcA_Disk1_X(0) - fixed value for X translation,  Pixel-EcA disk 1
  InDetDBTool.Pix_EcA_Disk1_Y(0) - fixed value for X translation,  Pixel-EcA disk 1
  InDetDBTool.Pix_EcA_Disk1_Z(0) - fixed value for X translation,  Pixel-EcA disk 1
  InDetDBTool.Pix_EcA_Disk1_A(0) - fixed value for Alpha rotation, Pixel-EcA disk 1
  InDetDBTool.Pix_EcA_Disk1_B(0) - fixed value for Beta rotation,  Pixel-EcA disk 1 
  InDetDBTool.Pix_EcA_Disk1_G(0) - fixed value for Gamma rotation, Pixel-EcA disk 1

  InDetDBTool.Pix_EcA_Disk2_X(0) - fixed value for X translation,  Pixel-EcA disk 2
  InDetDBTool.Pix_EcA_Disk2_Y(0) - fixed value for X translation,  Pixel-EcA disk 2
  InDetDBTool.Pix_EcA_Disk2_Z(0) - fixed value for X translation,  Pixel-EcA disk 2
  InDetDBTool.Pix_EcA_Disk2_A(0) - fixed value for Alpha rotation, Pixel-EcA disk 2
  InDetDBTool.Pix_EcA_Disk2_B(0) - fixed value for Beta rotation,  Pixel-EcA disk 2 
  InDetDBTool.Pix_EcA_Disk2_G(0) - fixed value for Gamma rotation, Pixel-EcA disk 2

  InDetDBTool.Pix_EcA_Disk3_X(0) - fixed value for X translation,  Pixel-EcA disk 3
  InDetDBTool.Pix_EcA_Disk3_Y(0) - fixed value for X translation,  Pixel-EcA disk 3
  InDetDBTool.Pix_EcA_Disk3_Z(0) - fixed value for X translation,  Pixel-EcA disk 3
  InDetDBTool.Pix_EcA_Disk3_A(0) - fixed value for Alpha rotation, Pixel-EcA disk 3
  InDetDBTool.Pix_EcA_Disk3_B(0) - fixed value for Beta rotation,  Pixel-EcA disk 3 
  InDetDBTool.Pix_EcA_Disk3_G(0) - fixed value for Gamma rotation, Pixel-EcA disk 3
  \endverbatim
  Pixel Endcap-C disks
  \verbatim
  InDetDBTool.Pix_EcC_Disk1_X(0) - fixed value for X translation,  Pixel-EcC disk 1
  InDetDBTool.Pix_EcC_Disk1_Y(0) - fixed value for X translation,  Pixel-EcC disk 1
  InDetDBTool.Pix_EcC_Disk1_Z(0) - fixed value for X translation,  Pixel-EcC disk 1
  InDetDBTool.Pix_EcC_Disk1_A(0) - fixed value for Alpha rotation, Pixel-EcC disk 1
  InDetDBTool.Pix_EcC_Disk1_B(0) - fixed value for Beta rotation,  Pixel-EcC disk 1 
  InDetDBTool.Pix_EcC_Disk1_G(0) - fixed value for Gamma rotation, Pixel-EcC disk 1

  InDetDBTool.Pix_EcC_Disk2_X(0) - fixed value for X translation,  Pixel-EcC disk 2
  InDetDBTool.Pix_EcC_Disk2_Y(0) - fixed value for X translation,  Pixel-EcC disk 2
  InDetDBTool.Pix_EcC_Disk2_Z(0) - fixed value for X translation,  Pixel-EcC disk 2
  InDetDBTool.Pix_EcC_Disk2_A(0) - fixed value for Alpha rotation, Pixel-EcC disk 2
  InDetDBTool.Pix_EcC_Disk2_B(0) - fixed value for Beta rotation,  Pixel-EcC disk 2 
  InDetDBTool.Pix_EcC_Disk2_G(0) - fixed value for Gamma rotation, Pixel-EcC disk 2

  InDetDBTool.Pix_EcC_Disk3_X(0) - fixed value for X translation,  Pixel-EcC disk 3
  InDetDBTool.Pix_EcC_Disk3_Y(0) - fixed value for X translation,  Pixel-EcC disk 3
  InDetDBTool.Pix_EcC_Disk3_Z(0) - fixed value for X translation,  Pixel-EcC disk 3
  InDetDBTool.Pix_EcC_Disk3_A(0) - fixed value for Alpha rotation, Pixel-EcC disk 3
  InDetDBTool.Pix_EcC_Disk3_B(0) - fixed value for Beta rotation,  Pixel-EcC disk 3 
  InDetDBTool.Pix_EcC_Disk3_G(0) - fixed value for Gamma rotation, Pixel-EcC disk 3
  \endverbatim
  -# SCT Barrel layers
  \verbatim
  InDetDBTool.DistTypeSCTLayers(0) - distribution type, SCT barrel layers (in this case: fixed values)

  InDetDBTool.SCT_Layers_Mean_X(0) - mean value of distribution for X translation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Mean_Y(0) - mean value of distribution for Y translation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Mean_Z(0) - mean value of distribution for Z translation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Mean_A(0) - mean value of distribution for Alpha rotation, SCT barrel layers
  InDetDBTool.SCT_Layers_Mean_B(0) - mean value of distribution for Beta rotation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Mean_G(0) - mean value of distribution for Gamma rotation, SCT barrel layers

  InDetDBTool.SCT_Layers_Sigma_X(0) - sigma value of distribution for X translation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Sigma_Y(0) - sigma value of distribution for Y translation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Sigma_Z(0) - sigma value of distribution for Z translation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Sigma_A(0) - sigma value of distribution for Alpha rotation, SCT barrel layers
  InDetDBTool.SCT_Layers_Sigma_B(0) - sigma value of distribution for Beta rotation,  SCT barrel layers
  InDetDBTool.SCT_Layers_Sigma_G(0) - sigma value of distribution for Gamma rotation, SCT barrel layers

  InDetDBTool.SCT_Ba0_X(0)      - fixed value for X translation,  SCT barrel layer 0
  InDetDBTool.SCT_Ba0_Y(0)      - fixed value for Y translation,  SCT barrel layer 0
  InDetDBTool.SCT_Ba0_Z(0)      - fixed value for Z translation,  SCT barrel layer 0
  InDetDBTool.SCT_Ba0_A(0)      - fixed value for Alpha rotation, SCT barrel layer 0
  InDetDBTool.SCT_Ba0_B(0)      - fixed value for Gamma rotation, SCT barrel layer 0
  InDetDBTool.SCT_Ba0_G(-0.001) - fixed value for Gamma rotation, SCT barrel layer 0
  
  InDetDBTool.SCT_Ba1_X(0.050)  - fixed value for X translation,  SCT barrel layer 1
  InDetDBTool.SCT_Ba1_Y(0.040)  - fixed value for Y translation,  SCT barrel layer 1
  InDetDBTool.SCT_Ba1_Z(0)      - fixed value for Z translation,  SCT barrel layer 1
  InDetDBTool.SCT_Ba1_A(0)      - fixed value for Alpha rotation, SCT barrel layer 1
  InDetDBTool.SCT_Ba1_B(0)      - fixed value for Gamma rotation, SCT barrel layer 1
  InDetDBTool.SCT_Ba1_G(0.009)  - fixed value for Gamma rotation, SCT barrel layer 1
  
  InDetDBTool.SCT_Ba2_X(0.070)  - fixed value for X translation,  SCT barrel layer 2
  InDetDBTool.SCT_Ba2_Y(0.080)  - fixed value for Y translation,  SCT barrel layer 2
  InDetDBTool.SCT_Ba2_Z(0)      - fixed value for Z translation,  SCT barrel layer 2
  InDetDBTool.SCT_Ba2_A(0)      - fixed value for Alpha rotation, SCT barrel layer 2
  InDetDBTool.SCT_Ba2_B(0)      - fixed value for Gamma rotation, SCT barrel layer 2
  InDetDBTool.SCT_Ba2_G(0.008)  - fixed value for Gamma rotation, SCT barrel layer 2
  
  InDetDBTool.SCT_Ba3_X(0.100)  - fixed value for X translation,  SCT barrel layer 3
  InDetDBTool.SCT_Ba3_Y(0.090)  - fixed value for Y translation,  SCT barrel layer 3
  InDetDBTool.SCT_Ba3_Z(0)      - fixed value for Z translation,  SCT barrel layer 3
  InDetDBTool.SCT_Ba3_A(0)      - fixed value for Alpha rotation, SCT barrel layer 3
  InDetDBTool.SCT_Ba3_B(0)      - fixed value for Gamma rotation, SCT barrel layer 3
  InDetDBTool.SCT_Ba3_G(0.007)  - fixed value for Gamma rotation, SCT barrel layer 3
  \endverbatim

  -# SCT endcap disks
  \verbatim
  InDetDBTool.DistTypeSCTDisks(0) - distribution type, SCT endcap disks (in this case: fixed values)

  InDetDBTool.SCT_Disks_Mean_X(0) - mean value of distribution for X translation,  SCT disks
  InDetDBTool.SCT_Disks_Mean_Y(0) - mean value of distribution for Y translation,  SCT disks
  InDetDBTool.SCT_Disks_Mean_Z(0) - mean value of distribution for Z translation,  SCT disks
  InDetDBTool.SCT_Disks_Mean_A(0) - mean value of distribution for Alpha rotation, SCT disks
  InDetDBTool.SCT_Disks_Mean_B(0) - mean value of distribution for Beta rotation,  SCT disks
  InDetDBTool.SCT_Disks_Mean_G(0) - mean value of distribution for Gamma rotation, SCT disks

  InDetDBTool.SCT_Disks_Sigma_X(0) - sigma value of distribution for X translation,  SCT disks
  InDetDBTool.SCT_Disks_Sigma_Y(0) - sigma value of distribution for Y translation,  SCT disks
  InDetDBTool.SCT_Disks_Sigma_Z(0) - sigma value of distribution for Z translation,  SCT disks
  InDetDBTool.SCT_Disks_Sigma_A(0) - sigma value of distribution for Alpha rotation, SCT disks
  InDetDBTool.SCT_Disks_Sigma_B(0) - sigma value of distribution for Beta rotation,  SCT disks
  InDetDBTool.SCT_Disks_Sigma_G(0) - sigma value of distribution for Gamma rotation, SCT disks
  \endverbatim
  SCT Endcap-C disks
  \verbatim
  InDetDBTool.SCT_EcA_Disk1_X(0.050)  - fixed value for X translation,  SCT-EcA disk 1
  InDetDBTool.SCT_EcA_Disk1_Y(0.040)  - fixed value for Y translation,  SCT-EcA disk 1
  InDetDBTool.SCT_EcA_Disk1_Z(0)      - fixed value for Z translation,  SCT-EcA disk 1
  InDetDBTool.SCT_EcA_Disk1_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 1
  InDetDBTool.SCT_EcA_Disk1_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 1
  InDetDBTool.SCT_EcA_Disk1_G(-0.001) - fixed value for Gamma rotation, SCT-EcA disk 1
  
  InDetDBTool.SCT_EcA_Disk2_X(0.010)  - fixed value for X translation,  SCT-EcA disk 2
  InDetDBTool.SCT_EcA_Disk2_Y(-0.080) - fixed value for Y translation,  SCT-EcA disk 2
  InDetDBTool.SCT_EcA_Disk2_Z(0)      - fixed value for Z translation,  SCT-EcA disk 2
  InDetDBTool.SCT_EcA_Disk2_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 2
  InDetDBTool.SCT_EcA_Disk2_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 2
  InDetDBTool.SCT_EcA_Disk2_G(0)      - fixed value for Gamma rotation, SCT-EcA disk 2
  
  InDetDBTool.SCT_EcA_Disk3_X(-0.050) - fixed value for X translation,  SCT-EcA disk 3
  InDetDBTool.SCT_EcA_Disk3_Y(0.020)  - fixed value for Y translation,  SCT-EcA disk 3
  InDetDBTool.SCT_EcA_Disk3_Z(0)      - fixed value for Z translation,  SCT-EcA disk 3
  InDetDBTool.SCT_EcA_Disk3_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 3
  InDetDBTool.SCT_EcA_Disk3_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 3
  InDetDBTool.SCT_EcA_Disk3_G(0.001)  - fixed value for Gamma rotation, SCT-EcA disk 3
  
  InDetDBTool.SCT_EcA_Disk4_X(-0.080) - fixed value for X translation,  SCT-EcA disk 4
  InDetDBTool.SCT_EcA_Disk4_Y(0.060)  - fixed value for Y translation,  SCT-EcA disk 4
  InDetDBTool.SCT_EcA_Disk4_Z(0)      - fixed value for Z translation,  SCT-EcA disk 4
  InDetDBTool.SCT_EcA_Disk4_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 4
  InDetDBTool.SCT_EcA_Disk4_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 4
  InDetDBTool.SCT_EcA_Disk4_G(0.002)  - fixed value for Gamma rotation, SCT-EcA disk 4
  
  InDetDBTool.SCT_EcA_Disk5_X(0.050)  - fixed value for X translation,  SCT-EcA disk 5
  InDetDBTool.SCT_EcA_Disk5_Y(0.040)  - fixed value for Y translation,  SCT-EcA disk 5
  InDetDBTool.SCT_EcA_Disk5_Z(0)      - fixed value for Z translation,  SCT-EcA disk 5
  InDetDBTool.SCT_EcA_Disk5_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 5
  InDetDBTool.SCT_EcA_Disk5_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 5
  InDetDBTool.SCT_EcA_Disk5_G(0.003)  - fixed value for Gamma rotation, SCT-EcA disk 5
  
  InDetDBTool.SCT_EcA_Disk6_X(-0.050) - fixed value for X translation,  SCT-EcA disk 6
  InDetDBTool.SCT_EcA_Disk6_Y(0.030)  - fixed value for Y translation,  SCT-EcA disk 6
  InDetDBTool.SCT_EcA_Disk6_Z(0)      - fixed value for Z translation,  SCT-EcA disk 6
  InDetDBTool.SCT_EcA_Disk6_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 6
  InDetDBTool.SCT_EcA_Disk6_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 6
  InDetDBTool.SCT_EcA_Disk6_G(0.004)  - fixed value for Gamma rotation, SCT-EcA disk 6
  
  InDetDBTool.SCT_EcA_Disk7_X(-0.030) - fixed value for X translation,  SCT-EcA disk 7
  InDetDBTool.SCT_EcA_Disk7_Y(-0.020) - fixed value for Y translation,  SCT-EcA disk 7
  InDetDBTool.SCT_EcA_Disk7_Z(0)      - fixed value for Z translation,  SCT-EcA disk 7
  InDetDBTool.SCT_EcA_Disk7_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 7
  InDetDBTool.SCT_EcA_Disk7_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 7
  InDetDBTool.SCT_EcA_Disk7_G(0.005)  - fixed value for Gamma rotation, SCT-EcA disk 7

  InDetDBTool.SCT_EcA_Disk8_X(0.060)  - fixed value for X translation,  SCT-EcA disk 8
  InDetDBTool.SCT_EcA_Disk8_Y(0.030)  - fixed value for Y translation,  SCT-EcA disk 8
  InDetDBTool.SCT_EcA_Disk8_Z(0)      - fixed value for Z translation,  SCT-EcA disk 8
  InDetDBTool.SCT_EcA_Disk8_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 8
  InDetDBTool.SCT_EcA_Disk8_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 8
  InDetDBTool.SCT_EcA_Disk8_G(0.006)  - fixed value for Gamma rotation, SCT-EcA disk 8
  
  InDetDBTool.SCT_EcA_Disk9_X(0.080)  - fixed value for X translation,  SCT-EcA disk 9
  InDetDBTool.SCT_EcA_Disk9_Y(-0.050) - fixed value for Y translation,  SCT-EcA disk 9
  InDetDBTool.SCT_EcA_Disk9_Z(0)      - fixed value for Z translation,  SCT-EcA disk 9
  InDetDBTool.SCT_EcA_Disk9_A(0)      - fixed value for Alpha rotation, SCT-EcA disk 9
  InDetDBTool.SCT_EcA_Disk9_B(0)      - fixed value for Beta rotation,  SCT-EcA disk 9
  InDetDBTool.SCT_EcA_Disk9_G(0.007)  - fixed value for Gamma rotation, SCT-EcA disk 9
  \endverbatim
  SCT Endcap-C disks
  \verbatim
  InDetDBTool.SCT_EcC_Disk1_X(-0.050) - fixed value for X translation,  SCT-EcC disk 1
  InDetDBTool.SCT_EcC_Disk1_Y(0.050)  - fixed value for Y translation,  SCT-EcC disk 1
  InDetDBTool.SCT_EcC_Disk1_Z(0)      - fixed value for Z translation,  SCT-EcC disk 1
  InDetDBTool.SCT_EcC_Disk1_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 1
  InDetDBTool.SCT_EcC_Disk1_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 1
  InDetDBTool.SCT_EcC_Disk1_G(0.008)  - fixed value for Gamma rotation, SCT-EcC disk 1
  
  InDetDBTool.SCT_EcC_Disk2_X(0)      - fixed value for X translation,  SCT-EcC disk 2
  InDetDBTool.SCT_EcC_Disk2_Y(0.080)  - fixed value for Y translation,  SCT-EcC disk 2
  InDetDBTool.SCT_EcC_Disk2_Z(0)      - fixed value for Z translation,  SCT-EcC disk 2
  InDetDBTool.SCT_EcC_Disk2_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 2
  InDetDBTool.SCT_EcC_Disk2_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 2
  InDetDBTool.SCT_EcC_Disk2_G(0)      - fixed value for Gamma rotation, SCT-EcC disk 2
  
  InDetDBTool.SCT_EcC_Disk3_X(0.020)  - fixed value for X translation,  SCT-EcC disk 3
  InDetDBTool.SCT_EcC_Disk3_Y(0.010)  - fixed value for Y translation,  SCT-EcC disk 3
  InDetDBTool.SCT_EcC_Disk3_Z(0)      - fixed value for Z translation,  SCT-EcC disk 3
  InDetDBTool.SCT_EcC_Disk3_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 3
  InDetDBTool.SCT_EcC_Disk3_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 3
  InDetDBTool.SCT_EcC_Disk3_G(0.001)  - fixed value for Gamma rotation, SCT-EcC disk 3
  
  InDetDBTool.SCT_EcC_Disk4_X(0.040)  - fixed value for X translation,  SCT-EcC disk 4
  InDetDBTool.SCT_EcC_Disk4_Y(-0.080) - fixed value for Y translation,  SCT-EcC disk 4
  InDetDBTool.SCT_EcC_Disk4_Z(0)      - fixed value for Z translation,  SCT-EcC disk 4
  InDetDBTool.SCT_EcC_Disk4_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 4
  InDetDBTool.SCT_EcC_Disk4_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 4
  InDetDBTool.SCT_EcC_Disk4_G(-0.008) - fixed value for Gamma rotation, SCT-EcC disk 4
  
  InDetDBTool.SCT_EcC_Disk5_X(0)      - fixed value for X translation,  SCT-EcC disk 5
  InDetDBTool.SCT_EcC_Disk5_Y(0.030)  - fixed value for Y translation,  SCT-EcC disk 5
  InDetDBTool.SCT_EcC_Disk5_Z(0)      - fixed value for Z translation,  SCT-EcC disk 5
  InDetDBTool.SCT_EcC_Disk5_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 5
  InDetDBTool.SCT_EcC_Disk5_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 5
  InDetDBTool.SCT_EcC_Disk5_G(0.003)  - fixed value for Gamma rotation, SCT-EcC disk 5
  
  InDetDBTool.SCT_EcC_Disk6_X(0.010)  - fixed value for X translation,  SCT-EcC disk 6
  InDetDBTool.SCT_EcC_Disk6_Y(0.030)  - fixed value for Y translation,  SCT-EcC disk 6
  InDetDBTool.SCT_EcC_Disk6_Z(0)      - fixed value for Z translation,  SCT-EcC disk 6
  InDetDBTool.SCT_EcC_Disk6_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 6
  InDetDBTool.SCT_EcC_Disk6_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 6
  InDetDBTool.SCT_EcC_Disk6_G(-0.004) - fixed value for Gamma rotation, SCT-EcC disk 6
  
  InDetDBTool.SCT_EcC_Disk7_X(0)      - fixed value for X translation,  SCT-EcC disk 7
  InDetDBTool.SCT_EcC_Disk7_Y(-0.060) - fixed value for Y translation,  SCT-EcC disk 7
  InDetDBTool.SCT_EcC_Disk7_Z(0)      - fixed value for Z translation,  SCT-EcC disk 7
  InDetDBTool.SCT_EcC_Disk7_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 7
  InDetDBTool.SCT_EcC_Disk7_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 7
  InDetDBTool.SCT_EcC_Disk7_G(0.004)  - fixed value for Gamma rotation, SCT-EcC disk 7
  
  InDetDBTool.SCT_EcC_Disk8_X(0.030)  - fixed value for X translation,  SCT-EcC disk 8
  InDetDBTool.SCT_EcC_Disk8_Y(0.030)  - fixed value for Y translation,  SCT-EcC disk 8
  InDetDBTool.SCT_EcC_Disk8_Z(0)      - fixed value for Z translation,  SCT-EcC disk 8
  InDetDBTool.SCT_EcC_Disk8_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 8
  InDetDBTool.SCT_EcC_Disk8_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 8
  InDetDBTool.SCT_EcC_Disk8_G(0.006)  - fixed value for Gamma rotation, SCT-EcC disk 8
  
  InDetDBTool.SCT_EcC_Disk9_X(0.040)  - fixed value for X translation,  SCT-EcC disk 9
  InDetDBTool.SCT_EcC_Disk9_Y(0.050)  - fixed value for Y translation,  SCT-EcC disk 9
  InDetDBTool.SCT_EcC_Disk9_Z(0)      - fixed value for Z translation,  SCT-EcC disk 9
  InDetDBTool.SCT_EcC_Disk9_A(0)      - fixed value for Alpha rotation, SCT-EcC disk 9
  InDetDBTool.SCT_EcC_Disk9_B(0)      - fixed value for Beta rotation,  SCT-EcC disk 9
  InDetDBTool.SCT_EcC_Disk9_G(-0.007) - fixed value for Gamma rotation, SCT-EcC disk 9
  \endverbatim

\subsection subsection3 LEVEL 1

\verbatim
     InDetDBTool.DistTypeID(0) - distribution type, ID subdetectors (in this case: fixed values)

     InDetDBTool.ID_Mean_X(0) - mean value of distribution for X translation,  ID subdetectors
     InDetDBTool.ID_Mean_Y(0) - mean value of distribution for Y translation,  ID subdetectors
     InDetDBTool.ID_Mean_Z(0) - mean value of distribution for Z translation,  ID subdetectors
     InDetDBTool.ID_Mean_A(0) - mean value of distribution for Alpha rotation, ID subdetectors
     InDetDBTool.ID_Mean_B(0) - mean value of distribution for Beta  rotation, ID subdetectors
     InDetDBTool.ID_Mean_G(0) - mean value of distribution for Gamma rotation, ID subdetectors

     InDetDBTool.ID_Sigma_X(0) - sigma value of distribution for X translation,  ID subdetectors
     InDetDBTool.ID_Sigma_Y(0) - sigma value of distribution for Y translation,  ID subdetectors
     InDetDBTool.ID_Sigma_Z(0) - sigma value of distribution for Z translation,  ID subdetectors
     InDetDBTool.ID_Sigma_A(0) - sigma value of distribution for Alpha rotation, ID subdetectors
     InDetDBTool.ID_Sigma_B(0) - sigma value of distribution for Beta  rotation, ID subdetectors
     InDetDBTool.ID_Sigma_G(0) - sigma value of distribution for Gamma rotation, ID subdetectors

     InDetDBTool.Pix_X(0.60)            - fixed value for X translation,  Pixel (whole detector: barrel+endcaps)
     InDetDBTool.Pix_Y(1.05)            - fixed value for Y translation,  Pixel (whole detector: barrel+endcaps)
     InDetDBTool.Pix_Z(1.15)            - fixed value for Z translation,  Pixel (whole detector: barrel+endcaps)
     InDetDBTool.Pix_A(-0.10*0.001)     - fixed value for Alpha rotation, Pixel (whole detector: barrel+endcaps)
     InDetDBTool.Pix_B(0.25*0.001)      - fixed value for Beta rotation,  Pixel (whole detector: barrel+endcaps)
     InDetDBTool.Pix_G(0.65*0.001)      - fixed value for Gamma rotation, Pixel (whole detector: barrel+endcaps)
  
     InDetDBTool.SCT_Ba_X(0.70)         - fixed value for X translation,  SCT barrel
     InDetDBTool.SCT_Ba_Y(1.20)         - fixed value for Y translation,  SCT barrel
     InDetDBTool.SCT_Ba_Z(1.30)         - fixed value for Z translation,  SCT barrel
     InDetDBTool.SCT_Ba_A(0.10*0.001)   - fixed value for Alpha rotation, SCT barrel
     InDetDBTool.SCT_Ba_B(0.05*0.001)   - fixed value for Beta rotation,  SCT barrel
     InDetDBTool.SCT_Ba_G(0.80*0.001)   - fixed value for Gamma rotation, SCT barrel

     InDetDBTool.SCT_EcA_X(2.10)        - fixed value for X translation,  SCT-EcA
     InDetDBTool.SCT_EcA_Y(-0.80)       - fixed value for Y translation,  SCT-EcA
     InDetDBTool.SCT_EcA_Z(1.80)        - fixed value for Z translation,  SCT-EcA
     InDetDBTool.SCT_EcA_A(-0.25*0.001) - fixed value for Alpha rotation, SCT-EcA
     InDetDBTool.SCT_EcA_B(0)           - fixed value for Beta rotation,  SCT-EcA
     InDetDBTool.SCT_EcA_G(-0.50*0.001) - fixed value for Gamma rotation, SCT-EcA

     InDetDBTool.SCT_EcC_X(-1.90)       - fixed value for X translation,  SCT-EcA
     InDetDBTool.SCT_EcC_Y(2.00)        - fixed value for Y translation,  SCT-EcA
     InDetDBTool.SCT_EcC_Z(-3.10)       - fixed value for Z translation,  SCT-EcA
     InDetDBTool.SCT_EcC_A(-0.10*0.001) - fixed value for Alpha rotation, SCT-EcA
     InDetDBTool.SCT_EcC_B(0.05*0.001)  - fixed value for Beta rotation,  SCT-EcA
     InDetDBTool.SCT_EcC_G(0.40*0.001)  - fixed value for Gamma rotation, SCT-EcA
\endverbatim

@section InDetAlignGenTools_InDetAlignTrackSelTool 

This tool provides a standard track selection for 'alignment quality' tracks
(for the SCT and pixels),
allowing this to be standardised among the various alignment algorithms
being developed. It currently makes a selection based on the track
transverse momentum (p_T), number of 'holes' on the track in the SCT and 
pixel detectors (i.e. layers crossed by the track but having no associated
hit), and number of 'shared' hits assigned to more than one track. The latter
quantity is calcuated internally by the tool, by analysing the set of tracks
it is passed in a preliminary fillMap call (since the information is not
calculated in the TrkTrack framework). 

The tool is used as follows:

 - create map by passing as argument to the tool a collection of tracks:
<pre>
// track collection
const DataVector<Trk::Track>* trks;

//retrieve pre-existing collection from StoreGate
sc = m_StoreGate->retrieve(trks, m_trackCollName);
  
// fill the map
m_alignTrackSelTool->fillMap(trks);
</pre>

 - loop over tracks and check if each track passes cuts or not:
<pre>
DataVector<Trk::Track>::const_iterator trkit;
for (trkit=trks->begin(); trkit!=trks->end();++trkit) {
   int stat = m_alignTrackSelTool->getStatus(**trkit);
   ...
  }
</pre>
The method getStatus returns 1 if the track passes cuts, or 0 otherwise.
Note that the fillMap step is essential to correctly fill the information
on shared hits.

An example of the use of this tool can be found in the InDetAlignNt
algorithm in InnerDetector/InDetAlignment/InDetAlignAlgs.

@section InDetAlignGenTools_InDetAlignTrackSelToolJobOptions InDetAlignTrackSelTool JobOptions

@include InDetAlignTrackSelTool_jobOptions.py





*/
