/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetReadoutGeometry_page InDetReadoutGeometry Package

@authors Grant Gorfine, Joe Boudreau, Thomas Kittlemann, 
         Davide Costanzo, David Calvet, Alessandro Fornaini

@section InDetReadoutGeometry_InDetReadoutGeometryIntro Introduction

This package contains the readout classes for
the inner detector. These are mainly used by digitization and
reconstruction. 


- Managers:
  The Detector managers contain methods to access the Identifier helpers and detector elements.


- Detector Elements:
The detector element is the object where geometrical information of the readout elements is obtained: 
  -  Pixel: The silicon sensor in a pixel module. 
  -  SCT: One side of the module. (One or two silicon wafers wire bonded together)
  -  TRT: A straw layer (contains several straws).

- Design and Descriptor Classes:
As well as the detector element there are design and descriptor classes which contain information common to several elements.


@section InDetReadoutGeometry_InDetReadoutGeometryClassOverview Class Overview

Described here are just the classes that clients will 
normally use.

- Managers:
  - InDetDD::SiDetectorManager: Base class for Pixel and SCT managers.
  - InDetDD::PixelDetectorManager: The Pixel detector manager.
  - InDetDD::SCT_DetectorManager: The SCT detector manager.
  - InDetDD::TRT_DetectorManager: The TRT detector manager.

- Detector Elements, Design and Descriptor Classes:
  - InDetDD::SiDetectorElement: Pixel and SCT share a common detector element class. This class contains most of the methods needed by reconstruction clients.
  - InDetDD::SiDetectorDesign: Base class for design.  
  - InDetDD::PixelModuleDesign: 
  - InDetDD::SCT_ModuleSideDesign:
  - InDetDD::SCT_BarrelModuleSideDesign:
  - InDetDD::SCT_ForwardModuleSideDesign:
  - InDetDD::TRT_BaseElement: 
  - InDetDD::TRT_BarrelElement:
  - InDetDD::TRT_EndcapElement:
  - InDetDD::TRT_BarrelDescriptor:
  - InDetDD::TRT_EndcapDescriptor:


- Other: 
  - InDetDD::SiCellId: Strip number for SCT or phi_index, eta_index for pixel.
  - InDetDD::Version: Version information
  - InDetDD::TRT_Numerology: TRT numerology 
  - InDetDD::SiNumerology: Pixel and SCT numerology

@section InDetReadoutGeometry_InDetReadoutGeometryUsage Usage Notes
The managers are retrieved from storegate. Their names are Pixel, SCT and TRT.
From the manager, one can get the id helpers and access the detector elements using various methods. 




*/
