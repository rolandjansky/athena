################################################################################
# Package: AthViews
################################################################################

# Declare the package name:
atlas_subdir( AthViews )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_library( AthViews
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS AthViews
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel CxxUtils EventInfo )

atlas_add_component( AthViewsDFlow
                     src_dflow/*.cxx
                     src_dflow/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel CxxUtils EventInfo AthViews )

# Install files from the package:
atlas_install_headers( AthViews )
atlas_install_joboptions( share/*.py )

