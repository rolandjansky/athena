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
atlas_add_component( TrigMuonRoITools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ByteStreamCnvSvcBaseLib EventInfo TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigMuonRoITools )

