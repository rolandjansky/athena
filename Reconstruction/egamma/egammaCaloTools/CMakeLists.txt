################################################################################
# Package: egammaCaloTools
################################################################################

# Declare the package name:
atlas_subdir( egammaCaloTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/SGTools
                          Event/FourMomUtils
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          Reconstruction/egamma/egammaInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( egammaCaloTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier CaloUtilsLib AthenaBaseComps SGTools FourMomUtils xAODCaloEvent GaudiKernel )

# Install files from the package:
atlas_install_headers( egammaCaloTools )
atlas_install_python_modules( python/*.py )

