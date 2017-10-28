################################################################################
# Package: TRT_ConditionsData
################################################################################

# Declare the package name:
atlas_subdir( TRT_ConditionsData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TRT_ConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS TRT_ConditionsData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions Identifier GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TRT_ConditionsDataDict
                      TRT_ConditionsData/TRT_ConditionsDataDict.h
                      TRT_ConditionsData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities DetDescrConditions Identifier GaudiKernel TRT_ConditionsData )

