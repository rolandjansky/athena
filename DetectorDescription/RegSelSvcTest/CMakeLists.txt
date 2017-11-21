################################################################################
# Package: RegSelSvcTest
################################################################################

# Declare the package name:
atlas_subdir( RegSelSvcTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/RegSelLUT
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          Tools/PathResolver
			  Trigger/TrigEvent/TrigSteeringEvent
			  AtlasTest/TestTools )

# Component(s) in the package:
atlas_add_library( RegSelSvcTestLib
                   src/*.cxx
                   PUBLIC_HEADERS RegionSelector
                   PRIVATE_LINK_LIBRARIES SGTools IRegionSelector Identifier GaudiKernel StoreGateLib SGtests AthenaBaseComps AthenaPoolUtilities RegSelLUT PathResolver TrigSteeringEvent )

atlas_add_component( RegSelSvcTest
                     src/components/*.cxx
                     LINK_LIBRARIES SGTools IRegionSelector Identifier GaudiKernel AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities RegSelLUT PathResolver TrigSteeringEvent RegionSelectorLib RegSelSvcTestLib )

atlas_add_test( RegSel_dump SCRIPT test/test_RegSel_dump.sh
		PROPERTIES TIMEOUT 500
	      )		

atlas_add_test( RegSel_mt SCRIPT test/test_RegSel_mt.sh
		PROPERTIES TIMEOUT 1200
	      )		

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

