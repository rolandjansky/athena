################################################################################
# Package: AthExRunHelloWorld
################################################################################

# Declare the package name:
atlas_subdir( AthExRunHelloWorld )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaExamples/AthExHelloWorld
                          Control/MinimalRunTime )

