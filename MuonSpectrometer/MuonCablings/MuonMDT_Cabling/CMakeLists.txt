################################################################################
# Package: MuonMDT_Cabling
################################################################################

# Declare the package name:
atlas_subdir( MuonMDT_Cabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/EventInfoMgt
                          MuonSpectrometer/MuonCablings/MuonCablingData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface 
			  MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
			  MuonSpectrometer/MuonConditions/MuonCondCabling/MDT_CondCabling
			  Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( MuonMDT_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonMDT_Cabling
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities Identifier EventInfo MuonCablingData MuonCondInterface MuonCondSvcLib )

atlas_add_component( MuonMDT_Cabling
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonMDT_CablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

