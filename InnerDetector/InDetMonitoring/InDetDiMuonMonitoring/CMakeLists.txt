################################################################################
# Package: InDetDiMuonMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetDiMuonMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_component( InDetDiMuonMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaMonitoringLib xAODMuon xAODTracking GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( InDetDiMuonMonitoring )
atlas_install_joboptions( share/*.py )

