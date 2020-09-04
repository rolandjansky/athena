################################################################################
# Package: TGC_CondCabling
################################################################################

# Declare the package name:
atlas_subdir( TGC_CondCabling )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel)

# Component(s) in the package:
atlas_add_component( TGC_CondCabling
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} AthenaBaseComps GaudiKernel MuonCondInterface AthenaPoolUtilities PathResolver)

# Install files from the package:
atlas_install_headers( TGC_CondCabling )
atlas_install_runtime( share/*.db )
