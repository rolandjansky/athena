################################################################################
# Package: EventKernel
################################################################################

# Declare the package name:
atlas_subdir( EventKernel )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_dep Tracking/TrkEvent/VxVertex )
   set( extra_lib VxVertex )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/Navigation
                          Control/AthenaKernel
                          ${extra_dep} )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventKernel
                   src/*.cxx
                   PUBLIC_HEADERS EventKernel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthLinks Navigation AthenaKernel ${extra_lib}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( EventKernelDict
                      EventKernel/EventKernelDict.h
                      EventKernel/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks Navigation AthenaKernel ${extra_lib} EventKernel )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/ParticleDataType.py )

