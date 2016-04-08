################################################################################
# Package: ISF_ParSimServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          PRIVATE
                          Simulation/ISF/ISF_Core/ISF_Event )

# Component(s) in the package:
atlas_add_component( ISF_ParSimServices
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel ISF_Interfaces TrkDetDescrInterfaces ISF_Event )

# Install files from the package:
atlas_install_headers( ISF_ParSimServices )
atlas_install_python_modules( python/*.py )

