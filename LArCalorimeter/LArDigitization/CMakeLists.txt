################################################################################
# Package: LArDigitization
################################################################################

# Declare the package name:
atlas_subdir( LArDigitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArDigitizationLib
                   src/*.cxx
                   PUBLIC_HEADERS LArDigitization
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps
                   AthenaKernel Identifier xAODEventInfo GaudiKernel
                   LArIdentifier LArRawEvent LArSimEvent
                   LArRecConditions LArRawConditions CaloDetDescrLib
                   PileUpToolsLib StoreGateLib SGtests LArCablingLib EventInfoUtils
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GeneratorObjects )

atlas_add_component( LArDigitization
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES LArDigitizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
