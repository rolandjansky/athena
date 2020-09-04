################################################################################
# Package: AmdcStand
################################################################################

# Declare the package name:
atlas_subdir( AmdcStand )

set_source_files_properties( src/*.F90
                             PROPERTIES
                             LANGUAGE "Fortran"
                             HEADER_FILE_ONLY FALSE
                             COMPILE_FLAGS "" )

# Component(s) in the package:
atlas_add_library( AmdcStand
                   src/*.F90
                   src/*.cxx
                   PUBLIC_HEADERS AmdcStand
                   LINK_LIBRARIES AmdcCore )

