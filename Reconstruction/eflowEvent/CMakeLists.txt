# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( eflowEvent )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( eflowEvent
                   src/*.cxx
                   PUBLIC_HEADERS eflowEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} CaloEvent AthContainers AthLinks Navigation AthenaKernel EventKernel FourMom NavFourMom xAODCaloEvent xAODPFlow xAODTracking GaudiKernel muonEvent Particle VxVertex )
atlas_generate_cliddb( eflowEvent )

atlas_add_dictionary( eflowEventDict
                      eflowEvent/eflowEventDict.h
                      eflowEvent/selection.xml
                      LINK_LIBRARIES eflowEvent
                      NAVIGABLES eflowObjectContainer )

