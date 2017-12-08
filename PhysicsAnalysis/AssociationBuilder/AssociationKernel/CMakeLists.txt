################################################################################
# Package: AssociationKernel
################################################################################

# Declare the package name:
atlas_subdir( AssociationKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthLinks
                          Event/EventKernel
                          Event/NavFourMom
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( AssociationKernel
                   src/*.cxx
                   PUBLIC_HEADERS AssociationKernel
                   LINK_LIBRARIES AthenaBaseComps AthLinks EventKernel NavFourMom GaudiKernel )

