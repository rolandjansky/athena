/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Mainpage for doxygen

/**
@page LArReadoutGeometry_page package LArReadoutGeometry

LArReadoutGeometry is the detector description for the Liquid Argon 
Calorimeter. Users can interact with this package to obtain details
about the geometry of readout cells in the Electromagnetic Barrel (EMB),
the Electromagnetic Endcap (EMEC), the Hadronic Endcap Calorimeter (HEC),
and the Forward Calorimeter (FCAL).  This layer aims to be a technically
accurate description of the LAr Readout Geometry.  It is synched to 
the LArGeoModel description, and follows the alignment of the LArGeoModel
elements, even if it changes.

Four detector managers provide access to very detailed information about the
readout cells of the LAr and other important characteristics of the 
calorimeters which is over and above the description of their material.

  - EMBDetectorManager
  - EMECDetectorManager
  - HECDetectorManager
  - FCALDetectorManager

These managers may generally be retreived from storegate through the
following access pattern:

<pre>
  const EMBDetectorManager * manager=NULL;
  ATH_CHECK( detStore()->retrieve(manager) );
  if (manager) {
  }
</pre>

From the managers, one retrieves information about the calorimeter regions
and their granularity as well as other key numbers such as focal distances,
sampling separations, and much more.  LArReadoutGeometry and the LArGeoModel
packages aim to centralize all the information about the LAr detectors though
in practice much of that information still resides in other parts of the 
code. 

Collaborating packages and their relationship to LArReadoutGeometry are 
listed below:

 - LArGeoModel/LArGeoAlgsNV

This package builds both the readout geometry and the GeoModel geometry, and
also attaches the alignable transformations to the callback mechanism that
changes their values at run boundaries. For the construction of the geometry,
LArGeoAlgsNV farms out the majority of the actual work to the following packages;

 - LArGeoModel/LArGeoBarrel
 - LArGeoModel/LArGeoEndcap
 - LArGeoModel/LArGeoHec
 - et cetera...

These packages create the geomodel geometry.  Test beam configurations also 
lie alongside the full ATLAS geometries in the same directory:

 - LArGeoH62002Algs, a dedicated factory for the 2002 H6 Beam Test
 - LArGeoH62003Algs, a dedicated factory for the 2003 H6 Beam Test
 - et cetera...

And these use subpackages such as:

 - LArGeoH6Cryostats
 - LArGeoTBEC
 - et cetera...

to do most of the actual, detailed geometry layer.  

 - CaloDetDescr

provides both a subsystem-independent description all four calorimeter subsystems
which has historically been the preferred access pattern for reconstruction algorithms,
though some of the abstractions must be stretched, conceptually, to apply to 
the entire calorimeter.   In the LArReadoutGeometry package, we have avoided,
 providing methods to retrieve quantities lacking a precise definition.
Examples include the positions of eta boundary of a nonprojective cell, or, 
the phi-width of an FCAL cell.  We urge people to use the LArReadoutGeometry
classes whenever they can.

All detector managers provide access to information about the Tree-Tops (physical
volumes) through the methods getNumTreeTops() and getTreeTop(unsigned int i). 
In the following sections we describe more fully the additional information 
available form EMBDetectorManager, EMECDetectorManager, HECDetectorManager,
and FCALDetectorManager.


@section LArReadoutGeometry_EMB EMBDetectorManager and Related Classes

The EMBDetectorManager provides both iterative and random access to the detector
regions (class EMBDetectorRegion).  For random access, an endcap index, sampling
index, and region index are required. The detector regions can provide their 
absolute transforms, as well as links to a cell information. 
The actual cell is created on the fly, is reference counted and is destroyed 
when the last link goes out of scope.  The cells can provide their own eta,
phi, and r, and z boundaries (in the local reference frame).  The complete picture
of where the cell boundaries actually lie in the global reference frame of ATLAS
can be constructed by composing the alignment information with the information 
about cell boundaries in the local reference frame.   

The EMBDetectorManager also provides access to certain key numbers in the EMB.
These are held in a structure called EMBBasicReadoutNumbers. These constants 
originate in the database, and the intention is to provide a friendlier interface
to those numbers than RDBAccessSvc, which in any case should not be accessed
during event processing. The class EMBAccordionDetails is similar, but it has
not yet been fully developed and so it is, for now, not defined.

@section LArReadoutGeometry_EMEC EMECDetectorManager and Related Classes

The EMECDetector manager provides access to four detector regions (two endcaps
for both inner and outer wheels).  The regions (class EMECDetectorRegion) can 
be accessed randomly or  iteratively.  The EMEC DetectorRegions provide access
to individual cell boundaries at either the front, middle, or back of the cell.

In the EMEC the cell boundaries are constructed so that, extrapolated forward
from the front face of the EMEC, they converge at a point called the EMEC focal
point.  Thus, the eta-boundaries together with the position of the focal point
determine the position of the cell separation boundaries on the physical emec.
Within the EMEC itself, a reference plane lies at the back of an element called
the front transversal bar.  This is the zero of the local coordinate system.  
The EMEC detector manager provides the distance of the focal length from the
reference plane.  It also provides the distance of the active part of the EMEC
to the reference plane.  The regions can return the position of the focal point,
3 coordinates,  and the position of the center of the reference plane, 3 
coordinates.   These points are not put into the EMEC construction procedure; instead
they come out of the EMEC construction procedure. The physical construction of the
EMEC together with the alignable transformations, normally derived from the position
of the back face of the cryostat, determine the location of the focal points.
A number of important database constants (e.g. Sampling Separations) are also part
of the interface of EMECDetectorManager.

@section LArReadoutGeometry_HEC HECDetectorManager and Related Classes

The HEC is a semi-projective calorimeter.  The eta-boundaries follow a staircase
shape, and the "eta-boundaries" follow the staircase.  In addition, the HEC,
like the EMEC, is displaced so that the HEC focal point does not in reality coincide
with the interaction point. The HECDetectorManager provides access to information
about the readout regions (class HECDetectorRegion)  within the HEC, in addition 
to the longitudinal blocks,  which are nonprojective cells out of which one makes 
projective towers. The distance from focal point to reference point is available, 
as is the position of the focal point in global coordinates.   The reference point 
is the front face of the HEC.  We note with regret that the HEC is currently 
implemented in one wheel, not two. This is going to be changed for version 14.

The HEC Cells provide a little more information than EMEC and EMB because, due to
the way they are built from longitudinal blocks, more information is required to
fully describe them.  The HECCell methods whose name includes the string Nominal,
such as getRMaxLocalNominal() and getEtaMinNominal() refer to the idealized
HEC boundaries, while those lacking that string refer to the actual staircase
shaped HEC boundaries which are described in some detail in this packages.  Boundaries
can be obtained for the front, middle, or back of a cell.

@section LArReadoutGeometry_FCAL FCALDetectorManager and Related Classes

The FCALDetectorManager can return six modules (class FCALModules)  which is the FCAL
equivalent of a detector region.  The modules return tiles (class FCALTile) which is 
the equivalent of a cell in the FCAL.  The tiles respond to queries about their indices,
their position, and the number of tubes, and, for performance reasons the module must
be queried to determine the lateral dimensions of the FCAL tile.  

A lot of additional readout information about the FCAL is provided by the FCALChannelMap,
which is available through the interface of FCALDetectorManager.



@section LArReadoutGeometry_DESCRIPTORS Descriptors.

All of the classes discussed above have descriptors for the regions.  These are
not considered user classes and so are not discussed in any detail here.  

@section LArReadoutGeometry_PHI Phi boundaries and indices.

The basic idea behind the phi boundaries and numbering is that the two endcaps
are created identically but positioned differently in the detector.  However,
this means that, since ATLAS indexes both endcaps such that phi=0 occurs at
x=0, it means that the readout geometry must be layered differently on either
end of the detector.  The negative endcap thus has a local coordinate that 
starts at M_PI, decreases to zero, and increases to -M_PI.   This produces, in global
coordinates, a phi that increases with index from 0 to 2*M_PI.

@section LArReadoutGeometry_VIS Visualization of LArReadout.

The v-atlas program (soon to be replaced by the vp1 program) can show the LAr
readout geometry including cell boundaries and focal point postions. One can
select the LArReadoutSystem.  This displays directly the LArReadoutGeometry.
Another related v-atlas display system is the CaloReadoutSystem, which displays
the CaloDetDescr interpretaion of the LArReadoutGeometry geometry.  



*/
