################################################################################
# Package: ALFA_CLinkAlg
################################################################################

# Declare the package name:
atlas_subdir( ALFA_CLinkAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Control/StoreGate
                          Event/xAOD/xAODForward
                          ForwardDetectors/ALFA/ALFA_CLinkEv
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_component( ALFA_CLinkAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel DataModel StoreGateLib SGtests xAODForward ALFA_CLinkEv GaudiKernel EventInfo )

# Install files from the package:
atlas_install_headers( ALFA_CLinkAlg )
atlas_install_joboptions( share/*.py )

