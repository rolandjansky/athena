################################################################################
# Package: AthExMonitored
################################################################################

# Declare the package name:
atlas_subdir( AthExMonitored )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoringKernel )

# Component(s) in the package:
atlas_add_component( AthExMonitored
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaMonitoringKernelLib)

# Install files from the package:
atlas_install_joboptions( share/*.py )

