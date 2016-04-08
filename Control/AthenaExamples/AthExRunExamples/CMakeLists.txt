################################################################################
# Package: AthExRunExamples
################################################################################

# Declare the package name:
atlas_subdir( AthExRunExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaExamples/AthExFortranAlgorithm
                          Control/AthenaExamples/AthExHelloWorld
                          Control/AthenaExamples/AthExHistNtup
                          Control/AthenaExamples/AthExStoreGateExample
                          Control/AthenaExamples/ToyConversion
                          Control/MinimalRunTime )

