/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AODFix

@section AODFix_AODFixIntro Introduction

AODFix is a way to break frozen tier-0 in a controlled way. When
reading "unfixed" AODs or AODs with a fix that is not current, it
schedules certain algorithms to "fix" the AOD. If the fixed AODs are
written out, in-file metadata is updated indicating which AODFix has
been applied. The metadata is a list of the fixes applied with _AOD
suffix suffix. Note that fixes are not re-applied (unless forced).

The related RecoFix package handles the case when fixes are propagated
to new reconstruction. It uses the same metadata, but with an _ESD suffix.

By default, AODFix schedules itself when reading AODs (not ESDs or
DAODs), and if the AOD was produced in a matching release Releases are
considred matching if the first two numbers, like 21.0, match, though
it can be increased, e.g. that 21.0 and 21.2 are considered matching
releases. Note that now AODFix runs on nightlies as if they were
actual releases.

A flag, rec.doApplyAODFix, can be locked to True or False if you want
to modify the behavior. (Note, it has to be locked.)  Locking it to
False always prevents AODFix from scheduling fixes or adding metadata,
while locking it to True can be used to run AODFix or ESDs or DOADs
(provided all the required inputs are available) or to rerun AODFix if
the metadata suggests it doesn't need to be rerun.

@section AODFix_AODFixOverview Module and Class Overview
  The AODFix package contains of following modules:

  - python.AODFix : The module that contains the interface to AODFix. No other module should be imported externally.
  - python.AODFix_base.AODFix_base : The AODFix base class
  - python.AODFix_r210.AODFix_r210 : The class implementing the release 21.0/21.2 AODFix


@author Rolf Seuster
@author Jovan Mitrevski

*/
