################################################################################
# Package: SiDetElementsRoadTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiDetElementsRoadTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
			  InnerDetector/InDetRecEvent/SiSPSeededTrackFinderData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions )

# Component(s) in the package:
atlas_add_component( SiDetElementsRoadTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel InDetReadoutGeometry SiSPSeededTrackFinderData PixelReadoutGeometry SCT_ReadoutGeometry InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkSurfaces AthenaPoolUtilities EventInfo TrkPrepRawData TrkExInterfaces MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_headers( SiDetElementsRoadTool_xk )

