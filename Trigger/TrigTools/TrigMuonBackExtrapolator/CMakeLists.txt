################################################################################
# Package: TrigMuonBackExtrapolator
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonBackExtrapolator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTrigMuon
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent )

# Component(s) in the package:
atlas_add_library( TrigMuonBackExtrapolatorLib
                   TrigMuonBackExtrapolator/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigMuonBackExtrapolator
                   LINK_LIBRARIES GaudiKernel TrigInDetEvent TrigMuonEvent xAODTrigMuon )

atlas_add_component( TrigMuonBackExtrapolator
                     src/*.cxx src/*.h src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrigMuonBackExtrapolatorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
