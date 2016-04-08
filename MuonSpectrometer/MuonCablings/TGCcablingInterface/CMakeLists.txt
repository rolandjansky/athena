################################################################################
# Package: TGCcablingInterface
################################################################################

# Declare the package name:
atlas_subdir( TGCcablingInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( TGCcablingInterfaceLib
                   src/*.cxx
                   PUBLIC_HEADERS TGCcablingInterface
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel )

atlas_add_component( TGCcablingInterface
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel TGCcablingInterfaceLib )

# Install files from the package:
atlas_install_python_modules( python/__init__.py )

