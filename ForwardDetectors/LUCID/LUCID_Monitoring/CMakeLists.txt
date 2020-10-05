################################################################################
# Package: LUCID_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( LUCID_Monitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LUCID_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib LUCID_RawEvent GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )

