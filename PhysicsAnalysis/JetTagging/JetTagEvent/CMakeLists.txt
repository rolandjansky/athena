# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagEvent )

# Component(s) in the package:
atlas_add_library( JetTagEvent
                   src/*.cxx
                   PUBLIC_HEADERS JetTagEvent
                   LINK_LIBRARIES CaloGeoHelpers Navigation ParticleEvent JetEvent Particle
                   PRIVATE_LINK_LIBRARIES EventKernel NavFourMom VxSecVertex )

atlas_add_dictionary( JetTagEventDict
                      JetTagEvent/JetTagEventDict.h
                      JetTagEvent/selection.xml
                      LINK_LIBRARIES JetTagEvent )
