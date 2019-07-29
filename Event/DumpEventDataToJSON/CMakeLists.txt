################################################################################
# Package: DumpEventDataToJson
################################################################################

# Declare the package name:
atlas_subdir( DumpEventDataToJSON )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTracking
   GaudiKernel 
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkDetDescr/TrkGeometry
   )

# External dependencies:
# find_package( Eigen )
find_package( nlohmann_json )

atlas_add_component( DumpEventDataToJSON
  src/*.cxx
   src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib xAODEventInfo xAODJet xAODMuon xAODTracking GaudiKernel nlohmann_json::nlohmann_json TrkGeometry)

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

atlas_add_test( flake8
 SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
 POST_EXEC_SCRIPT nopost.sh )
