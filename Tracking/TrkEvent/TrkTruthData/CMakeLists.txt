# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthData )

# Component(s) in the package:
atlas_add_library( TrkTruthData
                   PUBLIC_HEADERS TrkTruthData
                   LINK_LIBRARIES AthLinks AthContainers AthenaKernel GeoPrimitives Identifier GeneratorObjects TrkTrack )

atlas_add_dictionary( TrkTruthDataDict
                      src/TrkTruthDataDict.h
                      TrkTruthData/selection.xml
                      LINK_LIBRARIES AthLinks GeneratorObjects TrkTrack TrkTruthData )
