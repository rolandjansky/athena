# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTruthTools )

# Component(s) in the package:
atlas_add_component( InDetTruthTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps TrkEventPrimitives AtlasDetDescr InDetIdentifier ReadoutGeometryBase InDetReadoutGeometry InDetSimData InDetPrepRawData TrkPrepRawData TrkTruthData InDetTruthInterfaces TrkTruthTrackInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )
