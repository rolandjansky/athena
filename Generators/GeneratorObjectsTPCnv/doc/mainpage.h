/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage GeneratorObjectsTPCnv Package

This package defines the so-called T/P converters for Monte-Carlo classes.

@authors Sebastien Binet, Georgios Stavropoulos, Davide Costanzo

@section GeneratorObjectsTPCnvIntro Introduction

The ATLAS software, in order to provide backward compatibility (and extensive schema evolution) of on-disk stored data, decouples the transient classes that one manipulates in-memory and the persistent classes that one writes on disk.
To achieve this, a robust (and quite performant) Transient/Persistent separation library has been written.
This library has the responsibility for converting all (or a subset of) previous versions of persistent classes into the latest and finest in-memory representation of a given class.
The GeneratorObjectsTPCnv package provides that functionality for the MonteCarlo objects (e.g. @c HepMC::GenEvent).

As other transient (and hence persistent) classes may also need to re-use that functionality, the core of the conversion is provided as an installed library (so symbols are exported) so other component and/or installed libraries can link against (to prevent code duplication).

See https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation for a broader and more detailed of how the T/P separation of classes is working and implemented.

@htmlinclude used_packages.html

@include requirements

*/
