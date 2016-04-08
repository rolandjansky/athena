################################################################################
# Package: LArEventTest
################################################################################

# Declare the package name:
atlas_subdir( LArEventTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArTools
                          TestBeam/TBEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArElecCalib )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( LArEventTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArToolsLib TBEvent CaloEvent CxxUtils xAODEventInfo )

# Install files from the package:
atlas_install_headers( LArEventTest )
atlas_install_joboptions( share/*.py )

