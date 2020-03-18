################################################################################
# Package: CaloConditions
################################################################################

# Declare the package name:
atlas_subdir( CaloConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
			  Control/AthenaKernel
                          DetectorDescription/Identifier
			  Calorimeter/CaloCondBlobObjs
			  Calorimeter/CaloIdentifier
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/IdDictParser
                          AtlasTest/TestTools
                          LumiBlock/LumiBlockData
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system timer )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CaloConditions
                   src/*.cxx
                   PUBLIC_HEADERS CaloConditions
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils Identifier CaloIdentifier CaloCondBlobObjs AthenaKernel GaudiKernel 
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TestTools LArIdentifier LumiBlockData )

atlas_add_dictionary( CaloConditionsDict
                      CaloConditions/CaloConditionsDict.h
                      CaloConditions/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils Identifier GaudiKernel TestTools CaloConditions )

atlas_add_test( ToolConstants_test
                SOURCES
                test/ToolConstants_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils Identifier GaudiKernel TestTools CaloConditions )

atlas_add_test( CaloBCIDCoeffs_test
                SOURCES
                test/CaloBCIDCoeffs_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils Identifier GaudiKernel TestTools CaloConditions IdDictParser )

atlas_add_test( CaloBCIDLumi_test
                SOURCES
                test/CaloBCIDLumi_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils Identifier GaudiKernel TestTools CaloConditions IdDictParser LumiBlockData
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

