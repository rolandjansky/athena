################################################################################
# Package: LArDigitization
################################################################################

# Declare the package name:
atlas_subdir( LArDigitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/PileUpTools
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArSimEvent
                          LArCalorimeter/LArRecConditions
                          PRIVATE
                          Generators/GeneratorObjects )


# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( LArDigitizationLib
                   src/*.cxx
                   PUBLIC_HEADERS LArDigitization
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps AthenaKernel SGTools Identifier xAODEventInfo GaudiKernel LArIdentifier LArRawEvent LArSimEvent LArRecConditions CaloDetDescrLib PileUpToolsLib StoreGateLib SGtests LArCablingLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorObjects )

atlas_add_component( LArDigitization
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES LArDigitizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

