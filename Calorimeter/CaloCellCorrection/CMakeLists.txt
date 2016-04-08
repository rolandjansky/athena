################################################################################
# Package: CaloCellCorrection
################################################################################

# Declare the package name:
atlas_subdir( CaloCellCorrection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloCondBlobObjs
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( CaloCellCorrection
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloCondBlobObjs CaloIdentifier CaloUtilsLib AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities GaudiKernel CaloDetDescrLib CaloEvent Identifier )

# Install files from the package:
atlas_install_headers( CaloCellCorrection )
atlas_install_python_modules( python/*.py )

