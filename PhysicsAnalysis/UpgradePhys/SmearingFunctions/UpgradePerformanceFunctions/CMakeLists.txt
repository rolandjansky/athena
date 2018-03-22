################################################################################
# Package: UpgradePerformanceFunctions
################################################################################

# Declare the name of this package:
atlas_subdir( UpgradePerformanceFunctions )

# Extra dependencies, based on the build environment:
set( extra_deps )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess PhysicsAnalysis/D3PDTools/EventLoop
      PRIVATE )
   set( extra_libs xAODRootAccess EventLoop )
else()
   set( extra_deps Control/AthenaBaseComps Control/AthAnalysisBaseComps
      PRIVATE PhysicsAnalysis/POOLRootAccess GaudiKernel )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PathResolver
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
                          PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface
                          ${extra_deps} )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )
find_package( Eigen )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( UpgradePerformanceFunctions _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( UpgradePerformanceFunctions Root/*.cxx Root/*.h Root/*.icc
                   UpgradePerformanceFunctions/*.h UpgradePerformanceFunctions/*.icc UpgradePerformanceFunctions/*/*.h
                   UpgradePerformanceFunctions/*/*.icc ${_cintDictSource}
                   PUBLIC_HEADERS UpgradePerformanceFunctions
                   LINK_LIBRARIES PathResolver MuonEfficiencyCorrectionsLib MuonMomentumCorrectionsLib
                                  ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES}  ${extra_libs}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( plotPerformanceFunctions
                      util/plotPerformanceFunctions.cxx
                      LINK_LIBRARIES PathResolver
                                     ${ROOT_LIBRARIES}
                                     UpgradePerformanceFunctions )
atlas_add_executable( testPerformanceFunctions
                      util/testPerformanceFunctions.cxx
                      LINK_LIBRARIES PathResolver
                                     ${ROOT_LIBRARIES}
                                     UpgradePerformanceFunctions )

# Tests in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_UpgradePerformanceFunctionsTester_Electrons
      SOURCES test/ut_UpgradePerformanceFunctionsTester_Electrons.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} UpgradePerformanceFunctions AsgTools)
   atlas_add_test( ut_UpgradePerformanceFunctionsTester_Muons
      SOURCES test/ut_UpgradePerformanceFunctionsTester_Muons.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} UpgradePerformanceFunctions AsgTools)
   atlas_add_test( ut_UpgradePerformanceFunctionsTester_Jets
      SOURCES test/ut_UpgradePerformanceFunctionsTester_Jets.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} UpgradePerformanceFunctions AsgTools)
   atlas_add_test( ut_UpgradePerformanceFunctionsTester_MET
      SOURCES test/ut_UpgradePerformanceFunctionsTester_MET.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} UpgradePerformanceFunctions AsgTools)
   atlas_add_test( ut_UpgradePerformanceFunctionsTester_Photons
      SOURCES test/ut_UpgradePerformanceFunctionsTester_Photons.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} UpgradePerformanceFunctions AsgTools)
endif()
