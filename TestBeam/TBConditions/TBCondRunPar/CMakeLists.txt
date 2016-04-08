################################################################################
# Package: TBCondRunPar
################################################################################

# Declare the package name:
atlas_subdir( TBCondRunPar )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_library( TBCondRunParLib
                   src/TBCondRunParTool.cxx
                   PUBLIC_HEADERS TBCondRunPar
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities EventInfo )

atlas_add_component( TBCondRunPar
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel AthenaPoolUtilities EventInfo TBCondRunParLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

