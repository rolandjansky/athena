/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonHoughPatternEvent_page MuonHoughPatternEvent Package
@author jsnuverink@nikhef.nl

@section MuonHoughPatternEvent_MuonHoughPatternEventIntro Introduction

This package contains the different HoughTransforms used in the HoughPatternTool and also the internal EDM. Every houghtransformer does both the histogram filling and the association step.

@section MuonHoughPatternEvent_MuonHoughPatternEventOverview Class Overview
  The MuonHoughPatternEvent package contains of following classes:

  - MuonHoughHisto2D: Histogram class, similar to Root's TH2D
  - MuonHoughHisto2DContainer: Container class of MuonHoughHisto2D
  - MuonHoughHit: Internal Hit class
  - MuonHoughHitContainer: Container class of MuonHoughHit
  - MuonHoughMathUtils: Class with Mathematical functions, both general and trackmodel specific
  - MuonHoughPattern: Internal Pattern class
  - MuonHoughPatternCollection: Container class of MuonHoughPattern
  - MuonHoughTransformSteering: class that steers the initialization of HoughTransformers
  - MuonHoughTransformer: abstract base class
  - MuonHoughTransformer_CurvedAtACylinder: HoughTransformer in rz plane with curved track model (Atlas magnets bend charged particles in rz plane). Assumes muons from interaction point
  - MuonHoughTransformer_rz: HoughTransformer in rz plance with straight track model, used for cosmics
  - MuonHoughTransformer_xy: HoughTransformer in xy plane with straight track model
  - MuonHoughTransformer_xyz: base class for HoughTransformers in carthesian coordinates, straight track model
  - MuonHoughTransformer_yz: HoughTransformer in yz plane with straight track model, currently not in use



*/
