################################################################################
# Package: TrigT1RPClogic
################################################################################

# Declare the package name:
atlas_subdir( TrigT1RPClogic )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/MuonCablingTools
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          Trigger/TrigT1/TrigT1RPChardware )

# Component(s) in the package:
atlas_add_library( TrigT1RPClogicLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPClogic
                   LINK_LIBRARIES GaudiKernel MuonCablingTools CablingTools RPCcablingInterfaceLib TrigT1RPChardwareLib RPChardware )

atlas_add_component( TrigT1RPClogic
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonCablingTools CablingTools RPCcablingInterfaceLib TrigT1RPChardwareLib RPChardware TrigT1RPClogicLib )

