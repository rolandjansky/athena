################################################################################
# Package: MSVertexUtils
################################################################################

# Declare the package name:
atlas_subdir( MSVertexUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MSVertexUtils
                   src/*.cxx
                   PUBLIC_HEADERS MSVertexUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES MuonPrepRawData )

