################################################################################
# Package: TRT_PAI_Process
################################################################################

# Declare the package name:
atlas_subdir( TRT_PAI_Process )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TRT_PAI_Process
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( TRT_PAI_Process )
atlas_install_joboptions( share/TRT_PAI_Process.py )

