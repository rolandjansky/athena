################################################################################
# Package: TrigT1RPChardware
################################################################################

# Declare the package name:
atlas_subdir( TrigT1RPChardware )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          MuonSpectrometer/MuonCablings/MuonCablingTools
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( RPChardware
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPChardware
                   LINK_LIBRARIES MuonCablingTools CablingTools
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_library( TrigT1RPChardwareLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPChardware
                   LINK_LIBRARIES MuonCablingTools CablingTools RPChardware
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_component( TrigT1RPChardware
                     src/components/*.cxx
                     LINK_LIBRARIES MuonCablingTools CablingTools GaudiKernel RPChardware TrigT1RPChardwareLib )

