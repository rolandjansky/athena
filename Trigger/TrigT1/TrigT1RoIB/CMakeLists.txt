################################################################################
# Package: TrigT1RoIB
################################################################################

# Declare the package name:
atlas_subdir( TrigT1RoIB )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Event/EventInfo
                          GaudiKernel
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigT1RoIB
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel DataModel EventInfo GaudiKernel TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/*.py )

