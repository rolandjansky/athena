/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage

@section RecoFixIntro Introduction

AODFix is a way to break frozen tier-0 in a controlled way. It does it
in two ways: 

 - When reading "unfixed" AODs or AODs with a fix that is
not current, it schedules certain algorithms to "fix" the AOD. If the
fixed AODs are written out, in-file metadata is updated indicating
which AODFix has been applied.

 - When running full reconstruction, the standard assumption is that
 the new AODs are fixed to the latest version of AODFix. Therefore,
 the in-file metadata is updated saying that the latest fix is
 applied. (See AODFix_r172.py in earlier tags, e.g., AODFix-00-03-01,
 for an example when full reconstruction brings the file to a
 different AODFix version.)

By default, AODFix schedules itself when reading AODs (not ESDs or
DAODs), only in actual releases (not nightlies), and if the AOD was
produced in a matching actual release (i.e., if the first two numbers,
like 20.1, match). When running full reconstruction, the metadata is
only updated when running in an actual release. This behavior can be
modified by locking rec.doApplyAODFix to True or False. (Note, it has
to be locked.) Locking it to False always prevents AODFix from
scheduling fixes or adding metadata. Provided the first two digits of
the release being run in can be deciphered (which is true in
20.1.X.Y-VAL style nightlies) locking rec.doApplyAODFix to true
schedules the fixes associtated with that release when reading AODs
and adds the metadata when writing AODs, either fixed or due to full
reconstruction.

Now the AOD to AOD part has been separated from the RAW to ESD part.
This package is the RAW to ESD part.

@section RecoFixOverview Module and Class Overview
  The RecoFix package contains of following modules:

  - python.RecoFix : The module that contains the interface to RecoFix. No other module should be imported externally. 
  - python.AODFix_base.AODFix_base : The AODFix base class
  - python.AODFix_r191.AODFix_r191 : The class implementing the release 19.1 AODFix
  - python.AODFix_r201.AODFix_r201 : The class implementing the release 20.1 AODFix


@author Rolf Seuster
@author Jovan Mitrevski

@section ExtrasAODFix Extra Pages
 - @ref UsedAODFix
 - @ref RequirementsAODFix
*/
/**
@page UsedAODFix Used Packages
@htmlinclude used_packages.html
*/
/**
@page RequirementsAODFix Requirements
@include requirements
*/
