# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventKernel )

# Possible extra dependencies:
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_lib VxVertex )
endif()

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( EventKernel
                   src/*.cxx
                   PUBLIC_HEADERS EventKernel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthLinks CxxUtils Navigation AthenaKernel ${extra_lib} )

atlas_add_dictionary( EventKernelDict
                      EventKernel/EventKernelDict.h
                      EventKernel/selection.xml
                      LINK_LIBRARIES EventKernel )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/ParticleDataType.py )
