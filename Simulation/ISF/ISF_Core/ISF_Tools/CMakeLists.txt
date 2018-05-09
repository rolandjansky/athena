################################################################################
# Package: ISF_Tools
################################################################################

# Declare the package name:
atlas_subdir( ISF_Tools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Simulation/Barcode/BarcodeEvent
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/G4Sim/TrackRecord
                          Generators/GeneratorObjects
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces )

# Component(s) in the package:
atlas_add_component( ISF_Tools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AtlasDetDescr GeneratorObjects ISF_Event ISF_Interfaces )

# Install files from the package:
atlas_install_headers( ISF_Tools )
atlas_install_python_modules( python/*.py )

