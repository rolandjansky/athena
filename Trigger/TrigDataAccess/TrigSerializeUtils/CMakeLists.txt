################################################################################
# Package: TrigSerializeUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigSerializeUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

