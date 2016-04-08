################################################################################
# Package: MuonRPC_Cabling
################################################################################

# Declare the package name:
atlas_subdir( MuonRPC_Cabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/MuonCablingTools
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_component( MuonRPC_Cabling
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier GaudiKernel MuonCablingTools CablingTools RPCcablingInterfaceLib MuonIdHelpersLib StoreGateLib SGtests AthenaPoolUtilities MuonCondInterface PathResolver )

# Install files from the package:
atlas_install_headers( MuonRPC_Cabling )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/LVL1confAtlas.data share/LVL1confAtlas.corr share/ATLAS.data share/LVL1confAtlasRUN2_ver016.corr share/LVL1confAtlasRUN2_ver016.data )

