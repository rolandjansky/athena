# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetHitAssociation )

# External dependencies:
find_package( ROOT COMPONENTS Core TVector3 )

atlas_add_library( JetHitAssociationLib
                   src/*.h
                   INTERFACE
                   PUBLIC_HEADERS JetHitAssociation
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps TrkValInterfaces xAODEventInfo AthContainers xAODJet xAODTracking  InDetEventTPCnv )

# Component(s) in the package:
atlas_add_component( JetHitAssociation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventInfo EventPrimitives GaudiKernel JetHitAssociationLib )

atlas_install_joboptions( share/*.py )
