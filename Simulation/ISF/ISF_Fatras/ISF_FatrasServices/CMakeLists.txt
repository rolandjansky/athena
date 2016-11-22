################################################################################
# Package: ISF_FatrasServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/StoreGate
                          Simulation/ISF/ISF_Core/ISF_Event )

# Component(s) in the package:
atlas_add_component( ISF_FatrasServices
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr GaudiKernel ISF_Interfaces TrkTrack StoreGateLib SGtests ISF_Event )

# Install files from the package:
atlas_install_headers( ISF_FatrasServices )
atlas_install_python_modules( python/*.py )

