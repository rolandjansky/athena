################################################################################
# Package: ISF_Geant4CommonTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4CommonTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Simulation/G4Sim/TrackRecord
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces )

# Component(s) in the package:
atlas_add_component( ISF_Geant4CommonTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ISF_Event ISF_Interfaces )

# Install files from the package:
atlas_install_headers( ISF_Geant4CommonTools )
atlas_install_python_modules( python/*.py )

