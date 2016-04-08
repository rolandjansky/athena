################################################################################
# Package: VP1BPhysSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1BPhysSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          graphics/VP1/VP1Base
                          graphics/VP1/VP1Systems/VP1TrackSystems
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

include_directories( /usr/X11R6/include )

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( VP1BPhysSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1BPhysSystems
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives VP1Base VP1TrackSystems GL StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${COIN3D_LIBRARIES} EventInfo Particle TrkParameters TrkTrack TrkExInterfaces VP1Utils )

