/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage


@section TrigBSExtractionIntro Introduction 

This package contains only one top algorithms. It should be run during
reconstruction phase and is extracting all objects stored in
ByteStream (RAW) event file and puts them to SG. From SG they are
taken accordingly to the output streams configuration.

The main disadvantage of this package is that it depemds on all Trigger EDM objects. (Note list of dependencies.)



@htmlinclude used_packages.html

@include requirements

*/
