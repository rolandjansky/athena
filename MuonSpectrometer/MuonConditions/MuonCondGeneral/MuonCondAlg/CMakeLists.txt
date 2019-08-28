################################################################################
# Package: MuonCondAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonCondAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondTool
                          PRIVATE
                          Event/EventInfo
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCondAlgLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCondAlg
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier GaudiKernel MuonCondData MuonCondInterface MuonCondSvcLib MuonCondTool StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventInfo )

atlas_add_component( MuonCondAlg
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonCondAlgLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

