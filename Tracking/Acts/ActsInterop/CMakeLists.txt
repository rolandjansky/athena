
# Declare the package name:
atlas_subdir( ActsInterop )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Control/AtenaBaseComps
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:

find_package(Acts COMPONENTS Core)

# Component(s) in the package:
atlas_add_library( ActsInteropLib
                   src/*.cxx
                   PUBLIC_HEADERS ActsInterop
                   LINK_LIBRARIES 
                   AthenaBaseComps
                   Identifier
                   InDetIdentifier
                   InDetReadoutGeometry
                   ActsCore)

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

