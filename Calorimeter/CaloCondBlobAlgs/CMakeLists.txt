################################################################################
# Package: CaloCondBlobAlgs
################################################################################

# Declare the package name:
atlas_subdir( CaloCondBlobAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloCondBlobObjs
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( CaloCondBlobAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} CaloCondBlobObjs CaloDetDescrLib CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier xAODEventInfo GaudiKernel )

# Install files from the package:
atlas_install_headers( CaloCondBlobAlgs )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
