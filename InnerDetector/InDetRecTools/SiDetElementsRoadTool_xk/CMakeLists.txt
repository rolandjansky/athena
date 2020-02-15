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
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_component( SiDetElementsRoadTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel InDetReadoutGeometry PixelReadoutGeometry InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkSurfaces AthenaPoolUtilities EventInfo TrkPrepRawData TrkExInterfaces )

# Install files from the package:
atlas_install_headers( SiDetElementsRoadTool_xk )

