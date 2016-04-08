################################################################################
# Package: LArRegionSelector
################################################################################

# Declare the package name:
atlas_subdir( LArRegionSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          DetectorDescription/RegionSelector
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloTTDetDescr
                          Calorimeter/CaloTriggerTool
                          Control/StoreGate
                          Event/ByteStreamData
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArTools )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( LArRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps Identifier RegionSelectorLib GaudiKernel CaloDetDescrLib CaloTTDetDescr CaloTriggerToolLib StoreGateLib SGtests ByteStreamData ByteStreamData_test LArIdentifier LArRawUtilsLib LArToolsLib )

# Install files from the package:
atlas_install_headers( LArRegionSelector )
atlas_install_joboptions( share/*.py )

