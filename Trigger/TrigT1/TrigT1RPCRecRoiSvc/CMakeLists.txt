################################################################################
# Package: TrigT1RPCRecRoiSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigT1RPCRecRoiSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface )

# Component(s) in the package:
atlas_add_library( TrigT1RPCRecRoiSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPCRecRoiSvc
                   LINK_LIBRARIES MuonReadoutGeometry TrigT1Interfaces StoreGateLib SGtests RPCcablingInterfaceLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_component( TrigT1RPCRecRoiSvc
                     src/components/*.cxx
                     LINK_LIBRARIES MuonReadoutGeometry TrigT1Interfaces StoreGateLib SGtests Identifier GaudiKernel RPCcablingInterfaceLib TrigT1RPCRecRoiSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

