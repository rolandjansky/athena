################################################################################
# Package: MuonRecValidationNtuples
################################################################################

# Declare the package name:
atlas_subdir( MuonRecValidationNtuples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventPrimitives
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          Reconstruction/MuonIdentification/MuGirlStau
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MuonRecValidationNtuples
                   src/*.cxx
                   PUBLIC_HEADERS MuonRecValidationNtuples
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives MuonLayerEvent TrkMeasurementBase MuGirlStauLib MuonIdHelpersLib )

atlas_add_executable( MuonInsideOutValidation
                      exe/MuonInsideOutValidation.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives MuonLayerEvent MuGirlStauLib TrkMeasurementBase MuonIdHelpersLib MuonRecValidationNtuples )

