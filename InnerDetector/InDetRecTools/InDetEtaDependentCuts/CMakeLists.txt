################################################################################
# Package: InDetEtaDependentCuts
################################################################################

# Declare the package name:
atlas_subdir( InDetEtaDependentCuts )

# Declare the package's dependencies:
atlas_depends_on_subdirs(  )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost COMPONENTS unit_test_framework )

# tag rootMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( InDetEtaDependentCuts
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel InDetRecToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetEtaDependentCuts )
atlas_install_joboptions( share/*.txt )

atlas_add_test( InDetEtaDependentCutsSvc_test                
   SOURCES test/InDetEtaDependentCutsSvc_test.cxx 
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel AthenaBaseComps GaudiKernel TestTools CxxUtils InDetRecToolInterfaces
   POST_EXEC_SCRIPT "nopost.sh" )

