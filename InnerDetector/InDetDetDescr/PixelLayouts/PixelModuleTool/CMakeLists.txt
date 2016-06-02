################################################################################
# Package: PixelModuleTool
################################################################################

# Declare the package name:
atlas_subdir( PixelModuleTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelGeoComponent
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelGeoModelModule
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelInterfaces
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelLayoutUtils
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_component( PixelModuleTool
                     src/*.cxx
                     src/components/*.cxx
		     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel PixelGeoComponent PixelGeoModelModule PixelLayoutUtils                     
		     PRIVATE_LINK_LIBRARIES StoreGateLib InDetReadoutGeometry PathResolver)

# Install files from the package:
atlas_install_headers( PixelModuleTool )

