################################################################################
# Package: TBUtils
################################################################################

# Declare the package name:
atlas_subdir( TBUtils )

atlas_add_library( TBUtilsLib
                   TBUtils/*.h
                   INTERFACE
                   PUBLIC_HEADERS TBUtils )

