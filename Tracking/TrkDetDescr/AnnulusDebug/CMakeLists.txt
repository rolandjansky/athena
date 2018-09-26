################################################################################
# Package: AnnulusDebug
################################################################################

# Declare the package name:
atlas_subdir( AnnulusDebug )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( AnnulusDebug
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DataModel GaudiKernel TrkTrack StoreGateLib SGtests TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( AnnulusDebug )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

