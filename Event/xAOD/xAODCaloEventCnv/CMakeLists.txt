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
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )


# Component(s) in the package:
atlas_add_library( xAODCaloEventCnvLib
                   xAODCaloEventCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODCaloEventCnv
                   LINK_LIBRARIES GaudiKernel xAODCaloEvent )

# Component(s) in the package:
atlas_add_component( xAODCaloEventCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCaloEventCnvLib xAODCaloEvent CaloEvent AthenaBaseComps AthenaKernel EventKernel GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )

