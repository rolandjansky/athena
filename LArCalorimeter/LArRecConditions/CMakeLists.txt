################################################################################
# Package: LArRecConditions
################################################################################

# Declare the package name:
atlas_subdir( LArRecConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
			  LArCalorimeter/LArIdentifier
                          Control/AthenaKernel
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
			  LArCalorimeter/LArElecCalib
			  LArCalorimeter/LArCabling )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArRecConditions
                   src/*.cxx
                   PUBLIC_HEADERS LArRecConditions
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier LArIdentifier AthenaKernel Identifier GaudiKernel AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( LArRecConditionsDict
                      LArRecConditions/LArRecConditionsDict.h
                      LArRecConditions/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel Identifier GaudiKernel LArRecConditions)

atlas_add_test (test_LArBadChannelCont
	        SOURCES
	        test/test_LArBadChannelCont.cxx
                LINK_LIBRARIES Identifier CxxUtils StoreGateLib LArRecConditions 
               )
