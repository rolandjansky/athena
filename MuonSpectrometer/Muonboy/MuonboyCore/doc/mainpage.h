/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MuonboyCore Package

@author ahmimed.ouraou@cea.fr, jean-francois.laporte@cea.fr

@section IntroductionMuonboyCore Introduction

MuonboyCore is a package of the Muonboy suite.
It performs reconstruction in the Muon Spectrometer.

@section DescriptionMuonboyCore Description

The largest part of MuonboyCore, doing the effective muon reconstruction, is implemented in fortran 90.
At the initialization of the reconstruction, the routine matiere.F90 builds the dead matter and the routine amdcox.F90 transfers the Amdc geometry of the spectrometer into an internal Muonboy representation.
Then, the steering routine RunMuonboy from inirunendmuonboy.F90 performs the following steps

  - Do the T0 fit if necessary (routine t0fitter.F90)
  - Perform Segment reconstruction (routine recoseg.F90)
    - Reconstruction of the Region of Activity (routine doallowcell.F90)
    - Clusterize the RPC and TGC trigger strips (routine doclusters.F90)
    - Reconstruct segments in the CSC (routine filicsc.F90)
    - Reconstructs strict segments, then loose segments in MDT (routine findra.F90)
  - perform track reconstruction (routine recotra.F90)
    - Perform the track fit (routine trackfit.F90)
    - Backtrack to the IP (routine baktrak.F90)
    - Compute the error (routine matcov.F90)

@section OverviewMuonboyCore Class Overview

  - MuonboyInterface : this class is interfacing Muonboy reconstruction
  
  - MuonboyConfigure : this class manages configurations variables
    GetSetMuonboyVariable is a set of functions to manipulate
    configutations variables
    
  - MuonboyCocoupMaker : this class loads digits
  
  - MuonboyRecMaker : this class performs reconstruction
      
  - MB_SegmentStoreMaker : this class builds segments collection
  
  - MB_TrackStoreMaker : this class builds tracks collection
  
  - AandBlinesLoader : this class loads the stored A and B lines
  
  - MuonboyBFAccess and MuonboyRTAccess : these classes manage
    connection to B Field and RT.  MuonboyBFc and MuonboyRTc 
    are a set of functions to access B Field and RT
  
  - MuonboyCollectMatter, MuonboyPosmusc  and MuonboyTrmusc: these 
    classes are performing matter collection
  
  - MuonboyCocyli,  MuonboyCofiti , MuonboyCointr , MuonboyComcan
    and MuonboyComusc gives access to reconstruction results

  - AgddXml : This class is an interface for Agdd Xml variables extraction

  - MuonboyMaterialTagInfo : This class manages update of Muonboy material  
  

The MuonboyCore package contains:

@section ExtrasMuonboyCore Extra Pages

  - @ref used_MuonboyCore
  - @ref requirements_MuonboyCore

*/

/**
@page used_MuonboyCore Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MuonboyCore Requirements
@include requirements
*/
