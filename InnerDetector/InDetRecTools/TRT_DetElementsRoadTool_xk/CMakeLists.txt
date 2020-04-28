################################################################################
# Package: TRT_DetElementsRoadTool_xk
################################################################################

# Declare the package name:
atlas_subdir( TRT_DetElementsRoadTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions)
                          
# Component(s) in the package:
atlas_add_component( TRT_DetElementsRoadTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkSurfaces AthenaPoolUtilities TrkExInterfaces MagFieldElements MagFieldConditions)

# Install files from the package:
atlas_install_headers( TRT_DetElementsRoadTool_xk )

