/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page RecoFix

@section RecoFix_RecoFixIntro Introduction

AODFix and RecoFix are a way to break frozen tier-0 in a controlled
way. The related AODFix package handles the fixing AODs. RecoFix
handles the case when fixes are propagated to new reconstruction. It's
task is to set the AODFix metadata appropriately indicating what has
been fixed. It adds _ESD suffix.

@section RecoFix_RecoFixOverview Module and Class Overview
  The RecoFix package contains of following modules:

  - python.RecoFix : The module that contains the interface to RecoFix. No other module should be imported externally.
  - python.RecoFix_base.RecoFix_base : The RecoFix base class
  - python.RecoFix_r210.RecoFix_r210 : The class implementing the release 21.0/21.2 RecoFix


@author Rolf Seuster
@author Jovan Mitrevski

*/
