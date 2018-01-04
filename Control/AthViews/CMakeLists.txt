################################################################################
# Package: AthViews
################################################################################

find_package( TBB )

# Declare the package name:
atlas_subdir( AthViews )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Trigger/TrigEvent/TrigSteeringEvent
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
			  AtlasTest/TestTools )

# Component(s) in the package:
atlas_add_library( AthViews
                   src/*.cxx
                   PUBLIC_HEADERS AthViews
                   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TBB_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib GaudiKernel CxxUtils TrigSteeringEvent )

atlas_add_component( AthViewsAlgs
                   src/components/*.cxx
                   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TBB_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib GaudiKernel CxxUtils AthViews TrigSteeringEvent )

atlas_add_component( AthViewsDFlow
                     src_dflow/*.cxx
                     src_dflow/components/*.cxx
                     INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TBB_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib GaudiKernel CxxUtils AthViews )


atlas_add_test( ViewLinking_test 
		INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
		SOURCES test/ViewLinking_test.cxx 
		LINK_LIBRARIES ${TBB_LIBRARIES}
		AthViews TestTools )

# Install files from the package:
atlas_install_headers( AthViews )
atlas_install_joboptions( share/*.py )

