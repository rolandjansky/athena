################################################################################
# Package: MuonCondData
################################################################################

# Declare the package name:
atlas_subdir( MuonCondData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo
                          MuonSpectrometer/MuonIdHelpers )


# Component(s) in the package:
atlas_add_library( MuonCondData
                   src/*.cxx
                   PUBLIC_HEADERS MuonCondData
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthContainers AthenaPoolUtilities Identifier GaudiKernel StoreGateLib SGtests MuonIdHelpersLib 
                   PRIVATE_LINK_LIBRARIES EventInfo )

