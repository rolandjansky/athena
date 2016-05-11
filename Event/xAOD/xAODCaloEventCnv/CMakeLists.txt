################################################################################
# Package: xAODCaloEventCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloEventCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCaloEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventKernel
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( xAODCaloEventCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCaloEvent CaloEvent AthenaBaseComps AthenaKernel EventKernel GaudiKernel )

# Install files from the package:
atlas_install_headers( xAODCaloEventCnv )
atlas_install_joboptions( share/*.py )

