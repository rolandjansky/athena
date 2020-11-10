# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VxSecVertex )

# Component(s) in the package:
atlas_add_library( VxSecVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxSecVertex
		           LINK_LIBRARIES AthLinks AthContainers AthenaKernel CxxUtils TrkParticleBase xAODBase xAODTracking
                   PRIVATE_LINK_LIBRARIES VxJetVertex VxVertex )

atlas_add_dictionary( VxSecVertexDict
                      VxSecVertex/VxSecVertexDict.h
                      VxSecVertex/selection.xml
		              LINK_LIBRARIES VxSecVertex xAODTracking )
