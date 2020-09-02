################################################################################
# Package: TrigCostMonitorMT
################################################################################

# Declare the package name:
atlas_subdir( TrigCostMonitorMT )

find_package(TBB)

# Component(s) in the package:
atlas_add_library( TrigCostMonitorMTLib
                   TrigCostMonitorMT/*.h
                   src/AlgorithmIdentifier.cxx
                   PUBLIC_HEADERS TrigCostMonitorMT
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib xAODTrigger AthViews )

atlas_add_component( TrigCostMonitorMT
                     src/*.cxx
                     src/components/*.cxx 
                     INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TBB_LIBRARIES} AthContainers AthViews AthenaKernel TrigConfHLTData TrigCostMonitorMTLib TrigSteeringEvent TrigTimeAlgsLib )


atlas_add_test( AlgorithmIdentifier_test
     SOURCES test/AlgorithmIdentifier_test.cxx
     LINK_LIBRARIES TrigCostMonitorMTLib TestTools StoreGateLib AthenaKernel GaudiKernel SGTools )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
