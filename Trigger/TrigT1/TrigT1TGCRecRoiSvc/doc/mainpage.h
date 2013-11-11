/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
  @mainpage TrigT1TGCRecRoiSvc Package

  @author Chihiro Omachi<omati@phys.sci.kobe-u.ac.jp>
  @author Yoji Hasegawa<Yoji.Hasegawa@cern.ch>
  @author Naoko Kanaya<Naoko.Kanaya@cern.ch>

  @section TrigT1TGCRecRoiSvcIntro Introduction
  
  This package provides tools to calculate RoI position and size for a 
  given RoI word with MuonGeoModel, TGCIdHelper and TGCCablingSvc.


  @section TrigT1TGCRecRoiSvc Class Overview
  
  The packages includes one algorithm, TrigRecRoiSvc;

  - LVL1TGC::TGCRecRoiSvc: provide RoI size and center position.

  @section TrigT1TGCRecRoiSvc methods

  The following functions are available:
  
  - @link LVL1TGC::TGCRecRoiSvc::reconstruct @endlink : calculate a center of RoI(eta, phi) for a given RoI word.
  - @link LVL1TGC::TGCRecRoiSvc::eta @endlink : get eta of RoI center
  - @link LVL1TGC::TGCRecRoiSvc::phi @endlink : get phi of RoI center
  - @link LVL1TGC::TGCRecRoiSvc::RoIsize @endlink : return the maximum/minimum eta and phi values for a given RoI word.
   
  @section TrigT1TGCRecRoiSvc Extra Packages
  - @ref used_TrigT1TGCRecRoiSvc
  - @ref requirements_TrigT1TGCRecRoiSvc

*/

/**
   @page used_TrigT1TGCRecRoiSvc Used Packages
   @htmlinclude used_packages.html
*/

/**
   @page requirements_TrigT1TGCRecRoiSvc Requirements
   @include requirements
*/


