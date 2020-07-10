# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetTruthEvent )

# Component(s) in the package:
atlas_add_library( TrigInDetTruthEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetTruthEvent
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel AtlasHepMCLib GeneratorObjects TrigInDetEvent )

atlas_add_dictionary( TrigInDetTruthEventDict
                      TrigInDetTruthEvent/TrigInDetTruthEventDict.h
                      TrigInDetTruthEvent/selection.xml
                      LINK_LIBRARIES TrigInDetTruthEvent )
