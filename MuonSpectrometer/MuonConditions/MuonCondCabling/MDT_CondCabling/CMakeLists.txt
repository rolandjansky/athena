################################################################################
# Package: MDT_CondCabling
################################################################################

# Declare the package name:
atlas_subdir( MDT_CondCabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/MuonCablingData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
                          MuonSpectrometer/MuonIdHelpers
                          Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( MDT_CondCabling
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel MuonCablingData MuonCondInterface SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier MuonCondSvcLib MuonIdHelpersLib PathResolver )

# Install files from the package:
atlas_install_headers( MDT_CondCabling )

