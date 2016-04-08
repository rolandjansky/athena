################################################################################
# Package: CaloInterface
################################################################################

# Declare the package name:
atlas_subdir( CaloInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( CaloInterfaceDict
                      CaloInterface/CaloInterfaceDict.h
                      CaloInterface/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloIdentifier AthenaKernel Identifier xAODCaloEvent GaudiKernel )

# Install files from the package:
atlas_install_headers( CaloInterface )

