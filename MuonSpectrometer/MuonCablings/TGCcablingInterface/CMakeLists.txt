################################################################################
# Package: TGCcablingInterface
################################################################################

# Declare the package name:
atlas_subdir( TGCcablingInterface )

# Component(s) in the package:
atlas_add_library( TGCcablingInterfaceLib
                   src/*.cxx
                   PUBLIC_HEADERS TGCcablingInterface
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel )

