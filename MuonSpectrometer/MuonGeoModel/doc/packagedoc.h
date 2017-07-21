/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonGeoModel_page The MuonGeoModel Package

@section MuonGeoModel_IntroMuonGeoModel Introduction

This package provides the implementation of GeoModel for the various MuonSpectrometer layouts (from layout P to layout R'').
This geometry description is used for simulation (feed to G4 via the Geo2G4 tool) and reconstruction.
Reconstruction and generic geometry clients will need to depend on a few classes of MuonGeoModel.
This sub-set of classes, usually referred as "readout geometry", will be decoupled from the so called "raw geometry",
i.e. the tree of volumes, with associated materials and transforms which are directly translated into G4.

@section MuonGeoModel_MGMRawGeometry MuonGeoModel Raw Geometry
For the moment, please, refer to http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/detector_description/Muon.html

@section MuonGeoModel_MGMReadoutGeometry MuonGeoModel Readout Geometry
For the moment, please, refer to http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/detector_description/Muon.html

@section MuonGeoModel_MuonGeoModelClasses Class Overview

The MuonGeoModel package contains the following main classes:

MuonDetectorTool    - handled by GeoModelSvc to build the MS description),
MuonDetectorFactory - creates the actual volume tree for the MS + the related Readout Geometry objects),
MuonChamber         - used by the Factory, it builds the raw + readout geometry for each station in the MS;
                      in particular, it builds the station envelop, instantiate and build all station components,
                      creates the corresponding MuonStation (RO geometry) and all XxxReadutElements (RO geometry)
                      associated to its detector components, records in the Manager the RO info it generates.
Mdt, Rpc, Tgc, Csc build the raw geometry of each type of detector components -instantiated and built by the MuonChamber.
Multilayer    (Raw geom) created and built by Mdt - generates the whole tree of volumes in a multilayer (up to drift tubes).
DriftTube     (Raw geom) created and built by Multilayer.
RpcLayer      (Raw geom) created and built by Rpc - generates the whole tree of volumes in a Rpc (up to the active gas).
CscMultilayer (Raw geom) created and built by Ccs - generates the whole tree of volumes in a Csc chamber (up to the active gas).
Ded, Spacer, SpacerBeam, Sup are (Raw geom builders) created and built by MuonChamber to describe passive components of stations. 
BarrelToroidBuilderRDB, CalorimeterSaddleBuilder,  EndCapToroidBuilderRDB,  FeetToroidBuilderRDB,  ShieldBuilderRDB
are classes building big dead matter components (used by the Factory). 
Classes which build the raw geometry are un-necessary after the build-up is complete are appropriately deleted by the object producing them.

MuonDetectorManager   - holds the geometry information relevant to clients
MdtReadoutElements    - holds the Mdt geometry information relevant to clients
CscReadoutElements    - holds the Csc geometry information relevant to clients
RpcReadoutElements    - holds the Rpc geometry information relevant to clients
TgcReadoutElements    - holds the Tgc geometry information relevant to clients
MuonStation           - holds the station-related geometry information relevant to clients

DBReader and RDBReader are classes used to read and store in temporary objects the primary numbers in the Geometry Database.
Temporary objects allowing to store in a structured way the primary numbers and, afterwards, used by the raw geometry builders are:
Position,  AlignPos, Station,  Technology (+ MDT, RPC, TGC, CSC), MYSQL, etc. They are all deleted after the MS volume tree is fully built.

Please, refer to the Classes section for more details (especially about the Readout Geometry classes).



*/
