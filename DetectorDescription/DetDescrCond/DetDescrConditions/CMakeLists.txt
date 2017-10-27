################################################################################
# Package: DetDescrConditions
################################################################################

# Declare the package name:
atlas_subdir( DetDescrConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
			  Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DetDescrConditions
                   src/AlignableTransform.cxx
                   src/DetCondFloat.cxx
                   src/DetCondCFloat.cxx
                   src/DetCondKeyTrans.cxx
                   src/DetStatus.cxx
                   src/DetStatusMap.cxx
                   PUBLIC_HEADERS DetDescrConditions
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolUtilities Identifier GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( DetDescrConditionsDict
                      DetDescrConditions/DetDescrConditionsDict.h
                      DetDescrConditions/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolUtilities Identifier GaudiKernel DetDescrConditions )

