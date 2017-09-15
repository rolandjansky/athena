/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page AthenaPoolExampleConverter_page 

The package Database/AthenaPOOL/AthenaPoolExample contains running examples of algorithms writing and
reading Data Objects using AthenaPool. This sub package, AthenaPoolExampleConverter, defines
the persistent state representations for ExampleHitContainer and ExampleTrackContainer, their T-P converter
and their GAUDI converter.

Persistent state representations: ExampleHitContainer_p1, ExampleHit_p1, ExampleTrackContainer_p1 and
ExampleTrack_p1 are the persistent state representations for the transient classes defined in the
AthenaPoolExampleData package. They contain corresponding data member but do not need many of the
classes methods and inheritance. Pointers (especially in the top-level DataVector) can be replaced
with data member by value, allowing better ROOT browse-ability. Each time the transient class changes
beyond what ROOT can handle automatically a new persistent class (_p2, _p3...) is introduced and the older
definitions are kept to allow the reading of old data.

T-P converter: The conversion between the transient and persistent class objects are done using ExampleHitContainerCnv_p1,
ExampleHitCnv_p1, ExampleTrackContainerCnv_p1 and ExampleTrackCnv_p1. Once a transient class changes so
that a new persistent version has to be introduced, a new T-P converter is required and the existing ones
may have to be changed to create the new transient class object.

GAUDI converter: the GAUDI converter for the top-level classes are called when reading / writing the objects.
While reading they use the POOL class ID to dispatch the correct T-P converter. For writing the latest T-P
converter is used.


@section AthenaPoolExampleConverter_Documentation Additional Documentation

See the ATLAS User/Developer HowTo document for additional information.

The code can be browsed using LXR
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleConverter/)








@author Peter van Gemmeren <gemmeren@anl.gov>
*/
