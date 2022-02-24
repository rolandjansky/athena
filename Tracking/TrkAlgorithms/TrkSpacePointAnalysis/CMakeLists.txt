#   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: TrkSpacePointAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TrkSpacePointAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/ReadoutGeometryBase
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkEvent/TrkSpacePoint
                          )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TrkSpacePointAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps ReadoutGeometryBase InDetIdentifier PixelReadoutGeometry SCT_ReadoutGeometry InDetReadoutGeometry TrkSpacePoint)

# Install files from the package:
atlas_install_joboptions( share/*.py )

