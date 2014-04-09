/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  G4DetectorEnvelopes
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4DetectorEnvelopes Introduction

This package defines the envelopes to be used during G4 simulation.  The envelopes are further specified in G4AtlasApps.  Each subsystem is allowed to wiggle around inside of its own envelope; including the envelopes ensures that they don't run into each other.

@section G4DetectorEnvelopes Class Overview

The package includes the following classes:

 - Atlas , GenericPCONEnvelope , GenericPGONEnvelope , ResizeableBoxEnvelope , ResizeableTrdEnvelope , ResizeableTubsEnvelope : Generic envelopes from which specific envelopes may be constructed (all the types are available in the python layer for user access)
 - Calorimeter , CTBMagnet , InnerDetector , Muon , MuonSystemEntryLayer : Specific envelopes used during simulation for the subdetectors
 - EnvelopeGeometryManager : Defines the envelopes to be used and keeps track of all of them.  Also defines the primary numbers for envelope creation during simulation (or their defaults, at least)
 - EnvelopesXMLHandler : Provides an XML parser for reading in envelope information from an XML file

@ref used_G4DetectorEnvelopes

@ref requirements_G4DetectorEnvelopes

*/

/**
@page used_G4DetectorEnvelopes Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_G4DetectorEnvelopes Requirements
@include requirements
*/

