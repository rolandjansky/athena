################################################################################
# Package: MuonDQAMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonDQAMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( MuonDQAMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests GaudiKernel DataQualityInterfaces EventInfo TrigT1Result )

# Install files from the package:
atlas_install_headers( MuonDQAMonitoring )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
