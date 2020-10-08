################################################################################
# Package: InDetDiMuonMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetDiMuonMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( InDetDiMuonMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaMonitoringLib xAODMuon xAODTracking GaudiKernel CxxUtils StoreGateLib SGtests )

# Install files from the package:
atlas_install_joboptions( share/*.py )

