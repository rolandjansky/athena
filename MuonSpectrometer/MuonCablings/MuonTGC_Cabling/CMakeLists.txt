################################################################################
# Package: MuonTGC_Cabling
################################################################################

# Declare the package name:
atlas_subdir( MuonTGC_Cabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          MuonSpectrometer/MuonIdHelpers
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( MuonTGC_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonTGC_Cabling
                   LINK_LIBRARIES AthenaKernel GaudiKernel MuonCondInterface TGCcablingInterfaceLib StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities PathResolver )

atlas_add_component( MuonTGC_Cabling
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel TGCcablingInterfaceLib MuonCondInterface StoreGateLib SGtests AthenaPoolUtilities MuonIdHelpersLib PathResolver MuonTGC_CablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.db )

