################################################################################
# Package: CppUnitSGServiceExample
################################################################################

# Declare the package name:
atlas_subdir( CppUnitSGServiceExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/DataModel
                          Control/StoreGate
                          GaudiKernel
                          TestPolicy )

# Install files from the package:
atlas_install_headers( CppUnitSGServiceExample )

