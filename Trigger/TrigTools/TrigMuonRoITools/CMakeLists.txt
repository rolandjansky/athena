################################################################################
# Package: TrigMuonRoITools
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonRoITools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_library( TrigMuonRoIToolsLib
                   TrigMuonRoITools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigMuonRoITools
                   LINK_LIBRARIES GaudiKernel )

atlas_add_component( TrigMuonRoITools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps ByteStreamCnvSvcBaseLib GaudiKernel TrigT1Result TrigMuonRoIToolsLib )
