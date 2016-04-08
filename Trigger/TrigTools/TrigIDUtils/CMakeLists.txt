################################################################################
# Package: TrigIDUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigIDUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# Component(s) in the package:
atlas_add_component( TrigIDUtils
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib SGtests InDetIdentifier )

