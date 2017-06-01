################################################################################
# Package: TrigUpgradeTest
################################################################################

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy
			  AtlasPolicy	
			  GaudiKernel
			  PRIVATE
			  Control/AthenaBaseComps
			  Trigger/TrigEvent/TrigSteeringEvent
                          )

find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( TrigUpgradeTestLib
                   src/*.cxx
		   PUBLIC_HEADERS TrigUpgradeTest
		   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
		   GaudiKernel AthenaBaseComps TrigSteeringEvent DecisionHandlingLib )
										                    
atlas_add_component( TrigUpgradeTest
                     src/components/*.cxx
		     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
		     LINK_LIBRARIES ${Boost_LIBRARIES}
		     ${ROOT_LIBRARIES} GaudiKernel
		     AthenaBaseComps TrigUpgradeTestLib )


atlas_add_test( GraphView1 SCRIPT scripts/GraphView.sh
                ENVIRONMENT THREADS=1     )
atlas_add_test( GraphView2 SCRIPT scripts/GraphView.sh
                ENVIRONMENT THREADS=2     )
atlas_add_test( GraphView64 SCRIPT scripts/GraphView.sh
                ENVIRONMENT THREADS=64              )

atlas_add_test( creatingEVTest SCRIPT ../../TrigSteer/ViewAlgsTest/scripts/creatingEVTest.sh )

atlas_add_test( mergeTest SCRIPT scripts/mergeTest.sh)

atlas_add_test( IDRunMC     SCRIPT scripts/IDRunMC.sh
                PROPERTIES TIMEOUT 500 
              )

atlas_add_test( IDRunData     SCRIPT scripts/IDRunData.sh
                PROPERTIES TIMEOUT 500 
              )

atlas_add_test( caloRunData   SCRIPT scripts/caloRunData.sh
                PROPERTIES TIMEOUT 500 
              )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
