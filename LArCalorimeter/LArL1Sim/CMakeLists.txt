################################################################################
# Package: LArL1Sim
################################################################################

# Declare the package name:
atlas_subdir( LArL1Sim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthAllocators
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArDigitization
                          LArCalorimeter/LArElecCalib
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArSimEvent
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( LArL1Sim
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthAllocators AthenaBaseComps GaudiKernel LArDigitizationLib CaloDetDescrLib CaloEvent CaloIdentifier CaloTriggerToolLib AthenaKernel PileUpToolsLib StoreGateLib SGtests EventInfo LArCablingLib LArIdentifier LArRawEvent LArSimEvent AtlasCLHEP_RandomGenerators PathResolver )

# Install files from the package:
atlas_install_headers( LArL1Sim )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/Fcal_ptweights_table7.data )

