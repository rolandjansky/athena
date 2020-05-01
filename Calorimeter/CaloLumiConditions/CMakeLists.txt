################################################################################
# Package: CaloLumiConditions
################################################################################

# Declare the package name:
atlas_subdir( CaloLumiConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
			  Control/AthenaKernel
                          DetectorDescription/Identifier
                          PRIVATE
                          DetectorDescription/IdDictParser
                          AtlasTest/TestTools
                          LumiBlock/LumiBlockData
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( Boost COMPONENTS timer )

# Component(s) in the package:
atlas_add_library( CaloLumiConditions
                   src/*.cxx
                   PUBLIC_HEADERS CaloLumiConditions
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
                   LINK_LIBRARIES CxxUtils Identifier AthenaKernel LArElecCalib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TestTools LArIdentifier LumiBlockData )

atlas_add_test( CaloBCIDCoeffs_test
                SOURCES
                test/CaloBCIDCoeffs_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils Identifier TestTools CaloLumiConditions IdDictParser LArIdentifier LArElecCalib )

atlas_add_test( CaloBCIDLumi_test
                SOURCES
                test/CaloBCIDLumi_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils Identifier TestTools CaloLumiConditions IdDictParser LumiBlockData LArIdentifier LArElecCalib
                PROPERTIES TIMEOUT 300 )


