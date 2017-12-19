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


atlas_add_test( GraphView1 SCRIPT test/test_graph_view.sh
                ENVIRONMENT THREADS=1     )
atlas_add_test( GraphView2 SCRIPT test/test_graph_view.sh
                ENVIRONMENT THREADS=2     )
atlas_add_test( GraphView64 SCRIPT test/test_graph_view.sh
                ENVIRONMENT THREADS=64              )

# out untill we find a way to properly invoke tests from other packages
# atlas_add_test( creatingEVTest SCRIPT  forward.sh ViewAlgsTest/test/creatingEVTest.sh )

atlas_add_test( merge SCRIPT test/test_merge.sh)

atlas_add_test( IDRunMC     SCRIPT test/test_id_run_mc.sh
                PROPERTIES TIMEOUT 500 
              )

atlas_add_test( IDRunData     SCRIPT test/test_id_run_data.sh
                PROPERTIES TIMEOUT 500 
              )

#atlas_add_test( caloRunData   SCRIPT test/test_calo_run_data.sh
#                PROPERTIES TIMEOUT 500 
#              )
# replaced by complete egamma test

atlas_add_test( egammaRunData   SCRIPT test/test_egamma_run_data.sh
                PROPERTIES TIMEOUT 500 
              )

atlas_add_test( muRunData   SCRIPT test/test_mu_run_data.sh
                PROPERTIES TIMEOUT 500 
              )



atlas_add_test( idCaloRunData   SCRIPT test/test_id_calo_run_data.sh
                PROPERTIES TIMEOUT 500 
              )

atlas_add_test( EmuL1Decoding SCRIPT test/test_emu_l1_decoding.sh)
atlas_add_test( EmuStepProcessing SCRIPT test/test_emu_step_processing.sh)

atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
