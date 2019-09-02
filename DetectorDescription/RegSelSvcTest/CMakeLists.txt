################################################################################
# Package: RegSelSvcTest
################################################################################

# Declare the package name:
atlas_subdir( RegSelSvcTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          DetectorDescription/IRegionSelector
                          Control/AthenaBaseComps
			              Trigger/TrigEvent/TrigSteeringEvent
			              AtlasTest/TestTools )

# Component(s) in the package:
atlas_add_component( RegSelSvcTest
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel IRegionSelector AthenaBaseComps TrigSteeringEvent TestTools )

atlas_add_test( RegSel_dump
   SCRIPT test/test_RegSel_dump.sh
   PROPERTIES TIMEOUT 1200 )

atlas_add_test( RegSel_mt
   SCRIPT test/test_RegSel_mt.sh
   PROPERTIES TIMEOUT 1200 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

