################################################################################
# Package: TrigT1TGCRecRoiSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigT1TGCRecRoiSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_library( TrigT1TGCRecRoiSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1TGCRecRoiSvc
                   LINK_LIBRARIES MuonReadoutGeometry TrigT1Interfaces TGCcablingInterfaceLib StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_component( TrigT1TGCRecRoiSvc
                     src/components/*.cxx
                     LINK_LIBRARIES TGCcablingInterfaceLib MuonReadoutGeometry TrigT1Interfaces StoreGateLib SGtests Identifier GaudiKernel MuonIdHelpersLib TrigT1TGCRecRoiSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

